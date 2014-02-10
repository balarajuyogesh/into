TEMPLATE = subdirs
SUBDIRS =  utils \
           base \
           calibration \
           camera \
           colors \
           dsp \
           imageprocessing \
           matching \
           tracking \
           video \
           intodemo
           
base.depends += utils
calibration.depends += utils
camera.depends += utils
colors.depends += utils
dsp.depends += utils
imageprocessing.depends += utils
matching.depends += utils
tracking.depends += utils
video.depends += utils
intodemo.depends += utils
