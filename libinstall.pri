# Set default install path
!defined(INSTALL_PATH, var) {
  win32 {
    INSTALL_PATH = C:/Into/bin
  } else {
    INSTALL_PATH = /usr/lib/into
  }
}
target.path = $$INSTALL_PATH
INSTALLS += target

# If headers need to be installed...
defined(HEADER_INSTALL_PATH, var) {
  headers.path = $$HEADER_INSTALL_PATH$$replace(_PRO_FILE_PWD_, $$PWD, "")
  defined(HEADERS_TO_INSTALL, var) {
    headers.files = $$HEADERS_TO_INSTALL
  } else {
    headers.files = $$HEADERS
  }
  INSTALLS += headers
}
