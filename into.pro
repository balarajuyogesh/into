include(qt5.pri)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS  = core \
           ydin \
           gui \
           plugins \
           script \
           test \
           demos

qt5: SUBDIRS += qml