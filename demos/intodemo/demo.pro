INTODIR = ../../
include($$INTODIR/base.pri)

qt5: QT += widgets
FORMS += ui/*.ui
RESOURCES += resources.qrc

HEADERS   = *.h
SOURCES   = *.cc
LIBS += -lpiiydin$$LIBVER -lpiicore$$LIBVER
