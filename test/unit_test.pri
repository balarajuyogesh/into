# qmake treats relative paths differently depending on version and things like QMAKESPEC
exists(../base.pri) {
  include(../base.pri)
  include(../plugindeps.pri)
} else {
  include(../../base.pri)
  include(../../plugindeps.pri)
}

qt5 {
  QT += testlib
} else {
  CONFIG += qtestlib
}
SOURCES += *.cc
HEADERS += *.h

TARGET = $$PRO_FILE_BASENAME
defined(TEST_INSTALL_PATH, var): {
  target.path = $$TEST_INSTALL_PATH
  INSTALLS += target
}

LIBS += -lpiiydin$$LIBVER -lpiicore$$LIBVER
