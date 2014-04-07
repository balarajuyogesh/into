TEMPLATE = lib
TARGET = piiscript
CONFIG += plugin
QT += script
HEADERS = *.h
SOURCES = *.cc

INTODIR = ..
include($$INTODIR/base.pri)
!defined(INSTALL_PATH, var) {
  INSTALL_PATH = /usr/lib/into/script
} else {
  INSTALL_PATH = $$INSTALL_PATH/script
}
include($$INTODIR/libinstall.pri)
DEFINES += PII_BUILD_SCRIPT
LIBS += -lpiicore$$INTO_LIBV -lpiiydin$$INTO_LIBV
