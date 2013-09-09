include(qt5.pri)

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS  = core \
           ydin \
           plugins \
           gui \
           script \
           test \
           demos
