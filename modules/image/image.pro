MODULE = Image
include(../module.pri)
enabled(fast): SOURCES += $$INTODIR/3rdparty/fast/*.cc
qt {
  SOURCES += rawimage/*.cc
  HEADERS += rawimage/*.h
  INCLUDEPATH += $$INTODIR/modules/camera/lib
  DEFINES += QT_STATICPLUGIN
}
