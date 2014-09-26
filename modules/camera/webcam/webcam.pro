INTODIR = ../../..
MODULE = camera/webcam
include(../../module.pri)
TARGET = piiwebcamdriver
DEFINES += PII_BUILD_WEBCAMDRIVER
LIBS += -lpiiydin$$INTO_LIBV

linux {
  SUBFOLDER = linux
} else:win32 {
  SUBFOLDER = windows
} else {
  warning(No webcam support for your platform)
}

!isEmpty(SUBFOLDER) {
  HEADERS       += $$SUBFOLDER/*.h
  SOURCES       += $$SUBFOLDER/*.cc
  INCLUDEPATH   += $$SUBFOLDER
}
