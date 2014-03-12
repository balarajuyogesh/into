MODULE = Classification
include(../module.pri)
INCLUDEPATH += $$INTODIR/gui
LIBS += -L$$INTODIR/gui/$$MODE -lpiigui$$INTO_LIBV
