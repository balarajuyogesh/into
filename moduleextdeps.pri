# This file resolves all 3rd party extensions modules required by
# plug-ins the current project depends on.
#
# Usage example:
# MODULEDEPS = Base Image Texture
# include($$INTODIR/moduleextdeps.pri)

isEmpty(MODULESPATH) {
  MODULESPATH = modules
}

defineReplace(moduleExtDep) {
  newExtensions =
  for(module, ARGS) {
    # If "module" is the name of a dependency file, use it.
    # Otherwise build a default file name using the module name and MODULESPATH.
    exists($$module) {
      depFile = $$module
    } else {
      depFile = $$MODULESPATH/$$lower($$module)/dependencies.pri
    }
    #message(Inspecting $$depFile)
    exists($$depFile) {
      exts = $$fromfile($$depFile, EXTENSIONS)
      !isEmpty(exts) {
        for(ext, exts) {
          moduleextpath = $$MODULESPATH/$$lower($$module)/$$ext
          exists($$moduleextpath): INCLUDEPATH += $$INTODIR/$$moduleextpath
          # Module-specific extension config
          exists($$moduleextpath/config.pri): include($$moduleextpath/config.pri)
          # If the extensions isn't already handled, add it to the list
          !isEmpty(ext):!contains(EXTENSIONS, $$ext) {
            #message(Adding ext to list)
            newExtensions += $$ext
          }
        }
      }
    }
  }
  export(INCLUDEPATH)
  return($$unique(newExtensions))
}

!isEmpty(MODULEDEPS): EXTENSIONS += $$moduleExtDep($$MODULEDEPS)

include(extensions.pri)
