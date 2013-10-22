Creating a Configuration
========================

Steps {#creating_steps}
-----

**1**. The necessary first step in creating a new application
configuration is the creation of an instance of PiiEngine. 
The configuration is created by adding operations into the engine. 

~~~
PiiEngine engine;
~~~

**2**. Depending on what operations are needed for the
configuration, the necessary plug-ins need to be loaded. Each
plug-in provides a set of operations that are available to the
engine upon successful loading of the plug-in. If the configuration
consists only of custom operations built into the application, this
step may be omitted.

~~~
engine.loadPlugin("piiimage");
~~~

**3**. Once the necessary plug-ins are loaded, operations are created
and added to the configuration. The operations may also need to be
configured. The configuration can be done manually or with a user
interface (see [creating_ui]). In manual configuration, the parameters
are set with the [PiiOperation::setProperty()] function.

~~~
PiiOperation* reader = engine.createOperation("PiiImageFileReader");
PiiOperation* writer = engine.createOperation("PiiImageFileWriter");
reader->setProperty("fileNamePattern", "images/*.jpg");
writer->setProperty("outputDirectory", "output");
writer->setProperty("extension", "jpg");
~~~

**4**. The configuration is finished by connecting operations to each
other. There are multiple ways of doing this. The simplest one is to
use the [connectOutput()](PiiOperation::connectOutput())
function. Remember that each input and each output has a unique name
in the scope of the enclosing operation. Thus, input and output
sockets are can be referred to in a generic manner by their names.

~~~
reader->connectOutput("image", writer, "image");
// alternative syntax:
(*reader | "image") >> ("image" | *writer)
~~~

**5**. The configuration is ready when all connections have been
created. The final step is to start the engine. Since the ideology of
the whole platform is in parallel execution, the execution of the
engine is also done in parallel. Therefore, one must make sure the
process that starts the engine does not exit before the engine has
done its job. A simple way of ensuring this is to use the
[PiiOperationCompound::wait()] function.

~~~
engine.execute();
engine.wait(PiiOperation::Stopped);
~~~

Put together, the little example above turns out to be a program
that reads all Jpeg files in a directory called "images" and writes
them into a directory called "output". This simple configuration is
shown in the figure below. Due to the default configuration of
PiiImageFileWriter, the output images are numbered sequentially
starting from zero. The name of the first output image will be
"img000000.jpg".

@image html creating_reader_writer.png

A Simple Example {#simple_example}
----------------

The following simple example will introduce an application that
reads images from files, thresholds them using an adaptive
thresholding technique, and writes the resulting binary images into
files. The program displays buttons for browsing the input images
and an image display that shows the result of thresholding.

The program consists of a single file called `Threshold.cc`. It reads
the image files given as command-line arguments, sends them to an
image display and a thresholding operation, and finally writes the
thresholded images to the current working directory. The configuration
is described graphically below. Note that the output images are stored
as gray levels: 0 is black and 1 is white.  In most image viewers, the
images will look pitch black.

@image html creating_simple_example.png

~~~
#include <PiiEngine.h>
#include <QApplication>
#include <QWidget>
#include <PiiTriggerButton.h>
#include <PiiImageDisplay.h>
#include <PiiProbeInput.h>

int main(int argc, char* argv[])
{
  // Initialize Qt's services
  QApplication app(argc, argv);

  // Create the engine and load plug-ins
  PiiEngine engine;
  try
    {
      engine.loadPlugin("piibase"); // PiiTriggerSource
      engine.loadPlugin("piiimage"); // PiiImageFileReader/Writer and PiiThresholdingOperation
    }
  catch (PiiLoadException& ex)
    {
      qDebug("Cannot load all necessary plug-in modules: %s", qPrintable(ex.message()));
      return 1;
    }

  // Create operations
  PiiOperation* trigger = engine.createOperation("PiiTriggerSource");
  PiiOperation* reader = engine.createOperation("PiiImageFileReader");
  PiiOperation* thresholder = engine.createOperation("PiiThresholdingOperation");
  PiiOperation* writer = engine.createOperation("PiiImageFileWriter");

  // Configure operations. See the API docs of the corresponding
  // operations for explanations.
  reader->setProperty("fileNamePattern", "../../../demos/images/*.jpg");
  reader->setProperty("imageType", "GrayScale");
  writer->setProperty("outputDirectory", ".");
  writer->setProperty("extension", "bmp");
  thresholder->setProperty("thresholdType", "SauvolaAdaptiveThreshold");

  // Connect operations
  trigger->connectOutput("trigger", reader, "trigger");
  reader->connectOutput("image", thresholder, "image" );
  thresholder->connectOutput("image", writer, "image");

  // Create user interface
  QWidget* buttons = new PiiTriggerButton;
  PiiImageDisplay* display = new PiiImageDisplay;
  display->setProperty("displayType", "AutoScale");

  // Create a probe that sends an image from the engine to the
  // display.
  PiiProbeInput* probe = new PiiProbeInput(reader->output("image"),
                                           display, SLOT(setImage(PiiVariant)));

  // Connect button UI to the trigger source.
  QObject::connect(buttons, SIGNAL(triggered(int)), trigger, SLOT(trigger(int)));

  buttons->show();
  display->show();

  // Start the engine (in background)
  try
    {
      engine.execute();
    }
  catch (PiiExecutionException& ex)
    {
      qDebug("Cannot start the engine: %s", qPrintable(ex.message()));
      return 1;
    }

  // Run Qt's main event loop that delivers events to UI components.
  // Engine continues to run in background.
  app.exec();

  // Qt is done. Send stop signal to the engine.
  engine.interrupt();
  // Wait until it really finishes.
  engine.wait(PiiOperation::Stopped);

  // Destroy UI
  delete buttons;
  delete display;
  delete probe;
  
  return 0;
}
~~~

Since Into is built on top of Qt, compiling an Into application is
most convenient with Qt's tools. One only needs to create a project
file that contains the instructions for a compiler. A program called
qmake converts the project file into compiler-dependent instructions
(a makefile). The project file is called `threshold.pro`, and its
contents are as follows:

~~~
INTODIR=/home/me/into
include($$INTODIR/base.pri)
qt5: QT += widgets
LIBS += -lpiicore$$LIBVER -lpiiydin$$LIBVER -lpiigui$$LIBVER
SOURCES = Threshold.cc
TARGET = threshold
CONFIG += console
~~~

Change `INTODIR` to point to your local installation directory. In the
command prompt, simply type qmake. This produces instructions
(Makefile) to compile the application with a make tool. Then, build
the application with make. The process is slightly different with
different compilers.

### Linux/OS X, GCC ### {#creating_building_gcc}

Open a terminal and type the following commands:

~~~
cd into_install_dir/doc/examples/threshold
qmake
make
debug/threshold
~~~

### Windows, MinGW ### {#creating_building_mingw}

Open the Qt command prompt and type the following commands:

~~~
cd into_install_dir\doc\examples\threshold
qmake
mingw32-make
release\threshold.exe
~~~

### Windows, MSVC ### {#creating_building_msvc}

Open the Qt command prompt and type the following commands:

~~~
cd into_install_dir\doc\examples\threshold
qmake
nmake
release\threshold.exe
~~~
