/* This file is part of Into.
 * Copyright (C) Intopii
 * All rights reserved.
 */

#include "genicam_wrapper.h"

#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>

#include <pylon/PylonIncludes.h>
#include <pylon/gige/BaslerGigECamera.h>

typedef Pylon::CBaslerGigECamera PylonCameraType;
typedef PylonCameraType::StreamGrabber_t PylonGrabberType;

using namespace Pylon;
using namespace Basler_GigECameraParams;
using namespace Basler_GigEStreamParams;
using namespace std;

struct genicam_device
{
  genicam_device(PylonCameraType* c,
               PylonGrabberType* g) :
    camera(c), grabber(g)
  {}
               
  PylonCameraType *camera;
  PylonGrabberType *grabber;
  std::vector<void*> vecHandles;
  std::vector<void*> vecReservedHandles;
  int triggerMode;
};

static int iInstanceCount = 0;
static std::vector<genicam_device*> vecDevices;
static std::mutex errorMutex;
static std::string strLastError;
static ITransportLayer* pTransportLayer = 0;

static void print_values(genicam_device* device)
{
  std::cout << std::endl;
  std::cout << "Camera values" << std::endl;
  std::cout << "*************" << std::endl;

  // General
  std::cout << "AcquisitionMode: " << device->camera->AcquisitionMode.ToString() << std::endl;
  std::cout << "TriggerSelector: " << device->camera->TriggerSelector.ToString() << std::endl;
  std::cout << "TriggerMode: " << device->camera->TriggerMode.ToString() << std::endl;
  std::cout << "TriggerSource: " << device->camera->TriggerSource.ToString() << std::endl;
  std::cout << "TriggerActivation: " << device->camera->TriggerActivation.ToString() << std::endl;
  std::cout << "ExposureMode: " << device->camera->ExposureMode.ToString() << std::endl;

  if (device->camera->DeviceScanType.GetValue() != DeviceScanType_Areascan)
    { 
      // ShaftEncoderModuleMode
      std::cout << std::endl;
      std::cout << "ShaftEncoderModuleMode: "
                << device->camera->ShaftEncoderModuleMode.ToString() << std::endl;
      std::cout << "ShaftEncoderModuleCounterMode: "
                << device->camera->ShaftEncoderModuleCounterMode.ToString() << std::endl;
      std::cout << "ShaftEncoderModuleCounterMax: "
                << device->camera->ShaftEncoderModuleCounterMax.ToString() << std::endl;
      std::cout << "ShaftEncoderModuleReverseCounterMax: "
                << device->camera->ShaftEncoderModuleReverseCounterMax.ToString() << std::endl;
  
      // FrequencyConverter
      std::cout << std::endl;
      std::cout << "FrequencyConverterSignalAlignment: "
                << device->camera->FrequencyConverterSignalAlignment.ToString() << std::endl;
      std::cout << "FrequencyConverterInputSource: "
                << device->camera->FrequencyConverterInputSource.ToString() << std::endl;
      std::cout << "FrequencyConverterPreDivider: "
                << device->camera->FrequencyConverterPreDivider.ToString() << std::endl;
      std::cout << "FrequencyConverterMultiplier: "
                << device->camera->FrequencyConverterMultiplier.ToString() << std::endl;
      std::cout << "FrequencyConverterPostDivider: "
                << device->camera->FrequencyConverterPostDivider.ToString() << std::endl;
    }
}

static void set_error(const std::string& msg)
{
  errorMutex.lock();
  strLastError = msg;
  errorMutex.unlock();
}

static void set_error(const std::string& msg, const GenICam::GenericException e)
{
  errorMutex.lock();
  strLastError = msg;
  if (e.GetDescription())
    strLastError += std::string(" ") + e.GetDescription();
  errorMutex.unlock();
}

GENICAM_WAPI(void) genicam_last_error(char* bfr)
{
  errorMutex.lock();
  strncpy(bfr, strLastError.c_str(), GENICAM_ERROR_MSG_SIZE-1);
  bfr[GENICAM_ERROR_MSG_SIZE - 1] = 0;
  errorMutex.unlock();
}

GENICAM_WAPI(int) genicam_initialize(void)
{
  if (++iInstanceCount == 1)
    {
      try
        {
          Pylon::PylonInitialize();
          pTransportLayer = CTlFactory::GetInstance().CreateTl(PylonCameraType::DeviceClass());
        }
      catch (GenICam::GenericException& e)
        {
          set_error("Failed to initialize Pylon driver.", e);
          return 1;
        }
    }
  return 0;
}

GENICAM_WAPI(int) genicam_terminate(void)
{
  int retValue = 0;
  if (--iInstanceCount == 0)
    {
      while (vecDevices.size() > 0)
        {
          try
            {
              genicam_close_device(vecDevices.at(0));
            }
          catch (GenICam::GenericException& e)
            {
              set_error("Failed to close Pylon device.", e);
              retValue = 1;
            }
        }
      
      try
        {
          CTlFactory::GetInstance().ReleaseTl(pTransportLayer);
          pTransportLayer = 0;
          Pylon::PylonTerminate();
        }
      catch (GenICam::GenericException& e)
        {
          set_error("Failed to terminate Pylon driver.", e);
          retValue = 1;
        }
    }

  return retValue;
}

static void append_string(char** names, const char* name, int* len, int* bufferSize)
{
  int iNameLen = strlen(name);
  if (*names == 0)
    {
      *names = (char*)malloc(iNameLen + 2);
      *len = 1;
    }
  else if (*bufferSize <= *len + iNameLen)
    {
      *bufferSize = *len + iNameLen + 1;
      *names = (char*)realloc(*names, *bufferSize);
    }
  memcpy(*names + *len - 1, name, iNameLen + 1);
  *len += iNameLen + 1;
  (*names)[*len-1] = 0;
}


GENICAM_WAPI(int) genicam_list_cameras(char **names, int* count)
{
  *names = 0;
  *count = 0;

  int iLen = 0, iBufferSize = 0;
  try
    {
      // Get all attached cameras
      DeviceInfoList_t lstDevices;
      if (pTransportLayer->EnumerateDevices(lstDevices) != 0)
        {
          DeviceInfoList_t::iterator it;
          for (it=lstDevices.begin(); it!=lstDevices.end(); ++it )
            {
              append_string(names, it->GetSerialNumber().c_str(), &iLen, &iBufferSize);
              ++*count;
            }
        }
    }
  catch (GenICam::GenericException& e)
    {
      set_error("Failed to list cameras.", e);
      *count = 0;
      *names = 0;
      return 1;
    }
  return 0;
}

GENICAM_WAPI(const char*) genicam_next_camera(const char* camera)
{
  camera += strlen(camera) + 1;
  if (*camera == 0)
    return 0;
  return camera;
}

GENICAM_WAPI(int) genicam_open_device(const char* serial, genicam_device** device)
{
  *device = 0;
  
  // Get all attached cameras and throw exception if no camera is found
  DeviceInfoList_t devices;
  try
    {
      if (pTransportLayer->EnumerateDevices(devices) == 0)
        {
          set_error("No cameras were found.");
          return 1;
        }
    }
  catch (GenICam::GenericException& e)
    {
      set_error("Failed to enumerate devices.", e);
      return 1;
    }

  PylonCameraType* pCamera = 0;
  // Create the camera object of the selected camera
  // The camera object is used to set and get all available
  // camera features.
  try
    {
      for (DeviceInfoList_t::iterator i = devices.begin(); i != devices.end(); ++i)
        {
          if (!strcmp(i->GetSerialNumber().c_str(), serial))
            {
              IPylonDevice *pDevice = pTransportLayer->CreateDevice(*i);
              CDeviceInfo info = pDevice->GetDeviceInfo();
              
              // Create camera object
              pCamera = new PylonCameraType(pDevice);
              break;
            }
        }
    }
  catch (GenICam::GenericException& e)
    {
      set_error("Failed to create camera object.", e);
      return 1;
    }

  if (pCamera == 0)
    {
      set_error("No camera matching the given camera id was found.");
      return 1;
    }

  unsigned int uiGrabbers = 0;
  try
    {
      uiGrabbers = pCamera->GetNumStreamGrabberChannels();
    }
  catch (GenICam::GenericException& e)
    {
      set_error("Failed to get stream grabbers count.", e);
      delete pCamera;
      return 1;
    }
  
  if (uiGrabbers <= 0)
    {
      delete pCamera;
      set_error("Couldn't find any stream grabbers.");
      return 1;
    }
      
  // Get the first stream grabber object of the selected camera
  PylonGrabberType* pGrabber = new PylonGrabberType(pCamera->GetStreamGrabber(0));

  try
    {
      // Open the camera
      pCamera->Open();

      // Initialize default values for all trigger modes
      pCamera->AcquisitionMode.SetValue(AcquisitionMode_Continuous);
      pCamera->ExposureMode.SetValue(ExposureMode_Timed);
		
	  if (pCamera->DeviceScanType.GetValue() != DeviceScanType_Areascan)
	 { 
      // Initialize ShaftEncoderModule
      pCamera->ShaftEncoderModuleLineSelector.SetValue(ShaftEncoderModuleLineSelector_PhaseA);
      pCamera->ShaftEncoderModuleLineSource.SetValue(ShaftEncoderModuleLineSource_Line1);
      pCamera->ShaftEncoderModuleLineSelector.SetValue(ShaftEncoderModuleLineSelector_PhaseB);
      pCamera->ShaftEncoderModuleLineSource.SetValue(ShaftEncoderModuleLineSource_Line2);
      pCamera->ShaftEncoderModuleCounterMode.SetValue(ShaftEncoderModuleCounterMode_FollowDirection);
      pCamera->ShaftEncoderModuleMode.SetValue(ShaftEncoderModuleMode_ForwardOnly);
      pCamera->ShaftEncoderModuleCounterMax.SetValue(32767);
      pCamera->ShaftEncoderModuleReverseCounterMax.SetValue(0);

      // Initialize FrequencyConverter
      pCamera->FrequencyConverterSignalAlignment.SetValue(FrequencyConverterSignalAlignment_RisingEdge);
	  }
    }
  catch (GenICam::GenericException& e)
    {
      set_error("Failed to open/initialize camera.", e);
      delete pCamera;
      return 1;
    }
      
  try
    {
      // Open the stream grabber
      pGrabber->Open();
    }
  catch (GenICam::GenericException& e)
    {
      set_error("Failed to open grabber.", e);
      delete pCamera;
      delete pGrabber;
      return 1;
    }


  *device = new genicam_device(pCamera, pGrabber);
  genicam_set_property(*device, "triggerMode", 2);
  genicam_set_property(*device, "triggerLine", 0);
  genicam_set_property(*device, "triggerRate", 1);
  vecDevices.push_back(*device);
  return 0;
}

GENICAM_WAPI(int) genicam_free(void* data)
{
	free(data);
  return 0;
}

GENICAM_WAPI(int) genicam_close_device(genicam_device* device)
{
  int retValue = 0;
  try
    {
      genicam_deregister_framebuffers(device);
      
      // Free all resources used for grabbing
      device->grabber->Close();
      device->camera->Close();
    }
  catch (GenICam::GenericException& e)
    {
      set_error("Failed to close device.", e);
      retValue = 1;
    }
  
  delete device->grabber;
  delete device->camera;
  delete device;
  
  vecDevices.erase(std::find(vecDevices.begin(), vecDevices.end(), device));

  return retValue;
}

GENICAM_WAPI(int) genicam_reconnect_device(genicam_device*)
{
  return 0;
}

GENICAM_WAPI(int) genicam_register_framebuffers(genicam_device* device, unsigned char* buffer, int count)
{
  try
    {
      device->vecHandles.resize(count,0);
      
      // Create an image buffer
      unsigned uiImageSize = (unsigned)device->camera->PayloadSize();
      
      // Init grabber parameters
      device->grabber->MaxBufferSize.SetValue(uiImageSize);
      device->grabber->MaxNumBuffer.SetValue(count);
      
      // Allocate all resources for grabbing. Critical parameters like
      // image size must not be changed until FinishGrab() is called.
      device->grabber->PrepareGrab();
      
      // Buffers used for grabbing must be registered at the stream
      // grabber. The registration returns a handle to be used for
      // queuing the buffer
      for (int i=0; i<count; ++i)
        {
          device->vecHandles[i] = device->grabber->RegisterBuffer(buffer + i*uiImageSize, uiImageSize);
          device->grabber->QueueBuffer(device->vecHandles[i], 0);
        }
    }
  catch (GenICam::GenericException& e)
    {
      set_error("Failed to register frame buffers.", e);
      return 1;
    }

  return 0;
}

GENICAM_WAPI(int) genicam_deregister_framebuffers(genicam_device* device)
{
  try
    {
      device->grabber->CancelGrab();
      
      // Get all buffers back
      for (GrabResult r; device->grabber->RetrieveResult(r););
      
      // Deregister handles
      for (unsigned int i=0; i<device->vecHandles.size(); ++i)
        {
          if (device->vecHandles[i] != 0)
            device->grabber->DeregisterBuffer(device->vecHandles[i]);
        }
      
      device->vecHandles.clear();
      device->vecReservedHandles.clear();

      device->grabber->FinishGrab();
    }
    catch (GenICam::GenericException& e)
      {
        set_error("Failed to deregister frame buffers.", e);
        return 1;
      }

  return 0;
}

GENICAM_WAPI(int) genicam_grab_frame(genicam_device* device, unsigned char** buffer, int timeout)
{
  GrabResult grabResult;
  *buffer = 0;

  try
    {
      // If triggerMode == SoftwareTrigger, we must call TriggerSoftware
      // here and calculate timeout to be sure we can grab the frame
      if (device->triggerMode == 0)
        {
          // First we 'trig' the camera
          device->camera->TriggerSoftware.Execute();
          
          // Then calculate timeout
          if (device->camera->DeviceScanType.GetValue() == DeviceScanType_Areascan)
            {
              int expTime = 0;
              if (genicam_get_property(device, "exposureTime", &expTime) != 0)
                return 1;
              
              timeout = expTime * 1000 + 500; //expTime + 500ms
            }
          else //LineScan
            {
              int linePeriod = 0;
              int height = 0;
              if (genicam_get_property(device, "exposurePeriod", &linePeriod) != 0 ||
                  genicam_get_property(device, "height", &height) != 0)
                return 1;
              
              timeout = linePeriod * height + 500;
            }
        }
  
      if (device->grabber->GetWaitObject().Wait(timeout))
        {
          //Get an item from the grabber's output queue
          if (device->grabber->RetrieveResult(grabResult))
            {
              device->vecReservedHandles.push_back(grabResult.Handle());
              if (!grabResult.Succeeded())
                {
                  set_error(std::string("Failed to grab frame. ") + 
                            grabResult.GetErrorDescription().c_str());
                  return 2;
                }
            }
          else
            {
              set_error("Failed to retrieve an item from the output queue.");
              return 2;
            }
        }
      else
        {
          set_error("Timeout occurred when waiting for a grabbed image.");
          return 0;
        }
    }
  catch (GenICam::GenericException& e)
    {
      set_error("Failed to grab frame.", e);
      return 1;
    }

  *buffer = (unsigned char*)grabResult.Buffer();
  return 0; //(unsigned char*)grabResult.Buffer();
}

GENICAM_WAPI(int) genicam_requeue_buffers(genicam_device* device)
{
  try
    {
      for (unsigned int i=0; i<device->vecReservedHandles.size(); ++i)
        device->grabber->QueueBuffer(device->vecReservedHandles[i], 0);
      device->vecReservedHandles.clear();
    }
  catch (GenICam::GenericException& e)
    {
      set_error("Failed to requeue buffers.", e);
      return 1;
    }

  return 0;
}

GENICAM_WAPI(int) genicam_start_capture(genicam_device* device)
{
  print_values(device);
  try
    {
      // Reset the shaft encoder module counter and the shaft encoder
      // module reverse counter if the triggerMode == HardwareTrigger
      if (device->triggerMode == 1)
        {
          device->camera->ShaftEncoderModuleCounterReset.Execute();
          device->camera->ShaftEncoderModuleReverseCounterReset.Execute();
        }

      device->camera->AcquisitionStart.Execute();
    }
  catch (GenICam::GenericException& e)
    {
      set_error("Failed to start capture.", e);
      return 1;
    }
  
  return 0;
}

GENICAM_WAPI(int) genicam_stop_capture(genicam_device* device)
{
  try
    {
      device->camera->AcquisitionStop.Execute();
    }
  catch (GenICam::GenericException& e)
    {
      set_error("Failed to stop capture.", e);
      return 1;
    }

  return 0;
}

GENICAM_WAPI(int) genicam_set_property(genicam_device* device, const char* name, int value)
{
  try
    {
      if (!strcmp(name, "width"))
        device->camera->Width.SetValue(value);
      else if (!strcmp(name, "height"))
        device->camera->Height.SetValue(value);
      else if (!strcmp(name, "frameRate"))
        {
          if (device->camera->DeviceScanType.GetValue() == DeviceScanType_Areascan)
            {
              device->camera->AcquisitionFrameRateEnable.SetValue(value <= 0 ? false : true);
              if (value > 0)
                device->camera->AcquisitionFrameRateAbs.SetValue(double(value) / 1000.0);
            }
        }
      else if (!strcmp(name, "triggerMode"))
        {
          std::cout << "triggerMode " << value << std::endl;
          /**
           * SoftwareTrigger = 0,
           * HardwareTrigger,
           * FreeRun
           */
          switch(value)
            {
            case 0: //SoftwareTrigger
              std::cout << "Set TriggerSelector_FrameStart = " << (int)TriggerSelector_FrameStart << std::endl;
              device->camera->TriggerSelector.SetValue(TriggerSelector_FrameStart);
              std::cout << "TriggerSelector.GetValue() = " << device->camera->TriggerSelector.GetValue() << std::endl;
              device->camera->TriggerMode.SetValue(TriggerMode_On);
              device->camera->TriggerSource.SetValue(TriggerSource_Software);
              device->triggerMode = 0;
              break;
            case 1: //HardwareTrigger
              device->camera->TriggerSelector.SetValue(TriggerSelector_LineStart);
              device->camera->TriggerMode.SetValue(TriggerMode_On);
              device->camera->TriggerSource.SetValue(TriggerSource_FrequencyConverter);
              device->camera->TriggerActivation.SetValue(TriggerActivation_RisingEdge);
              device->triggerMode = 1;
              break;
            default: //FreeRun
              // Set the camera to continuous frame mode
              device->camera->TriggerSelector.SetValue(TriggerSelector_FrameStart);
              device->camera->TriggerMode.SetValue(TriggerMode_Off);
              device->triggerMode = 2;
              break;
            }
        }
      else if (!strcmp(name, "triggerLine"))
        {
          std::cout << "triggerLine " << value << std::endl;
          switch (value)
            {
            case 0:
              device->camera->FrequencyConverterInputSource.SetValue(FrequencyConverterInputSource_Line1);
              break;
            case 1:
              device->camera->FrequencyConverterInputSource.SetValue(FrequencyConverterInputSource_Line2);
              break;
            case 2:
              device->camera->FrequencyConverterInputSource.SetValue(FrequencyConverterInputSource_Line3);
              break;
            case 3:
              device->camera->FrequencyConverterInputSource.SetValue(FrequencyConverterInputSource_Line4);
              break;
            case 4:
              device->camera->FrequencyConverterInputSource.SetValue(FrequencyConverterInputSource_Line5);
              break;
            case 5:
              device->camera->FrequencyConverterInputSource.SetValue(FrequencyConverterInputSource_Line6);
              break;
            case 6:
              device->camera->FrequencyConverterInputSource.SetValue(FrequencyConverterInputSource_Line7);
              break;
            case 7:
              device->camera->FrequencyConverterInputSource.SetValue(FrequencyConverterInputSource_Line8);
              break;
            default:
              device->camera->FrequencyConverterInputSource.SetValue(FrequencyConverterInputSource_ShaftEncoderModuleOut);
              break;
            }
        }
      else if (!strcmp(name, "triggerRate"))
        {
          std::cout << "triggerRate " << value << std::endl;
          device->camera->FrequencyConverterPreDivider.SetValue(1);
          if (value == 1)
            {
              device->camera->FrequencyConverterMultiplier.SetValue(1);
              device->camera->FrequencyConverterPostDivider.SetValue(1);
            }
          else
            {
              double dRatio = double(value) / 10000.0;
              double dSmallestError = dRatio;
              int iBestDivider = 1, iBestMultiplier = 1;
              // Scan the whole allowable range for best divider/multiplier combination.
              for (int iDivider=1; iDivider<=128; ++iDivider)
                {
                  for (int iMultiplier=1; iMultiplier<=32; ++iMultiplier)
                    {
                      double dError = double(iMultiplier)/iDivider - dRatio;
                      if (dError < 0) dError = -dError; // no fabs() to reduce dependencies
                      
                      if (dError < dSmallestError)
                        {
                          dSmallestError = dError;
                          iBestMultiplier = iMultiplier;
                          iBestDivider = iDivider;
                        }
                    }
                }
              std::cout << "Requested ratio: " << dRatio << ", got " << double(iBestMultiplier)/iBestDivider
                        << " (" << iBestMultiplier << '/' << iBestDivider << ")\n";
              
              device->camera->FrequencyConverterPreventOvertrigger.SetValue(1);
              device->camera->FrequencyConverterMultiplier.SetValue(iBestMultiplier);
              device->camera->FrequencyConverterPostDivider.SetValue(iBestDivider);
              return 1;
            }
        }
      else if (!strcmp(name, "imageFormat"))
        {
          /**
             InvalidFormat = 0,
             MonoFormat = 1,
             BayerRGGBFormat,
             BayerBGGRFormat,
             BayerGBRGFormat,
             BayerGRBGFormat,
             RgbFormat = 16,
             BgrFormat
          */
          int format;
          switch(value)
            {
            case 2: format = PixelFormat_BayerRG8; break;
            case 3: format = PixelFormat_BayerBG8; break;
            case 4: format = PixelFormat_BayerGB8; break;
            case 5: format = PixelFormat_BayerGR8; break;
            default: format = PixelFormat_Mono8; break;
            }
          device->camera->PixelFormat.SetValue((PixelFormatEnums)format);
        }
      else if (!strcmp(name, "exposure") || !strcmp(name, "exposureTime"))
        {
          int min = device->camera->ExposureTimeRaw.GetMin();
          int max = device->camera->ExposureTimeRaw.GetMax();

          // For some reason exposureTime in PylonAreascan cameras must be divisible by 35
          if (device->camera->DeviceScanType.GetValue() == DeviceScanType_Areascan)
            {
              int mod = value % 35;
              if (mod != 0)
                {
                  if (mod > 17)
                    value = value + (35-mod);
                  else
                    value = value - mod;
                }
            }

          if (value < min) value = min;
          if (value > max) value = max;
          
          device->camera->ExposureTimeRaw.SetValue(value);
        }
      else if (!strcmp(name, "exposurePeriod"))
        {
          // PENDING
        }
      else if (!strcmp(name, "gain"))
        device->camera->GainRaw.SetValue(value);
      else if (!strcmp(name, "offsetX"))
        device->camera->OffsetX.SetValue(value);
      else if (!strcmp(name, "offsetY"))
        device->camera->OffsetY.SetValue(value);
      else if (!strcmp(name, "packetSize"))
        device->camera->GevSCPSPacketSize.SetValue(value);
      else if (!strcmp(name, "flipHorizontally"))
        device->camera->ReverseX.SetValue(value == 0 ? false : true);
      else if (!strcmp(name, "autoExposureTarget"))
        {
          if (value > 0)
            {
              device->camera->ExposureAuto.SetValue(ExposureAuto_Continuous);
              device->camera->AutoTargetValue.SetValue(value);
            }
          else
            device->camera->ExposureAuto.SetValue(ExposureAuto_Off);
        }
      else if (!strcmp(name, "autoExposureAreaOffsetX"))
        device->camera->AutoFunctionAOIOffsetX.SetValue(value);
      else if (!strcmp(name, "autoExposureAreaOffsetY"))
        device->camera->AutoFunctionAOIOffsetY.SetValue(value);
      else if (!strcmp(name, "autoExposureAreaWidth"))
        device->camera->AutoFunctionAOIWidth.SetValue(value);
      else if (!strcmp(name, "autoExposureAreaHeight"))
        device->camera->AutoFunctionAOIHeight.SetValue(value);
    }
  catch (GenICam::GenericException& e)
    {
      set_error(std::string("Failed to write ") + name + '.', e);
      return 1;
    }

  return 0;
}

GENICAM_WAPI(int) genicam_get_property(genicam_device* device, const char* name, int *value)
{
  try
    {
      if (!strcmp(name, "exposure") || !strcmp(name, "exposureTime"))
        *value = (int)device->camera->ExposureTimeRaw.GetValue();
      else if (!strcmp(name, "exposurePeriod"))
        {
          // PENDING
        }
      else if (!strcmp(name, "frameRate"))
        {
          if (device->camera->DeviceScanType.GetValue() == DeviceScanType_Areascan &&
              device->camera->AcquisitionFrameRateEnable.GetValue() == true)
            *value = (int)(device->camera->AcquisitionFrameRateAbs.GetValue() * 1000);
          else
            *value = 0;
        }
      else if (!strcmp(name, "cameraType"))
        {
          if (device->camera->DeviceScanType.GetValue() == DeviceScanType_Areascan)
            *value = 0;
          else
            *value = 1;
        }
      else if (!strcmp(name, "imageFormat"))
        {
          /**
             InvalidFormat = 0,
             MonoFormat = 1,
             BayerRGGBFormat,
             BayerBGGRFormat,
             BayerGBRGFormat,
             BayerGRBGFormat,
             RgbFormat = 16,
             BgrFormat
          */
          
          int format = device->camera->PixelFormat.GetValue();
          switch (format)
            {
            case PixelFormat_BayerRG8: *value = 2; break;
            case PixelFormat_BayerBG8: *value = 3; break;
            case PixelFormat_BayerGB8: *value = 4; break;
            case PixelFormat_BayerGR8: *value = 5; break;
            case PixelFormat_Mono8: *value = 1; break;
            default :*value = 0; break;
            }
        }
      else if (!strcmp(name, "bitsPerPixel"))
        {
          int pixelSize = device->camera->PixelSize.GetValue();
          switch(pixelSize)
            {
            case PixelSize_Bpp8: *value = 8; break;
            case PixelSize_Bpp12: *value = 12; break;
            case PixelSize_Bpp14: *value = 14; break;
            case PixelSize_Bpp16: *value = 16; break;
            case PixelSize_Bpp24: *value = 24; break;
            case PixelSize_Bpp32: *value = 32; break;
            case PixelSize_Bpp36: *value = 36; break;
            case PixelSize_Bpp48: *value = 48; break;
            case PixelSize_Bpp64: *value = 64; break;
            default: *value = 8; break;
            }
        }
      else if (!strcmp(name, "triggerMode"))
        return device->triggerMode;
      else if (!strcmp(name, "triggerLine"))
        {
          switch (device->camera->FrequencyConverterInputSource.GetValue())
            {
            case FrequencyConverterInputSource_Line1: *value=0; break;
            case FrequencyConverterInputSource_Line2: *value=0; break;
            case FrequencyConverterInputSource_Line3: *value=0; break;
            case FrequencyConverterInputSource_Line4: *value=0; break;
            case FrequencyConverterInputSource_Line5: *value=0; break;
            case FrequencyConverterInputSource_Line6: *value=0; break;
            case FrequencyConverterInputSource_Line7: *value=0; break;
            case FrequencyConverterInputSource_Line8: *value=0; break;
            case FrequencyConverterInputSource_ShaftEncoderModuleOut: *value=-1; break;
            default: *value=-2; break;
            }
        }
      else if (!strcmp(name, "triggerRate"))
        {
          return int(double(device->camera->FrequencyConverterMultiplier.GetValue()) /
                     device->camera->FrequencyConverterPostDivider.GetValue() * 10000 + 0.5);
        }
      else if (!strcmp(name, "inputPulseFrequency"))
        {
          // PENDING
        }
      else if (!strcmp(name, "cameraType"))
        *value = device->camera->DeviceScanType.GetValue() == DeviceScanType_Areascan ? 0 : 1;
      else if (!strcmp(name, "gain"))
        *value = (int)device->camera->GainRaw.GetValue();
      else if (!strcmp(name, "gain$min"))
        *value = (int)device->camera->GainRaw.GetMin();
      else if (!strcmp(name, "gain$max"))
        *value = (int)device->camera->GainRaw.GetMax();
      else if (!strcmp(name, "offsetX"))
        *value = (int)device->camera->OffsetX.GetValue();
      else if (!strcmp(name, "offsetY"))
        *value = (int)device->camera->OffsetY.GetValue();
      else if (!strcmp(name, "width"))
        *value = (int)device->camera->Width.GetValue();
      else if (!strcmp(name, "width$min"))
        *value = (int)device->camera->Width.GetMin();
      else if (!strcmp(name, "width$max"))
        *value = (int)device->camera->WidthMax.GetValue();
      else if (!strcmp(name, "sensorWidth"))
        *value = (int)device->camera->SensorWidth.GetValue();
      else if (!strcmp(name, "height"))
        *value = (int)device->camera->Height.GetValue();
      else if (!strcmp(name, "height$min"))
        *value = (int)device->camera->Height.GetMin();
      else if (!strcmp(name, "height$max"))
        {
          *value = (int)device->camera->HeightMax.GetValue();

          // We restrict the maximum height of the LineScan -cameras if necessary
          if (device->camera->DeviceScanType.GetValue() != DeviceScanType_Areascan)
            {
              int width2 = (int)(device->camera->WidthMax.GetValue() / 2);
              if (width2 < *value)
                *value = width2;
            }
        }
      else if (!strcmp(name, "sensorHeight"))
        *value = (int)device->camera->SensorHeight.GetValue();
      else if (!strcmp(name, "payloadSize"))
        *value = (int)device->camera->PayloadSize();
      else if (!strcmp(name, "packetSize"))
        *value = (int)device->camera->GevSCPSPacketSize.GetValue();
      else if (!strcmp(name, "flipHorizontally"))
        *value = (int)device->camera->ReverseX.GetValue();
      else if (!strcmp(name, "autoExposureTarget"))
        {
          if (device->camera->ExposureAuto.GetValue() == ExposureAuto_Continuous)
            *value = (int)device->camera->AutoTargetValue.GetValue();
          else
            *value = 0;
        }
      else if (!strcmp(name, "autoExposureAreaOffsetX"))
        *value = (int)device->camera->AutoFunctionAOIOffsetX.GetValue();
      else if (!strcmp(name, "autoExposureAreaOffsetY"))
        *value = (int)device->camera->AutoFunctionAOIOffsetY.GetValue();
      else if (!strcmp(name, "autoExposureAreaWidth"))
        *value = (int)device->camera->AutoFunctionAOIWidth.GetValue();
      else if (!strcmp(name, "autoExposureAreaHeight"))
        *value = (int)device->camera->AutoFunctionAOIHeight.GetValue();
    }
  catch (GenICam::GenericException& e)
    {
      set_error(std::string("Failed to read ") + name + '.', e);
      return 1;
    }
  return 0;
}
