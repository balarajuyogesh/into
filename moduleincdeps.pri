# qmake instructions for creating compiler parameters for module
# dependencies. This is required for compile-time dependencies.
#
# Usage example:
# MODULEDEPS = Base Image Texture application/SmartClassifier
# include($$INTODIR/moduleincdeps.pri)

isEmpty(MODULESPATH) {
  MODULESPATH = modules
}

defineReplace(moduleIncDep) {
  modules = $$ARGS
  incpath =
  for(module, modules) {
    incpath += $$INTODIR/$$MODULESPATH/$$lower($$module)/plugin
    incpath += $$INTODIR/$$MODULESPATH/$$lower($$module)/lib
  }
  return($$incpath)
}

!isEmpty(MODULEDEPS) {
  INCLUDEPATH += $$moduleIncDep($$MODULEDEPS)
}
