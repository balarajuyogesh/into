defined(MODULE_INSTALL_PATH, var) {
  INSTALL_PATH = $$MODULE_INSTALL_PATH
} else {
  !defined(INSTALL_PATH, var) {
    win32 {
      INSTALL_PATH = C:/Into/modules
    } else {
      INSTALL_PATH = /usr/lib/into/modules
    }
  } else {
    INSTALL_PATH = $$INSTALL_PATH/modules
  }
}
include(libinstall.pri)
