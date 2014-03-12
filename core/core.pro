# Generate a debug makefile with "qmake MODE=debug"

TEMPLATE = lib
TARGET = piicore

qt {
  HEADERS = *.h serialization/*.h matrix/*.h
  SOURCES = *.cc serialization/*.cc matrix/*.cc

  !contains(DISABLE,network) {
    HEADERS += network/*.h
    SOURCES += network/*.cc
  }
} else {
  SOURCES += PiiBits.cc PiiColorTable.cc PiiConstCharWrapper.cc PiiException.cc PiiGlobal.cc \
    PiiInvalidArgumentException.cc PiiIOException.cc PiiMath.cc PiiMathException.cc \
    PiiPtrHolder.cc PiiRandom.cc PiiResourceStatement.cc PiiResourceDatabase.cc \
    PiiSharedObject.cc PiiSharedPtr.cc PiiSimpleMemoryManager.cc PiiTimer.cc PiiVariant.cc \
    PiiVersionNumber.cc
  SOURCES += stdwrapper/*.cc matrix/*.cc
  INCLUDEPATH += stdwapper
  LIBS += -lrt
}

INTODIR = ..
include($$INTODIR/base.pri)
include($$INTODIR/libinstall.pri)
VERSION = $$INTO_LIB_VERSION

DEFINES += PII_BUILD_CORE
