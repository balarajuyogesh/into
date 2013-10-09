TEMPLATE = lib
TARGET = piiqml
CONFIG += plugin
QT += qml quick
HEADERS = *.h
SOURCES = *.cc

INTODIR = ..
include($$INTODIR/base.pri)

DEFINES += PII_BUILD_QML

LIBS += -lQt5V8 -lpiicore$$LIBVER -lpiiydin$$LIBVER

# HACK: Qt does spectacular job in hiding important functionality.
# We need to hack around the limitations by accessing private API.
privateDirs = QtQml QtCore QtV8 QtQuick
for (dir, privateDirs) {
  INCLUDEPATH += $$(QTDIR)/include/$$dir/$$QT_VERSION/$$dir
}

defined(QML_INSTALL_PATH, var) {
  INSTALL_PATH = $$PLUGIN_INSTALL_PATH/Into
} else {
  !defined(INSTALL_PATH, var) {
    INSTALL_PATH = /usr/lib/into/qml/Into
  } else {
    INSTALL_PATH = $$INSTALL_PATH/qml/Into
  }
}

target.path = $$INSTALL_PATH
target.files += qmldir
INSTALLS += target
