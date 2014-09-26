TEMPLATE = subdirs
INTODIR = ..
include(../extensions.pri)
enabled(pylon): SUBDIRS += pylon/wrapper
