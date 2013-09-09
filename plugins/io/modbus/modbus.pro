INTODIR = ../../..
PLUGIN = io/modbus
include(../../piiplugin.pri)
TARGET = piimodbusiodriver
DEFINES += PII_BUILD_MODBUSIODRIVER

INCLUDEPATH += .. $$INTODIR/3rdparty/modbus/include
LIBS += -L$$INTODIR/3rdparty/modbus/lib -lmodbus
