DEFINES += PII_CXX11
qt5 {
  CONFIG += c++11
} else {
  *-g++ {
    win32 {
      QMAKE_CXXFLAGS += -std=c++0x
    } else {
      system(g++ --version | grep -q " 4.[0-7]") {
        QMAKE_CXXFLAGS += -std=c++0x
      } else {
        QMAKE_CXXFLAGS += -std=c++11
      }
    }
  }
}
