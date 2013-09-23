TEMPLATE        = lib
TARGET          = piiydin
HEADERS         = *.h
SOURCES         = *.cc

!contains(DISABLE,network) {
  HEADERS += network/*.h
  SOURCES += network/*.cc
}

INTODIR = ..
include($$INTODIR/base.pri)
include($$INTODIR/libinstall.pri)
VERSION         = $$INTO_LIB_VERSION

DEFINES += PII_BUILD_YDIN
LIBS += -lpiicore$$LIBVER
