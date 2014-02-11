TEMPLATE        = lib
TARGET          = demoutils
HEADERS         = *.h
SOURCES         = *.cc

RESOURCES += resources.qrc

INTODIR         = ../..
include($$INTODIR/base.pri)

DEFINES += PII_BUILD_DEMOUTILS
LIBS += -lpiiydin$$INTO_LIBV -lpiicore$$INTO_LIBV

qt5: QT += widgets
