include(qt5.pri)

TEMPLATE = subdirs

SUBDIRS  = core \
           ydin \
           gui \
           plugins \
           script \
           test \
           demos

ydin.depends += core
gui.depends += ydin
plugins.depends += ydin
script.depends += ydin
test.depends += ydin
demos.depends += plugins

qt5: {
  SUBDIRS += qml
  qml.depends += ydin
}
