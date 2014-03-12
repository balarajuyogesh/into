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

    qmake -r
    make
    cd lib
    ./createlinks.sh release

You are now ready to run the demos:

    cd ~/into/demos/intodemo/release
    ./intodemo

If you want to install Into globally to your system, do this:

    export QTDIR=~/Qt/5.1.0/gcc_64
    qmake -r MODE=release HEADER_INSTALL_PATH=/usr/include/into
    make install

Into libraries will be installed under `/usr/lib` and headers under
`/usr/include/into`.


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

This assumes you have the `INTODIR` environment variable set. If you
installed Into globally, configure your project to use `/usr/lib` and
`/usr/include/into` as library and include paths, respectively.



Build configuration
-------------------

**Building without Qt**

Into's layered design makes it possible to use parts of the library
without Qt. This is useful if you want to use Into in an embedded
environment where Qt is not available. When built without Qt support,
Into uses an emulation layer that mimics Qt data types using the
corresponding types in the standard library.

Only the `core` and `modules` subdirectories can be built without Qt
support, and only a subset of features will be available. In plugin
directories, code under `lib` depends only on standard C++ features
whereas everything under `plugin` requires Qt.

Even when built without Qt support, you need to use qmake for
configuring the build:

    qmake -r CONFIG-=qt
    make

**Configuration options**

Pass configuration options to qmake with

    qmake -r "CONFIG += option1 option2"

- release: build in release mode (default)
- debug: build in debug mode
- c++03: use the c++03 standard (default is c++11)

**Variables**

Pass variables to qmake with

    qmake -r "VAR = value"

- `INSTALL_PATH`: where `make install` deploys libraries. Default is
  `/usr/lib`.
- `PLUGIN_INSTALL_PATH`: where `make install` deploys plug-in
  libraries. Default is `INSTALL_PATH/plugins`.
- `HEADER_INSTALL_PATH`: where `make install` copies header files.
  There is no default value. If left unspecified, headers will not be
  installed.

**Optional components**

The `DISABLE` variable to qmake can be used to switch off build for
components that would otherwise be built. This includes all 3rd party
components listed under the 3rdparty folder and `network`, which turns
off Into's network support. For example, if you don't want to use the
`fast` and `opencv` extensions, do this:

    qmake -r "DISABLE = fast opencv"
