INTODIR = ..
include($$INTODIR/extensions.pri)

TEMPLATE = subdirs
CONFIG += ordered

# Directories
SUBDIRS = base \
          database \
          dsp \
          flowcontrol \
          geometry \
          image \
          classification \
          optimization \
          matching \
          network

# Include opencv?
enabled(opencv):SUBDIRS += opencv

SUBDIRS += calibration \
           texture \
           video \
           camera \
           colors \
           tracking \
           transforms \
           statistics \
           io \
           camera/emulator \
           io/emulator

# Camera drivers
CAMERADRIVERS = webcam
for(driver, CAMERADRIVERS) {
  enabled($$driver): SUBDIRS += camera/$$driver
}

# Io drivers
IODRIVERS = modbus
for(driver, IODRIVERS) {
  enabled($$driver): SUBDIRS += io/$$driver
}
