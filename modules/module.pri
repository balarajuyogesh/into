# Common build configuration for all modules
#
# All one needs to do in a plugin-specific .pro file is this:
# MODULE = Texture
# include(../module.pri)
#
# Use class naming conventions for the module name, without the "Pii"
# prefix.

isEmpty(MODULE) {
  error(You must define MODULE before including piiplugin.pri)
}

isEmpty(INTODIR) {
  INTODIR = $$clean_path($$PWD/..)
}

include(../base.pri)

LOCASE_NAME = $$lower($$basename(MODULE))
TARGET = pii$$LOCASE_NAME

exists($$OUT_PWD/lib) {
  HEADERS += lib/*.h
  exists($$OUT_PWD/lib/*.cc): SOURCES += lib/*.cc
  INCLUDEPATH += $$INTODIR/modules/$$LOCASE_NAME/lib
}

# Qt builds include operations and the plugin
qt:exists($$OUT_PWD/plugin) {
  HEADERS += plugin/*.h
  SOURCES += plugin/*.cc
  LIBS += -lpiiydin$$INTO_LIBV
}

TEMPLATE        = lib
unix:VERSION    = $$INTO_LIB_VERSION
DEFINES         += PII_BUILD_$$upper($$basename(MODULE))
DEFINES         += PII_LOG_MODULE=$$basename(MODULE)

win32-msvc*: QMAKE_LFLAGS_WINDOWS_DLL += /OPT:NOREF

include(../moduledeps.pri)

defined(PLUGIN_INSTALL_PATH, var) {
  INSTALL_PATH = $$PLUGIN_INSTALL_PATH
} else {
  !defined(INSTALL_PATH, var) {
    INSTALL_PATH = /usr/lib/into/plugins
  } else {
    INSTALL_PATH = $$INSTALL_PATH/plugins
  }
}
include(../libinstall.pri)

LIBS += -lpiicore$$INTO_LIBV
