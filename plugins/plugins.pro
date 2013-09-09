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
#          ocr

# Include opencv?
enabled(opencv):SUBDIRS += opencv

SUBDIRS += calibration \
           texture \
           video \
           camera \
           colors \
           features \
           tracking \
           transforms \
           statistics \
           io \
           camera/emulator \
           io/emulator

# Camera drivers
CAMERADRIVERS = multicam iport currera pylon simplon pleora webcam
for(driver, CAMERADRIVERS) {
  enabled($$driver): SUBDIRS += camera/$$driver
}

# Io drivers
IODRIVERS = multicam advantech currera modbus
for(driver, IODRIVERS) {
  enabled($$driver): SUBDIRS += io/$$driver
}

SUBDIRS += gui
