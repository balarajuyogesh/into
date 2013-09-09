PLUGIN = Image
include(../piiplugin.pri)
enabled(fast): SOURCES += $$INTODIR/3rdparty/fast/*.cc
SOURCES += rawimage/*.cc
HEADERS += rawimage/*.h
INCLUDEPATH += ../camera
DEFINES += QT_STATICPLUGIN
