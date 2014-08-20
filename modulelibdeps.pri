# qmake instructions for creating linker parameters for plug-in
# dependencies.
#
# Usage example:
# MODULEDEPS = Base Image Texture
# include($$INTODIR/modulelibdeps.pri)

isEmpty(MODULESPATH) {
  MODULESPATH = modules
}

# Creates library dependencies
defineReplace(moduleLibDep) {
  modules = $$ARGS
  libs =
  for(module, modules) {
    dirname = $$lower($$module)
    libs += -L$$INTODIR/$$MODULESPATH/$$dirname/$$MODE
    libs += $$forcedLink(pii$$lower($$basename(module)))
    #!unix:libs += -lpii$$lower($$basename(module))
  }
  return($$libs)
}

# Creates linker command line parameters that force MSVC to link the
# plug-ins. We need to use the mangled C++ names here.
defineReplace(msvcForceDeps) {
  modules = $$ARGS
  incs =
  for(module, modules) {
    p = Pii$$basename(module)Plugin
    incs += /INCLUDE:??0$$p@@QAE@XZ
  }
  return($$incs)
}

!isEmpty(MODULEDEPS) {
  LIBS += $$moduleLibDep($$MODULEDEPS)
  #win32-msvc*:QMAKE_LFLAGS_RELEASE += $$msvcForceDeps($$MODULEDEPS)
  #win32-msvc*:QMAKE_LFLAGS_DEBUG += $$msvcForceDeps($$MODULEDEPS)
}
