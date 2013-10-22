INTODIR = ..
include($$INTODIR/extensions.pri)

TEMPLATE = subdirs

# Directories
SUBDIRS = base \
          camera \
          classification \
          colors \
          calibration \
          database \
          dsp \
          flowcontrol \
          geometry \
          image \
          io \
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

video.depends += image
colors.depends += image
geometry.depends += dsp
transforms.depends += image
image.depends += dsp
matching.depends += geometry classification optimization
camera.depends += image geometry
texture.depends += image
calibration.depends += image classification optimization

# Camera drivers
CAMERADRIVERS = emulator webcam
for(driver, CAMERADRIVERS) {
  !enabled($$driver):!equals(driver,emulator) {
    warning($$driver camera driver will not be built)
  } else {
    DRIVERID = camera_$$driver
    SUBDIRS += $$DRIVERID
    $${DRIVERID}.subdir = camera/$$driver
    $${DRIVERID}.depends = camera
  }
}

# Io drivers
IODRIVERS = emulator modbus
for(driver, IODRIVERS) {
  !enabled($$driver):!equals(driver,emulator) {
    warning($$driver I/O driver will not be built)
  } else {
    DRIVERID = io_$$driver
    SUBDIRS += $$DRIVERID
    $${DRIVERID}.subdir = io/$$driver
    $${DRIVERID}.depends = io
  }
}
