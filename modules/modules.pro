INTODIR = ..
include($$INTODIR/extensions.pri)

TEMPLATE = subdirs

# Directories
SUBDIRS = base \
          camera \
          classification \
          colors \
          calibration \
          database \
          dsp \
          flowcontrol \
          geometry \
          image \
          io \
          matching \
          network \
          optimization \
          statistics \
          texture \
          tracking \
          transforms \
          video

enabled(opencv) {
  SUBDIRS += opencv
  opencv.depends += image
  calibration.depends += opencv
}

image.depends += dsp
geometry.depends += dsp
video.depends += image
colors.depends += image
classification.depends += image
texture.depends += image
transforms.depends += image
camera.depends += image geometry colors
matching.depends += geometry classification optimization
calibration.depends += classification optimization

# Camera drivers
qt {
  CAMERADRIVERS = emulator webcam pylon
  for(driver, CAMERADRIVERS) {
    !enabled($$driver):!equals(driver,emulator) {
      warning($$driver camera driver will not be built)
    } else {
      DRIVERID = camera_$$driver
      SUBDIRS += $$DRIVERID
      $${DRIVERID}.subdir = camera/$$driver
      $${DRIVERID}.depends = camera
    }
  }

  # Io drivers
  IODRIVERS = emulator modbus
  for(driver, IODRIVERS) {
    !enabled($$driver):!equals(driver,emulator) {
      warning($$driver I/O driver will not be built)
    } else {
      DRIVERID = io_$$driver
      SUBDIRS += $$DRIVERID
      $${DRIVERID}.subdir = io/$$driver
      $${DRIVERID}.depends = io
    }
  }
}
