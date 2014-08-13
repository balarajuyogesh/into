MODULE = Image
include(../module.pri)
enabled(fast) {
  SOURCES += $$INTODIR/3rdparty/fast/*.cc
  defined(HEADER_INSTALL_PATH, var) {
    headers.files += $$INTODIR/3rdparty/fast/*.h
  }
}
qt {
  SOURCES += rawimage/*.cc
  HEADERS += rawimage/*.h
  INCLUDEPATH += $$INTODIR/modules/camera/lib
  DEFINES += QT_STATICPLUGIN
} else {
  LIBS -= -lpiidsp
}
