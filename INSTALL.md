Installation
============

Prerequisites
-------------

You need to have Qt installed. If you don't already have a copy, get
one at http://qt-project.org/. Into works with Qt 4 and Qt 5.


Installation on Linux
---------------------

Ensure that Qt environment is set up correctly. At the minimum, Qt
libraries must be found in system library path or in directory that is
in the `LD_LIBRARY_PATH` environment variable.

Type the following at the command prompt:

    export QTDIR=~/Qt/5.1.0/gcc_64
    export INTODIR=~/into
    export LD_LIBRARY_PATH=$QTDIR/lib$INTODIR/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
    export PATH=$QTDIR/bin:$PATH

Change the paths to match your installation of Qt and Into.

To build and deploy locally, type

    qmake -r MODE=release
    make
    cd lib
    ./createlinks.sh release

If you want to build in debug mode, change "release" to "debug".

You are now ready to run the demos:

    cd ~/into/demos/intodemo/release
    ./intodemo


Installation on Windows
-----------------------

Ensure that Qt environment is set up correctly. On Windows, Qt and
Into dlls must be found in `PATH`. The Qt command prompt modifies
`PATH` to point to Qt libs, but you need to add Into libraries
yourself. Open the Qt command prompt and type the following to build
and deploy locally:

    set INTODIR=C:\Into
    set PATH=%INTODIR%\lib;%PATH%
    cd %INTODIR%
    qmake -r MODE=release
    mingw32-make
    cd lib
    copydlls

Make the `INTODIR` variable to point to your copy of Into.

To run the demos:

    cd \Into\demos\intodemo\release
    intodemo


Building software that uses Into
--------------------------------

You can build your own software against Into by including
`into/base.pri` in your project file:

    INTODIR=$$(INTODIR)
    include($$INTODIR/base.pri)
