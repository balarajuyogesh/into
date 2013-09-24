!defined(INSTALL_PATH, var): INSTALL_PATH = /usr/lib
target.path = $$INSTALL_PATH
INSTALLS += target

defined(HEADER_INSTALL_PATH, var): {
  headers.path = $$HEADER_INSTALL_PATH$$replace(_PRO_FILE_PWD_, $$PWD, "")
  headers.files = $$HEADERS
  INSTALLS += headers
}
