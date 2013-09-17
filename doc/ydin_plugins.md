Creating Plug-ins
=================

Plug-ins are dynamic parts of the Into platform that can be loaded and
unloaded on request. Placing operations into plug-ins has the
advantage that they don't need to be linked to the application
itself. The approach also forces one to use a generic interface for
all operations, which is good (TM) because it helps one in resisting
the temptation of including one gazillion interface definitions to the
main executable. It is also possible to unload the features that are
no longer needed by an application. Granted, the generic interface
makes it hard to work with complex data types, but it is still worth
the trouble.

All Ydin-compatible plug-ins must contain two query functions the
system can find with dlsym(): `pii_get_plugin_name`() and
`pii_get_plugin_version`(). Both must return a `const char*`.  In
addition, plug-in libraries may contain static data that needs to be
registered to the [PiiYdin::resourceDatabase()] "resource database".

[PiiPlugin.h] contains a bunch of useful macros for dealing with
plug-ins.

Implementing a plug-in is easy. The only thing one needs to do is to
1) select a unique name for the plug-in and 2) use the
[PII_IMPLEMENT_PLUGIN] macro in a .cc file:

~~~
PII_IMPLEMENT_PLUGIN(MyPlugin);
~~~


Exporting Classes {#creating_plugins_exporting}
-----------------

### Registering Operations ###

Once the plug-in has been implemented, it is time to register your
operations to it. Use the [PII_REGISTER_OPERATION] macro:

~~~
#include "MyOperation.h"

PII_IMPLEMENT_PLUGIN(MyPlugin);

PII_EXPORT_OPERATION(MyOperation);
~~~

You can repeat the macro as many times as needed. The macro registers
the named class to resource database. Now that your class is
registered to the plug-in, it can be created dynamically by its
name. Use the PiiYdin::createResource() function for this.

Note that the operations you register this way must be
*serializable*. (See [Serialization].)

### Registering Other Classes ###

Plugins are allowed to register not just types derived from
PiiOperation or QObject. For this, the [PII_REGISTER_CLASS] macro is
provided. Note that registering dynamically instantiable classes is
useful only if the type of the class is known to those creating the
instances. Usually, the superclass of a registered class needs to be
defined outside of the plug-in.

~~~
// Register a class derived from PiiCameraDriver
PII_EXPORT_CLASS(MyCameraDriver, PiiCameraDriver);
~~~


Building {#building_plugins}
--------

Once the plugin is ready, it must be compiled and linked. All the
necessary options are provided by piiplugin.pri, located in the
plugins directory. Minimally, all one needs in a project file is this
(example taken from the `piibase` plug-in):

~~~
PLUGIN = Base
include (../piiplugin.pri)
~~~

This assumes that the plug-in is placed under the plugins
directory. Then type `qmake` followed by `make`.

If a plug-in depends on other plug-ins, one needs to tell the compiler
and the linker where to find the necessary headers an libraries,
including those required by the other plug-in. To avoid this hassle,
the build system can resolve recursive dependencies for you. Assume
your plug-in depends on `piidsp` and `piicolors` plug-ins. Add the
dependencies to a file called `dependencies.pri` in the project
folder:

~~~
DEPENDENCIES = Dsp Colors
~~~
