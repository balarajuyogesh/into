# Generate a debug makefile with "qmake MODE=debug"

TEMPLATE        = lib
TARGET          = piicore
HEADERS         = *.h
SOURCES         = *.cc serialization/*.cc matrix/*.cc
INCLUDEPATH     += $$(QTDIR)/include/Qt

!contains(DISABLE,network) {
  HEADERS += network/*.h
  SOURCES += network/*.cc
}

INTODIR          = ..
include($$INTODIR/base.pri)
VERSION         = $$INTO_LIB_VERSION

DEFINES += PII_BUILD_CORE
