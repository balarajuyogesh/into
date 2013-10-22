Custom Operations
=================

Custom operations are needed when the set of prebuilt operations is
not enough for solving an inspection problem. The need for custom
operations also arises when one needs to connect an inspection
application to process control or to custom hardware.

Operation Interfaces {#operation_interfaces}
--------------------

When PiiEngine executes a configuration, it only needs a way of
starting, stopping and pausing operations. To create a configuration,
one also needs a way of retrieving the inputs and outputs of an
operation. Finally, a way of creating operation-specific user
interfaces is needed. These are the functions all Ydin-compatible
operations must implement. The functions are defined in the
PiiOperation class, which must be inherited when creating a custom
operation.

Although it is possible to create a fully custom operation from
scratch, it is almost always better to use a partial implementation of
the PiiOperation interface called PiiDefaultOperation. This class
takes care of synchronization, and is derived from PiiBasicOperation
that handles the adding and removing of sockets.  This section
concentrates on implementing operations based on
PiiDefaultOperation. PiiOperation and PiiBasicOperation can be used in
some special situations, but they require more work for to properly
cope with synchronization, for example.

PiiDefaultOperation uses a [flow
controller](PiiDefaultOperation::FlowController) to control the flow
of objects through the operation. Whenever the controller decides that
it is time to process, a
[processor](PiiDefaultOperation::OperationProcessor) invokes a
protected [process](PiiDefaultOperation::process()) function. This
function is overridden by subclasses to perform the actual processing.

PiiDefaultOperation provides three threading models: *non-threaded*,
*single-threaded* and *multi-threaded*. The processing mode can be
changed with the [PiiDefaultOperation::threadCount] property.

In non-threaded model, `process()` will be called by the thread that
sends the last object required for a processing round. There is no
separate execution thread. It logically follows that non-threaded
model cannot be used with operations that just produce data without an
external trigger. Such operations must be processed in
parallel. Non-threaded model is the right choice when the overhead in
parallel processing exceeds the complexity of the operation. Such
operations are, for example, simple logical and arithmetic
calculations.

In single-threaded model, a separate processing thread is started for
the operation. The thread sleeps when no data is available in inputs
and is awakened by each incoming object. Single-threaded operations
are used when the the actual processing takes a relatively long time.

In multi-threaded model, any number of threads can be started. They
are run in parallel, and the system ensures that the ordering of
output objects matches that of the input. If the `process()` function
modifies shared data such as member variables, the programmer must
ensure that mutual exclusion is handled properly.

A typical declaration for an operation is as follows:

~~~
class MyOperation : public PiiDefaultOperation
{
public:
  MyOperation();
protected:
  void process();
};
~~~

The constructor should add the necessary input and output sockets to
the operation. The `process`() function reads all objects from input
sockets and processes them. It should then send the results of
processing to output sockets, or to any other interface such as a user
interface, network socket or database.

Input and Output Sockets {#input_and_output_sockets}
------------------------

Connections between operations are handled with objects called
sockets. PiiSocket is a common super-interface for both input and
output sockets, and it is derived by PiiInputSocket and
PiiOutputSocket. All sockets have a unique name in the scope of the
enclosing operation, which makes it possible to refer to them by
name. They also have a number of dynamic properties that are used by
the Development Environment.

Let us assume MyOperation has one input and one output. The task of
the operation is simply to pass an input object to the output. The
sockets are created as shown below:

~~~
MyOperation::MyOperation()
{
  // Create an input socket
  addSocket(new PiiInputSocket("input"));
  // Create an output socket
  addSocket(new PiiOutputSocket("output"));
}
~~~

Since we actually need no processing, the `process`() function is a
one-liner:

~~~
void MyOperation::process()
{
  emitObject(readInput());
}
~~~

The function reads an incoming object ([PiiInputSocket::objectAt()])
and emits it through the output socket
([PiiOutputSocket::emitObject()]). This is all one needs to do for a
functional operation.

Data Types {#data_types}
----------

### Variants ### {#data_types_variants}

The data types passed between operations are encapsulated in a class
called PiiVariant, a generic wrapper for any type of data. There is a
large number of prebuilt data types (see the API documents for the
PiiYdin namespace and PiiVariant class), and nothing prevents one from
creating new ones as far as unique ID numbers are selected.

Instead of copying an object each time it is passed from an operation
to another, Into uses reference counted types and the copy-on-write
paradigm to implement a primitive but efficient garbage collector.

Let us assume for a while that our illustrative `MyOperation` not only
passes the object but also increments it if it happens to be an
integer. The process function must now inspect the type of the
incoming object:

~~~
void MyOperation::process()
{
  PiiVariant obj = readInput();
  if (obj.type() == PiiVariant::IntType)
    emitObject(obj.valueAs<int>() + 1);
  else
    emitObject(obj);
}
~~~

The [PiiVariant::type()] function returns the type ID of a variant
object. The `if` clause in the example tests if the type ID equals the
type ID of an integer. If that is the case we know the value of the
variant can be safely retrieved as an `int`.

The [PiiVariant::valueAs()] function template returns a reference to
the held object as the template type, in this case
`int`. [PiiOutputSocket::emitObject()] is also a template that
supports any data type. The function template creates a new PiiVariant
out of the value passed as the only argument. It gets the type ID for
the new variant by another function template, [Pii::typeId()]. It has
specializations for all the types defined in PiiVariant.h and
PiiYdinTypes.h. (Remember to include PiiYdinTypes.h if you use types
other than the primitive ones.)

Variants can also be created excplicitly. This way it is possible
to give a custom type ID to an object.

~~~
PiiVariant newObj(obj.valueAs<int>()+1, PiiVariant::IntType);
emitObject(newObj);
~~~

This technique is useful if you want to mark a derived object with the
type ID of a parent that is more widely supported. Another case is if
you want to use an existing data type in a special role that needs to
be handled separately.

### Common Data Types ### {#common_data_types}

In Into, the most common form of data that is passed between
operations are numbers and matrices. Matrices are also used to
represent images, which might first feel a bit awkward. (Matlab users
should feel home.) All primitive data types are reflected by
corresponding type IDs in the PiiVariant class itself. Matrices,
complex numbers and colors are defined in the PiiBaseType namespace.

The Into system uses a class template called PiiMatrix in representing
all matrix and image types. For example.  PiiMatrix<unsigned char>
represents 8-bit gray-scale images. A color is also denoted by a class
template, called PiiColor or PiiColor4, depending on whether the color
has three or four channels. An RGB image with eight bits per color
channel is thus represented by PiiMatrix<PiiColor<unsigned char> >.

Four channel colors are used to optimize speed. Since four bytes
equals 32 bits, addressing such a color always happens at a
word-aligned memory address. The drawback is that more memory is
needed. Four channel colors are, however, in intense use because image
I/O is performed with 32-bit colors.

All image operations in Into are assumed to support at least the
following image types:

Image type           |Class
---------------------|------------------------------------
8-bit gray           | PiiMatrix<unsigned char> 
16-bit gray          | PiiMatrix<unsigned short> 
32-bit gray          | PiiMatrix<unsigned int> 
floating point gray  | PiiMatrix<float>
24-bit color         | PiiMatrix<PiiColor<unsigned char>>
32-bit color         | PiiMatrix<PiiColor4<unsigned char>>
48-bit color         | PiiMatrix<PiiColor<unsigned short>>
floating point color | PiiMatrix<PiiColor<float>>

All matrix types have a corresponding type ID in the PiiYdin
namespace. For example, PiiMatrix<unsigned char> is reflected by
PiiYdin::UnsignedCharMatrixType.

Matrices are used not only for images but also for areas, points,
feature vectors etc. Please see the documentation for PiiYdin.h and
PiiPoint for more information.

### Supporting Multiple Types ### {#supporting_multipe_types}

Let us now assume that MyOperation should add one to all pixels of
each image it reads in. Since it is assumed to support all image
types, there should be a routine to perform the addition for all the
image types listed above. Doing this manually would be error-prone,
laborious and stupid:

~~~
void MyOperation::process()
{
  PiiVariant obj = readInput();
  using namespace PiiYdin;
  switch (obj.type())
  {
  case UnsignedCharMatrixType:
    addOneTo8BitGray(obj.valueAs<PiiMatrix<unsigned char> >);
    break;
  case UnsignedCharColor4Type:
    addOneTo32BitColor(obj.valueAs<PiiMatrix<PiiColor4<unsigned char> > >());
    break;
  // etc...
  }
}
~~~

The suggested way is to define a function template that handles this
all. We'll first need to add this function to the class declaration:

~~~
class MyOperation : public PiiDefaultOperation
{
...
private:
  template <class T> addOne(const PiiVariant& obj);
};
~~~

The file PiiBaseType.h contains useful macros for building the
necessary switch clauses. Since we are supporting all image types, we
need a macro called [PII_ALL_IMAGE_CASES]:

~~~
void MyOperation::process()
{
  PiiVariant obj = readInput();
  switch (obj->type())
    {
      PII_ALL_IMAGE_CASES(addOne, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}
~~~

The result is that the function template function `addOne`() is
instantiated with a different template parameter depending on the type
ID of the incoming object. If the type ID does not match any of our
supported types, the [PII_THROW_UNKNOWN_TYPE] macro throws an
exception that PiiEngine handles as an error.

In the implementation of the function template something like the
following needs to be done:

~~~
template <class T> MyOperation::addOne(const PiiVariant& obj)
{
  const PiiMatrix<T> &image = obj.valueAs<PiiMatrix<T> >();
  emitObject(image + T(1));
}
~~~

The first line takes the value of the passed object as a matrix of any
type. The correct template type `T` is ensured by the
[PII_ALL_IMAGE_CASES] macro. The second line adds one to the matrix
and emits the result through the only output socket. Using `T`(1)
ensures that the operation also works with colors.

Since matrices are the most common form of data passed between
operations, their use should be as easy as possible. For this
reason, PiiBaseType.h provides a lot more similar convenience
macros. The variations are explained in the API documentation.

Let us still work through another example. This version does the same
as the previous one, but converts color images to gray scale.  For
this, we need a new conversion function:

~~~
class MyOperation : public PiiDefaultOperation
{
...
private:
  template <class T> convertAndAdd(const PiiVariant& obj);
};
~~~

The process function is modified like this:

~~~
void MyOperation::process()
{
  PiiVariant obj = readObject();
  switch (obj.type())
    {
      // Gray images work with addOne
      PII_GRAY_IMAGE_CASES(addOne, obj);
      // Color images are converted with convertAndAdd
      PII_COLOR_IMAGE_CASES(convertAndAdd, obj);
    default:
      PII_THROW_UNKNOWN_TYPE(inputAt(0));
    }
}
~~~

Within the switch block, color and gray-scale images are separated
into two different processing functions with two marcos. The old one
(`addOne`()) is now used for gray-scale images only. The color version
looks like this:

~~~
template <class T> MyOperation::convertAndAdd(const PiiVariant& obj)
{
  // T is a PiiColor or PiiColor4 now.
  // A handy typedef for the color's actual element type.
  typedef typename T::Type Element;
  PiiMatrix<T> &image = obj.valueAs<PiiMatrix<T> >();
  emitObject(static_cast<PiiMatrix<Element> >(image) + 1);
}
~~~

The first line of the function defines a `typedef` for the primitive
type that forms the color's channels. Converting the color image to
gray scale is straightforward: just perform a type cast to the type of
the color channels. The third line does just that, adds one to the
result, and emits it through the output socket.

Configuring Synchronization {#configuring_synchronization}
---------------------------

Synchronizing ansychronous processing units is a subtle issue and
needs to be carefully considered when implementing custom
operations. Fortunately, most operations work in one-to-one
input-output correspondence: every incoming object is processed and
one result object is sent. For other types of processing, the
programmer must be aware of synchronization.

An operation that produces more than one result per input object must
inform the corresponding output socket that it is going to break the
1:1 rule. A good example is PiiImageSplitter that emits 1-N subimages
for each incoming image.

In the following example, the operation produces two outputs for each
input.

~~~
void MyOperation::process()
{
  PiiVariant obj = readInput();
  outputAt(0)->startMany();
  outputAt(0)->emitObject(obj);
  outputAt(0)->emitObject(obj);
  outputAt(0)->endMany();
}
~~~

Before the program starts sending the objects, it tells the output
socket that multiple objects will follow instead of the expected
single one. This is accomplished with
[PiiOutputSocket::startMany()]. When the operation is done sending, it
informs the socket with [PiiOutputSocket::endMany()]. The same process
applies to situations where the operation omits sending an object. In
such a case it only signals `startMany` and `endMany` without any
`emitObject` calls in between. Thus, the meaning of "many" is actually
"other than one".

The internal effect of this in Ydin is that the *flow level* of the
output socket is first raised and then lowered. Ydin tracks the flow
level of synchronized sockets through the whole pipeline of
operations, which is why one needs to indicate what sockets are
synchronized to each other. Input sockets within an operation are
synchronized when their flow levels are equal.

Omitting the `startMany` and `endMany` signals causes no problems if
outputs with different object rates are not connected to the inputs of
a single operation. However, breaking this or any other
synchronization rule causes hard-to-debug problems in complex
configurations.

### Synchronization Groups ### {#sync_groups}

Input and output sockets are arranged into synchronization groups.
Sockets with the same group id always work in sync with each other:
there must be an object in all grouped input sockets before processing
can be performed. And the results must be emitted to all output
sockets with the same group id.

By default, all sockets belong to group 0. This means that all inputs
must be filled before processing will happen and that each output will
always emit an object whenever the processing is performed.

PiiDefaultOperation uses a PiiDefaultFlowController to handle
synchronization in operations with more than one input group.
PiiDefaultFlowController can be configured to handle parent-child
relationships between input groups. In PiiImagePieceJoiner, for
example, the `image` input has group id 0 (parent), and it expects to
receive a large image. The `location` and `label` inputs have group id
1 (child), and they expect to receive objects at a higher flow
level. Since the flow level of the `location` and `label` inputs is
higher than that of the `image`, a synchronization error would occur
if the inputs were connected the other way round.

If a strict parent-child relationship the parent group will be
processed first. Operations should however prefer loose relationships
because they are more efficient and are less prone to deadlocks. In
some cases (such as PiiObjectReplicator) this is unfortunately not
possible.

The [PiiDefaultOperation::process()] function is called whenever a
group of sockets needs to be processed. The function must process one
and only one group at a time. To find out which group is active, use
the [PiiDefaultOperation::activeInputGroup()] function.

To group sockets into synchronization groups, use the
[PiiSocket::setGroupId()] function.


### Capturing Synchronization Events ### {#capturing_sync_events}

Most operations don't need to care about synchronizing sockets with
different object rates because they read one object from each input
socket on each processing round. There are however situations where
the object rate in one input socket is greater than that of the other.

PiiDefaultOperation has a virtual function called
[syncEvent](PiiDefaultOperation::syncEvent()) that needs to be
overridden if one needs to capture synchronization events. This
function is called just before an object is about to be processed in a
group or any of its child groups, and whenever all input sockets are
back on the same flow level.

Let us assume our custom operation has two input sockets: one that
receives an image (`_pImageInput`) and another that receives any
number of interesting objects (e.g. their coordinates) within the
image. The `process`() function is used to handle the objects, but
there you have no clue when the last location related to the large
image was received. This is where the `syncEvent`() function is
used.

~~~
MyOperation::MyOperation()
{
  // Default group id is 0
  addSocket(_pImageInput = new PiiInputSocket("image"));
  addSocket(_pLocationInput = new PiiInputSocket("location"));
  _pLocationInput->setGroupId(1);
}

void MyOperation::process()
{
  if (activeInputGroup() == 0) // large image received
    doSomething();
  else // location received
    doSomethingElse();
}

void MyOperation::synEvent(SyncEvent* event)
{
  // See if inputs are in sync with the image input.
  // If we have just two sync groups, this check is actually unnecessary.
  if (event->type() == SyncEvent::EndInput &&
      event->groupId() == _pImageInput->groupId())
    {
      // Do something here
      // PiiImagePieceJoiner, for example, would
      // perform the actual joining algorithm now.
    }
}
~~~


Configuration Interface {#configuration_interface}
-----------------------

Since all operations eventually inherit from QObject, QObject's
property system is used in configuring the operations. The details of
the system are best described in Qt's documentation. It suffices here
to show an example. We will modify our MyOperation so that instead of
incrementing bypassing integers by one it adds a user-configurable
value to it. The class declaration must be changed as follows:

~~~
class MyOperation : public PiiDefaultOperation
{
  Q_OBJECT

  Q_PROPERTY(int change READ change WRITE setChange);
public:
  MyOperation();

  int change() const { return _iChange; }
  void setChange(int change) { _iChange = change; }

protected:
  virtual void process();

private:
  int _iChange;

};
~~~

The Q_OBJECT macro is a Qt feature that needs to be included in all
classes using properties or the signal and slot system. We will omit
the details here. The next line in the class declaration tells that
the operation has a `change` property whose type is an `int`. The
value of the property is retrieved with a function called
`change`(). A new value is set with `setChange`(). The implementations
of these functions retrieve and store an internal variable called
`_iChange`. Now, the value of this property can be set with
`setProperty("change", value)`, where `value` is an integer.

The final step is to modify the process function to make use of the
property:

~~~
void MyOperation::process()
{
  PiiVariant obj = readInput();
  if (obj.type() == PiiVariant::IntType)
    emitObject(obj.valueAs<int>() + _iChange);
  else
    emitObject(obj);
}
~~~


Using Custom Operation in Configuration {#using_custom_operation}
---------------------------------------

If custom operations are used in a program whose configuration is
created programmatically (non-dynamically), it is not necessary to
create a dynamically loadable plug-in that exports the operation to
the resource database. Instead, the code can be compiled directly into
the application.

A common pitfall in creating custom operations is that their
declaration is not handled by `moc`, the Qt's meta object
compiler. The `moc` is needed in converting the properties, signals
and slots to C++ code. The declarations of all classes with the
Q_OBJECT macro must reside in files mentioned in the HEADERS section
of the project file. Let us assume that we placed the declaration of
class `MyOperation` into `MyOperation`.h. Let us further assume that
we need `MyOperation` in the thresholding program created in
[simple_example]. To make `moc` process our new operation,
`threshold`.pro needs to be modified by adding the new source and
header files:

~~~
SOURCES = Threshold.cc MyOperation.cc
HEADERS = MyOperation.h
~~~

Adding the operation to the configuration is straighforward. It will
be inserted between PiiThresholdingOperation and PiiImageFileWriter
like this:

~~~
PiiOperation* my = new MyOperation;
my->setProperty("change", -6);
engine.addOperation(my);
threshold->connectOutput("image", my, "input");
my->connectOutput("output", writer, "image");
~~~

Note that since the operation is not registered to the resource
database, it can not be created by its name with
[PiiOperationCompound::createOperation()]. Instead, an instance of the
operation is created and connected to PiiEngine with
[PiiOperationCompound::addOperation()].

In this example, the `change` property is set to -6. Since the
operation will never see other objects than images, the value will
affect nothing. But the example illustrates how to set your own
properties.
