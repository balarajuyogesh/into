# qmake treats relative paths differently depending on version and things like QMAKESPEC
levels = 0 1 2 3
current_path = ..
for(level, levels) {
  exists($$current_path/base.pri) {
    include($$current_path/base.pri)
    include($$current_path/plugindeps.pri)
    break()
  } else {
    current_path = $$current_path/..
  }
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

LIBS += -lpiiydin$$INTO_LIBV -lpiicore$$LIBVER
