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

LIBS += -lpiiydin$$LIBVER -lpiicore$$LIBVER
