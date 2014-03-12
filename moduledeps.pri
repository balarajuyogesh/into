# Recursively finds dependencies between plug-ins. This file is
# automatically included from module.pri, but can also be used
# directly in projects that depend on plug-ins.

!isEmpty(APPLICATIONPATH) {
  MODULE = $$APPLICATIONPATH/dependencies.pri
} else:isEmpty(MODULE) {
  MODULE = $$OUT_PWD/dependencies.pri
}

isEmpty(MODULESPATH) {
  MODULESPATH = modules
}

# qmake programmers apparently don't know what a stack is for.
# Recursive calls crash the program. This function works iteratively,
# one dependency level at a time. (breadth-first search)

defineReplace(resolveDependencies) {
  newDeps =
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
      deps = $$fromfile($$depFile, DEPENDENCIES)
      !isEmpty(deps) {
        for(dep, deps) {
          #message($$module depends on $$dep)
          # See if the dependency is conditional
          parts = $$split(dep,":")
          # There are conditions
          !count(parts,1) {
            extensions = $$member(parts, 0, -2)
            dep = $$last(parts)
            for(ext, extensions) {
              !enabled($$ext) {
                dep =
                break()
              }
            }
          }
          # If the dependency isn't already handled, add it to the list
          !isEmpty(dep):!contains(MODULEDEPS, $$dep) {
            #message(Adding $$dep to list)
            newDeps += $$dep
          }
        }
      }
    }
  }
  return($$unique(newDeps))
}

MODULEDEPS = $$resolveDependencies($$MODULE)
!isEmpty(MODULEDEPS) {
  newDeps = $$MODULEDEPS
  # There's no while loop in qmake. Hope 10 levels is enough.
  levels = 0 1 2 3 4 5 6 7 8 9
  for(level, levels) {
    newDeps = $$resolveDependencies($$newDeps)
    isEmpty(newDeps) {
      break()
    }
    MODULEDEPS += $$newDeps
  }
}
#message(Final deps: $$MODULEDEPS)
# Remove myself from dependencies (dependency loop)
MODULEDEPS -= $$MODULE
include($$PWD/moduleincdeps.pri)
include($$PWD/modulelibdeps.pri)
MODULEDEPS += $$MODULE
include($$PWD/moduleextdeps.pri)
