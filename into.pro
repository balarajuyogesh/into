include(qt5.pri)

TEMPLATE = subdirs

contains(CONFIG,qt) {
  SUBDIRS  = core \
             ydin \
             gui \
             modules \
             script \
             test \
             demos \
             3rdparty

  ydin.depends += core
  gui.depends += ydin
  modules.depends += ydin 3rdparty
  script.depends += ydin
  test.depends += modules
  demos.depends += modules

  qt5: {
    SUBDIRS += qml
    qml.depends += ydin
  }
} else {
  SUBDIRS = core modules
  modules.depends += core
}
