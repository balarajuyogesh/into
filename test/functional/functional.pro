include(../unit_test.pri)

# Hack: delete the binary before compilation. Otherwise #include <functional> will include it!
unix: delete_bin.commands = rm -f $$MODE/functional
win32: delete_bin.commands = del $$MODE\\functional

QMAKE_EXTRA_TARGETS += delete_bin
PRE_TARGETDEPS += delete_bin