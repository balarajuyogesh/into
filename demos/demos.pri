# Common build configuration for all test programs that use Ydin. The
# default build mode is "release". Generate a debug makefile with
# "qmake MODE=debug".
#
# All one needs to do in a demos-specific .pro file is this:
# include(../demos.pri)

isEmpty(INTODIR) { INTODIR = ../../ }
exists(../base.pri) {
  include(../base.pri)
  include(../plugindeps.pri)
} else {
  include($$INTODIR/base.pri)
  include($$INTODIR/plugindeps.pri)
}

INCLUDEPATH += $$INTODIR/demos/utils
HEADERS   = *.h
SOURCES   = *.cc

LIBS += -lpiiydin$$INTO_LIBV -lpiicore$$LIBVER -L$$INTODIR/demos/utils/$$MODE -ldemoutils

TARGET = $$PRO_FILE_BASENAME
defined(DEMO_INSTALL_PATH, var): {
  THIS_DEMO_INSTALL_PATH = $$DEMO_INSTALL_PATH/$$replace(_PRO_FILE_PWD_, $$PWD, "")
  target.path = $$THIS_DEMO_INSTALL_PATH
  resources.path = $$THIS_DEMO_INSTALL_PATH
  resources.files = image.png README
  INSTALLS += target resources
}

qt5: QT += widgets
