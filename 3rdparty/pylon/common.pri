isEmpty(PYLON_ROOT): PYLON_ROOT = $$(PYLON_ROOT)
INCLUDEPATH += $$INTODIR/modules/camera/genicam

win32 {
  LIBS += -L$$PYLON_ROOT/pylon/lib/Win32
  LIBS += -L$$PYLON_ROOT/genicam/library/cpp/lib/Win32_i86
  LIBS += -L$$PYLON_ROOT/genicam/bin/Win32_i86/GenApi/Generic
  LIBS += -lPylonBase_MD_VC80
}

unix {
  genicambin = $$PYLON_ROOT/genicam/bin
  exists($$PYLON_ROOT/lib64) {
    pylonlibs = lib64
    genicamlibs = Linux64_x64
  } else {
    pylonlibs = lib
    exists($$genicambin/Linux32_ARM) {
      genicamlibs = Linux32_ARM
    } else {
      genicamlibs = Linux32_i86
    }
  }
  LIBS += -L$$PYLON_ROOT/$$pylonlibs
  LIBS += -L$$genicambin/$$genicamlibs
  LIBS += -L$$genicambin/$$genicamlibs/GenApi/Generic
  LIBS += -lpylonbase
}

equals(MODE,release): DEFINES += NDEBUG
