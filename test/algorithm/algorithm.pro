include(../unit_test.pri)

# Hack: delete the binary before compilation. Otherwise #include <algorithm> will include it!
unix: delete_bin.commands = rm -f $$MODE/algorithm
win32: delete_bin.commands = del $$MODE\\algorithm

QMAKE_EXTRA_TARGETS += delete_bin
PRE_TARGETDEPS += delete_bin
