DEFINES += PII_CXX11
*-g++ {
  system(g++ --version | grep -q " 4.[0-7]") {
    QMAKE_CXXFLAGS += -std=c++0x
  } else {
    QMAKE_CXXFLAGS += -std=c++11
  }
}
