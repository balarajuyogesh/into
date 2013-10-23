# Generate a debug makefile with "qmake MODE=debug"

TEMPLATE        = lib
TARGET          = piicore
HEADERS         = *.h serialization/*.h matrix/*.h
SOURCES         = *.cc serialization/*.cc matrix/*.cc

!contains(DISABLE,network) {
  HEADERS += network/*.h
  SOURCES += network/*.cc
}

INTODIR         = ..
include($$INTODIR/base.pri)
include($$INTODIR/libinstall.pri)
VERSION         = $$INTO_LIB_VERSION

DEFINES += PII_BUILD_CORE
