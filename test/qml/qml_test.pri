include(../../base.pri)
TEMPLATE = app
CONFIG += qmltestcase console
QT += qmltest
SOURCES = main.cc
HEADERS =
#IMPORTPATH += $$INTODIR/../../qml


TARGET = $$PRO_FILE_BASENAME
defined(TEST_INSTALL_PATH, var): {
  target.path = $$TEST_INSTALL_PATH
  INSTALLS += target
}