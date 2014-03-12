MODULE = Optimization
include(../module.pri)
enabled(lbfgs): SOURCES += $$INTODIR/3rdparty/lbfgs/*.cpp
enabled(lmfit): SOURCES += $$INTODIR/3rdparty/lmfit/*.c
