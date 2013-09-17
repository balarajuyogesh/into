Basic Concepts
==============

Archives {#serialization_concepts_archives}
--------

Archive is an abstract concept for an object that defines the format
of data and is able to read and write data in that format.  The
serialization library is completely data format agnostic. The storage
format may be changed simply by providing a new archive
class. Currently, binary and plain text archives are supported.

Although an archive can be specialized for a specific storage format
and storage device only, a better design pattern is to separate
archive and the actual I/O device. This way it is possible to use any
storage format with any I/O device. This allows one to separate data
format from storage format.

Archives are separated into two categories: input and output
archives. Since they always come in pairs, the obvious question is why
they haven't been combined into one? The reason is that separate input
and output archives allow us to create template serialization
functions that work in both directions. Both archive types define an
`operator`& function that works as a '>>' for input archives and as a
'<<' for output archives.

The class hierarchy for archives is rather complex, but it makes it
possible to easily customize archives at any level. Here, we'll focus
on input archives. The class hierarchy for output archives is
equivalent.

All input archives are derived from the PiiInputArchive template.  The
purpose of this archive is to overload the & operator so that it uses
the >> operator of the archive implementation. An archive
implementation must provide a >> operator for all primitive types and
pointers. PiiInputArchive also provides support for handling pointer
serialization. It provides a template implementation for the >>
operator that works as a fallback when the derived archive
implementation does not provide a specialized one. The implementation
takes care of handling references and pointers so that pointers and
references to the same memory address are correctly restored.

The actual archive implementations must only provide specialized
implementations of the >> operator for reading primitive types and
QStrings. Whenever the operator & is invoked in source code,
PiiInputArchive converts it to `operator`>>. If the archive
implementation has a specialized implementation for the data type in
question, it will be invoked. If not, the fallback function will be
called.

Serializers and Factory Objects {#serialization_serializers}
-------------------------------

All non-primitive types are stored and restored through special
objects called serializers. Since serialization functions are
templates that are bound to a certain archive type at compile time,
serializers must be registered to a specific archive type.

The Serialization library uses two different types of serializers, one
for serializing class instances whose type is not known at compile
time and the other for all others. If the type of the class is known
at compile time (it is not serialized through a base class
pointer/reference), the PiiSerialization::serialize() template
function will be invoked. If a template specialization for the type to
be serialized is available, it will be called. Otherwise, the fallback
function calls a `serialize`() member function of the serializable
object through the PiiSerialization::Accessor struct.  Declaring this
stuct as a friend of a serializable class makes it possible to make
the serialization function(s) private.

The only portable way of requesting the name of a type at run time is
to use a virtual function. The serialization library uses meta objects
for this (See [PiiVirtualMetaObject.h]). Therefore, the base class of
virtually serialized objects must declare a virtual piiMetaObject()
function. However, if you have an alternative way of doing this, just
override the PiiSerialization::metaObjectPointer() function.

If the type of a class cannot be known at compile time, the type is
said to be *dynamic* (see PiiSerialization::isDynamicType()). In
this case a serializer with a virtual serialization function needs
to be registered to the serializer map of an archive type. The name
of the type (obtained from the meta object) is used as a key in
look-ups. Once the serializer is found, its *virtual* 
`serialize`() member function is invoked.

Another requirement for dynamic types is to register a factory
object for creating instances of the class when deserializing (see
PiiSerializationFactory). Again, the class name (read from the
archive) is used as a key in look-ups.

The library needs to be specifically told to use the serializer and
factory maps. The way to do this is to override the
PiiSerialization::isDynamicType() function (See
[PII_SERIALIZATION_DYNAMIC]).


Serializable Objects {#serialization_serializables}
--------------------

An object can be made serializable in a few different ways. All
primitive types are serializable without a serializer because the
archive implementations are required to provide the << and >>
operators for them. Class and struct types need a custom serialization
mechanism.

In *internal* serialization, the serializable object itself has a
member function called `serialize`():

~~~
class MyClass
{
  friend struct PiiSerialization::Accessor;
  template <class Archive> void serialize(Archive& archive, const unsigned int version);
};
~~~

The function can be public, but since it should never be used
directly, it is wise to leave it private and declare
PiiSerialization::Accessor as a friend.

In *external* serialization, there is no need to modify the class
itself. One only needs to provide a specialization of
PiiSerialization::serialize() for the serializable type. The downside
is that one can only work through the public interface of the
serializable object.

~~~
template <class Archive> inline void serialize(Archive& archive, MyType& value, const unsigned int version)
{
  // do serialization stuff here
}
~~~

In *virtual* serialization, the actual serialization mechanism may be
either internal or external, but it is bound at run time.  Virtual
serializers must be bound to a class name by registering it to the
serialization map of an archive type.


Serialization Traits {#serialization_traits}
--------------------

Traits are a C++ programming concept in which properties are bound to
types at compile time. Traits are implemented as template classes with
static constants which can be evaluated by the compiler. The
PiiSerializationTraits namespace holds traits that control the
serialization of objects. The controllable aspects of serialization
are:

- [Tracking](PiiSerializationTraits::Tracking). Whether or not memory
  addresses to this type of objects will be tracked. See
  [serialization_tracking] for details. Primitive types will never be
  tracked. For all other types, tracking is enabled by default. If you
  need to store a pointer graph to, say, an `int`, create a
  [wrapper](serialization_wrappers) or use a typedef:

~~~
typedef int MyInt;
PII_SERIALIZATION_TRACKING(MyInt, true)
template <class Archive> void serialize(Archive& ar, MyInt& i, const unsigned int)
  {
    archive & (int&)i;
  }
~~~

- [ClassInfo](PiiSerializationTraits::ClassInfo). Will class
  information be stored? Currently, the class information only
  includes the version number.

- [Version](PiiSerializationTraits::Version). The version number of a
  type. The default version number is 0. See
  [Versioning](serialization_making_version) for details.

- [ClassName](PiiSerializationTraits::ClassName). The class
  name. Class name must be defined for all dynamic types because it is
  needed to a) instantiate the object through a factor b) look up a
  serializer for serializing the object.

- [IsAbstract](PiiSerializationTraits::IsAbstract). Is it possible to
  instantiate the class? Abstract superclasses of serializable types
  need to turn this trait to `true`. It is also needed for
  non-abstract superclasses that provide no default constructor.

The easiest way of controlling the traits is through macros in
[PiiSerializationTraits.h].

Note that the traits affect archive format. Changing them may
invalidate previously stored data. Thus, one should change the
defaults with caution.
