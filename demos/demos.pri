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

LIBS += -lpiiydin$$LIBVER -lpiicore$$LIBVER -L$$INTODIR/demos/utils/$$MODE -ldemoutils

qt5: QT += widgets

#win32:CONFIG -= embed_manifest_exe
