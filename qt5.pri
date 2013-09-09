# QT_MAJOR_VERSION is "= 4 && QT_MINOR_VERSION > 5" according to qmake (at least up to Qt 5.0.1)
QT_MAJOR_VERSION = $$QT_VERSION
QT_MAJOR_VERSION ~= s/\\.[0-9]+\\.[0-9]+$//
equals(QT_MAJOR_VERSION,5): CONFIG += qt5
