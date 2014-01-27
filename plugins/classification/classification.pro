PLUGIN = Classification
include(../piiplugin.pri)
INCLUDEPATH += $$INTODIR/gui
LIBS += -L$$INTODIR/gui/$$MODE -lpiigui$$INTO_LIBV
