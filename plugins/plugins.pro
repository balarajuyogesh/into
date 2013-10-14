INTODIR = ..
include($$INTODIR/extensions.pri)

TEMPLATE = subdirs

# Directories
SUBDIRS = base \
          camera \
          camera/emulator \
          classification \
          colors \
          calibration \
          database \
          dsp \
          features \
          flowcontrol \
          geometry \
          image \
          io \
          io/emulator \
          matching \
          network \
          optimization \
          statistics \
          texture \
          tracking \
          transforms \
          video

enabled(opencv) {
  SUBDIRS += opencv
  opencv.depends += image
  calibration.depends += opencv
}

image.depends += dsp
video.depends += image
colors.depends += image
geometry.depends += dsp
features.depends += image
matching.depends += geometry classification optimization
camera.depends += image geometry
texture.depends += image
calibration.depends += image classification optimization


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
