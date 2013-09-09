/* This file is part of Into. 
 * Copyright (C) Intopii 2013.
 * All rights reserved.
 *
 * Licensees holding a commercial Into license may use this file in
 * accordance with the commercial license agreement. Please see
 * LICENSE.commercial for commercial licensing terms.
 *
 * Alternatively, this file may be used under the terms of the GNU
 * Affero General Public License version 3 as published by the Free
 * Software Foundation. In addition, Intopii gives you special rights
 * to use Into as a part of open source software projects. Please
 * refer to LICENSE.AGPL3 for details.
 */

#include "TestPiiVideoFileReader.h"
#include "TestPiiVideoFileWriter.h"
#include "TestPiiVideo.h"

#ifndef PII_NO_AVCODEC
#include <avcodec/PiiVideoReader.h>
#include <avcodec/PiiVideoWriter.h>

#include <PiiImageFileWriter.h>
#include <PiiImageFileReader.h>
#endif

#include <QtTest>

#include <PiiColor.h>
#include <PiiMatrix.h>


#if QT_VERSION < 0x050000
#  define AVCODEC_SKIP QSKIP("Avcoded support not enabled", SkipAll)
#else
#  define AVCODEC_SKIP QSKIP("Avcoded support not enabled")
#endif

void TestPiiVideoFileReader::initTestCase()
{
#ifndef PII_NO_AVCODEC
  QVERIFY(createOperation("piivideo", "PiiVideoFileReader"));
#else
  AVCODEC_SKIP;
#endif
}

void TestPiiVideoFileReader::readVideo()
{
#ifndef PII_NO_AVCODEC
  QVERIFY(start(ExpectFail));

  QString strVideo =  "data/ov14c1.avi";
  if(!QFile::exists(strVideo))
    QVERIFY(false);

  operation()->setProperty("fileName", strVideo);
  operation()->setProperty("imageType", "Color");
  
  QVERIFY(connectInput("trigger"));
  
  QVERIFY(start());
  QVERIFY(sendObject("trigger", 1));
  QVERIFY(hasOutputValue("image"));

  PiiMatrix<PiiColor4<unsigned char> > img = outputValue("image").valueAs<PiiMatrix<PiiColor4<unsigned char> > >();

  QVERIFY(img.rows() != 0);
  QVERIFY(img.columns() != 0);

  stop();
#else
  AVCODEC_SKIP;
#endif
}

void TestPiiVideoFileWriter::initTestCase()
{
#ifndef PII_NO_AVCODEC
  QVERIFY(createOperation("piivideo", "PiiVideoFileWriter"));
#else
  AVCODEC_SKIP;
#endif
}

void TestPiiVideoFileWriter::writeVideo()
{
#ifndef PII_NO_AVCODEC
  QVERIFY(start(ExpectFail));
    
  QString strVideoTmp =  "output/op_colorframe.avi";
  QString strVideo = "op_colorframe.avi";

  if(!QFile::exists(strVideo))
    QVERIFY(false);

  if (QFile::exists(strVideoTmp))
    QFile::remove(strVideoTmp);
   
  operation()->setProperty("fileName",strVideo);
  operation()->setProperty("outputDirectory", "output");
 
  connectAllInputs();

  QVERIFY(start());
    
  QString strImages = "data/images/colorframe0.jpeg";
  QVERIFY(QFile::exists(strImages));
  
  PiiColorQImage *pImg = PiiImageFileReader::readColorImage(strImages); 
  QVERIFY(pImg != 0);

  QVERIFY(sendObject("image", pImg->toMatrix()));
  QVERIFY(hasOutputValue("image"));
  QVERIFY(QFile::exists(strVideoTmp));
  
  stop();
#else
  AVCODEC_SKIP;
#endif
}

void TestPiiVideo::getFrame()
{
#ifndef PII_NO_AVCODEC
  QString strVideo = "data/ov14c1.avi";
  if(QFile::exists(strVideo))
    {
      
      PiiVideoReader reader(strVideo);
      
      try
        {
          reader.initialize();
        }
      catch(...)
        {
          QFAIL("Unexcepted exception");
        }
      
      PiiMatrix<PiiColor4<unsigned char> > frame;
      
      for(int j = 0; j < 1; ++j)
        {
          frame = reader.getFrame<PiiColor4<unsigned char> >();
          QVERIFY(!frame.isEmpty());
            
          PiiColor4<unsigned char> color;
            
          int red = 0, b = 0, g = 0, a = 0;
          for(int r = 0; r < frame.rows(); ++r)
            {
              for(int c = 0; c <frame.rows(); ++c)
                {
                  color = frame(r,c);
                  if(color.channels[0] != 0)
                    ++red;
                  if(color.channels[1] != 0)
                    ++g;
                  if(color.channels[2] != 0)
                    ++b;
                  if(color.channels[3] != 0)
                    ++a;
                }
            }
            
          QVERIFY(red!=0);
          QVERIFY(g!=0);
          QVERIFY(b!=0);
          QVERIFY(a!=0);
        }
    }
  else
    {
      QSKIP("Video file not found.", SkipAll);
    }
#else
  AVCODEC_SKIP;
#endif
    
}


void TestPiiVideo::saveNextColorFrame()
{
#ifndef PII_NO_AVCODEC
  if(QFile::exists("output/colorframe.avi"))
    QFile::remove("output/colorframe.avi");
  
  PiiVideoWriter writer("output/colorframe.avi",576,768);
  
  try
    {
      writer.initialize();
    }
  catch (PiiVideoException& ob)
    {
      qDebug()<<"Exception thrown: "<<ob.message();
      QVERIFY(false);
    }
  
  for(int i = 0; i < 2; ++i)
    {
      QString file = "data/images/colorframe";
      file = file + QString().setNum(i) + QString(".jpeg");

      QVERIFY(QFile::exists(file));

      // Read images.
      PiiColorQImage* pImg = PiiImageFileReader::readColorImage(file);
      QVERIFY(pImg != 0);

      //Write into video. 
      bool res =  writer.saveNextColorFrame(pImg->toMatrix());
      QVERIFY(res);
    }
#else
  AVCODEC_SKIP;
#endif
}

void TestPiiVideo::saveNextGrayFrame()
{
#ifndef PII_NO_AVCODEC
  if(QFile::exists("output/grayframe.avi"))
    QFile::remove("output/grayframe.avi");
  
  PiiVideoWriter writer("output/grayframe.avi",656,491);
  
  try
    {
      writer.initialize();
    }
  catch(PiiVideoException& ob)
    {
      qDebug()<<"Exception thrown: "<<ob.message();
      QVERIFY(false);
    }
  
  for(int i = 0; i < 2; ++i)
    {
      QString file = "data/images/videoframe";
      file = file + QString().setNum(i) + QString(".jpeg");

      QVERIFY(QFile::exists(file));
      // Read images.
        
      PiiGrayQImage* pImg = PiiImageFileReader::readGrayImage(file);
      QVERIFY(pImg != 0);

      //Write into video. 
      bool res = writer.saveNextGrayFrame(pImg->toMatrix());
      QVERIFY(res);
    }
#else
  AVCODEC_SKIP;
#endif
}

int main(int argc, char *argv[]) 
{
  //Contains all video-namespace functions. 
  TestPiiVideoFileReader videoFileReader;
  QTest::qExec(&videoFileReader,argc, argv);

  //Contains all video-reader functions.
  TestPiiVideoFileWriter videoFileWriter;
  QTest::qExec(&videoFileWriter,argc, argv);
  
  TestPiiVideo video;
  QTest::qExec(&video, argc, argv);
  
  return 0;
}



