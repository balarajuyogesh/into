Architecture
============

The figure below shows a conceptual overview of the Into platform.
The platform consists of an execution engine and a number of different
operations organized into plug-in modules. The dashed line surrounds a
set of image analysis operations that make up a plug-in. An
application, surrounded by the dotted circle, is built by a
configuration that makes use of operations in different plug-ins.

@image html pinta_ydin_devenv.png

Software Components {#software_components}
-------------------

@image html architecture_components.png

The figure above shows a diagram of the essential components of Into
and their interdependencies. The core of the platform, called Ydin,
consists of an execution engine (PiiEngine), interfaces to executable
operations (PiiOperation), interfaces and classes for handling
plug-ins (PiiPlugin), and a resource database (PiiResourceDatabase).

With no plug-ins loaded, Ydin is unable to perform any useful
tasks. In addition to the plug-ins, a configuration that describes the
operations, their configuration and connections, is needed. The
configuration can be created either programmatically or with a
graphical Development Environment.

Operations are software components that receive some input data and
produce some results after processing. An operation may also have no
inputs or no ouputs, in which case it is considered a producer or a
consumer, respectively. Each operation typically runs in a separate
execution thread, receiving objects and emitting new ones.

The operations can work either synchronously or asynchronously. For
example, an image source can emit images in its own internal
frequency, irrespective of any consumers of the image data
(asynchronous operation). The receivers process the data if they
aren't too busy doing other things. In most cases, however, it is
necessary to process all images captured. In this case the image
source halts until the next processing layer is done with processing
before it sends the next image.

Each operation can have any number (0-N) of input and outputs called
sockets (PiiSocket). Each socket has a data type or multiple data
types associated with it. That is, each socket either reads or emits a
limited set of object types types of object. Input and output sockets
with matching types can be connected to each other, and an output can
be connected to multiple inputs. Note, however, that the platform does
not prevent you from connecting non-matching sockets.

The figure below shows two operations connected to each other. The
first one, PiiLbpOperation reads in images and outputs features.  The
second one, [PiiSomOperation<float>](PiiSomOperation), reads in
features and outputs a code vector index (classification).  The
feature output of the LBP operation is connected to the feature input
of the SOM.

@image html architecture_lbp_som.png

A [plug-in](PiiPlugin) is a bundle of operations dynamically
loadable into PiiEngine. When loaded, a plug-in registers its
operations to the resource database so that each operation can be
dynamically created at run time. Plug-ins can also provide
extensions to the Development Environment and register custom data
types and classes.

Design Principles {#design_principles}
-----------------

The most important design guidelines in Into are:

- **Performance**. Application software created with Into is ready to
  be used in demanding applications such as industrial visual
  inspection. Processing data happens asynchronously, and in
  parallel. The platform can automatically utilize all the processors
  of a computer without additional programming effort. Even in a
  single-processor machine, the multi-threaded architecture allows the
  processor to utilize its full capacity. Asynchronous operation means
  that incoming data is processed as soon as it appears, minimizing
  unnecessary waiting.

- **Reuse**. A generic input/output interface between software
  components allows all components to interact with each other without
  translations. All new operations have all the old ones at their
  disposal. Standardized data formats ensure that all operations speak
  the same language.

- **Extensibility**. Operations are loaded into Into via a plug-in
  system that makes it possible to adjust the capabilities of Into and
  its memory requirements at run time. Custom operations and plug-ins
  can be created and used in a straightforward manner.

- **Easy programming**. Often, an intelligent analysis application can
  be created with just a few lines of code. One only needs to learn a
  couple of functions for loading plug-ins, creating operations, and
  for connecting operations to each other. It is also possible to
  implement a fully-featured user interface, for which the operations
  provide standard building blocks like configuration dialogs.

- **Dynamic configuration**. Creating an application with Into does
  not require programming. It is possible to create a configuration
  with a graphical tool that stores the operations, their properties,
  and connections into a file. A dynamically configurable pattern
  recognition application reads the file, creates the configuration at
  run-time and executes it. Not a single line of code is
  needed. Applications created in this way perform equivalently to
  their hard-coded counterparts; there is no performance penalty in
  executing dynamic configurations.

- **Portability**. The main hardware platform for Into is the PC.
  Prebuilt binaries are currently available for Linux, OS X and
  Microsoft Windows. Due to the requirements on portability,
  performance and easy programming (no kidding), C++ is the
  programming language of choice. No other programming language
  combines productivity and speed better. (Let the flame war begin.)
  Into is built on top of [Qt](http://www.qt-project.org).
