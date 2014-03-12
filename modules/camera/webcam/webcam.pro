INTODIR = ../../..
MODULE = camera/webcam
include(../../module.pri)
TARGET = piiwebcamdriver
DEFINES += PII_BUILD_WEBCAMDRIVER

# Webcam driver for Linux
unix {
    HEADERS       += linux/*.h
    SOURCES       += linux/*.cc
    INCLUDEPATH   += linux
}

# FireWire driver for Windows
win32 {
    HEADERS     += windows/*.h
    SOURCES     += windows/*.cc
    INCLUDEPATH += windows
}
