TEMPLATE = lib
TARGET = piiqml
CONFIG += plugin
QT += qml quick
HEADERS = *.h
SOURCES = *.cc

INTODIR = ..
include($$INTODIR/base.pri)

DEFINES += PII_BUILD_QML

LIBS += -lQt5V8 -lpiicore$$INTO_LIBV -lpiiydin$$LIBVER

# HACK: Qt does spectacular job in hiding important functionality.
# We need to hack around the limitations by accessing private API.
privateDirs = QtQml QtCore QtV8 QtQuick
for (dir, privateDirs) {
  INCLUDEPATH += $$(QTDIR)/include/$$dir/$$QT_VERSION/$$dir
}

# qml install


defined(QML_INSTALL_PATH, var) {
  QML_INSTALL_PATH = $$QML_INSTALL_PATH/Into
} else {
  !defined(INSTALL_PATH, var) {
    QML_INSTALL_PATH = /usr/lib/into/qml/Into
  } else {
    QML_INSTALL_PATH = $$INSTALL_PATH/qml/Into
  }
}

qml.path = $$QML_INSTALL_PATH
qml.files += qmldir
INSTALLS += qml

target.path = $$QML_INSTALL_PATH
INSTALLS += target
