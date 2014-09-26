include(common.pri)

INCLUDEPATH += $$INTODIR/3rdparty/pylon/wrapper
LIBS += -L$$INTODIR/3rdparty/pylon/wrapper/$$MODE -lpylonwrapper

win32 {
  win32-msvc*:!isEmpty(PYLON_ROOT):exists($$PYLON_ROOT):EXT_ENABLED=true
} else {
  !isEmpty(PYLON_ROOT):exists($$PYLON_ROOT):EXT_ENABLED=true
}
