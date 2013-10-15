Making a Class Serializable
===========================

Basic Serialization {#serialization_making_basic}
-------------------

The simplest way of making a class serializable is to implement a
`serialize`() member function:

~~~
class MyClass
{
private:
  int _i;
  double _d;

  // Serialization declarations
  friend struct PiiSerialization::Accessor;
  template <class Archive> void serialize(Archive& archive, const unsigned int version)
    {
       archive & _i;
       archive & _d;
    }
};
~~~

The `friend` declaration isn't actually necessary, but makes it
possible to hide the serialization function from the cruel world.  The
Serialization library invokes the same serialization function for both
input and output archives. Since they both have the `operator`&
defined, the same function works both in saving and loading data. Note
that serialization depends on the order of reading / writing. If you
write `_i` first, you *must* read it back first.

Now, let's assume you need to serialize a class whose code you cannot
or don't want to alter. Its public interface looks like this:

~~~
class ProprietaryClass
{
public:
  int value() const;
  void setValue(int value);
};
~~~

Since modification of the class declaration is not feasible, one needs
to implement an external serialization function. The problem is that
the member variables cannot be directly accessed. In saving, the
`value`() member function must be used. In loading, one needs its
`setValue`() counterpart. The library provides a macro,
[PII_SEPARATE_SAVE_LOAD_FUNCTIONS], to conveniently solve this
problem. The macro creates a `serialize`() function that invokes
either `save`() or `load`(), depending on the archive type.

~~~
PII_SEPARATE_SAVE_LOAD_FUNCTIONS(ProprietaryClass)

namespace PiiSerialization
{
  template <class Archive> inline void save(Archive& ar, ProprietaryClass& obj, const unsigned int version)
    {
       int value = obj.value();
       ar << value;
    }

  template <class Archive> inline void load(Archive& ar, ProprietaryClass& obj, const unsigned int version)
    {
       int value;
       ar >> value;
       obj.setValue(value);
    }
}
~~~

Save and load can be separated also for internal serialization, if
needed:

~~~
class MyClass
{
private:
  friend struct PiiSerialization::Accessor;
  PII_SEPARATE_SAVE_LOAD_MEMBERS
  template <class Archive> void save(Archive& archive, const unsigned int version)
    {
       // ...
    }
  template <class Archive> void load(Archive& archive, const unsigned int version)
    {
       // ...
    }
};
~~~

Serializing Base Class {#serialization_making_base}
----------------------

When derived objects are serialized, their base class also needs to be
serialized. Since serialization of an object must always be controlled
by the Serialization library, one should **never** call the
`serialize`() member function of a base class directly.  Keeping the
serialization function private as suggested helps in this.

[PiiBaseClass.h] provides means of serializing base classes. The
easiest way of serializing the base class is the [PII_SERIALIZE_BASE]
macro:

~~~
class MyDerivedClass : public MyClass
{
private:
  float _f;

  friend struct PiiSerialization::Accessor;
  template <class Archive> void serialize(Archive& archive, const unsigned int version)
    {
      PII_SERIALIZE_BASE(archive, MyClass);
      archive & _f;
    }
};
~~~

This approach has the advantage that the subclass does not need to
know how the superclass has serialization implemented. It may be
either internal or external, but we don't need to care.

The serialization of the base class goes through a
[wrapper](serialization_wrappers) for which the
[tracking](serialization_tracking) and classinfo
[traits](serialization_traits) are turned off. Thus, the wrapper
itself leaves no marks into the archive. The version number of the
base class will be stored, which makes it possible to change the
storage format on all levels of the inheritance hierarchy.

Serializing through Base Class Pointer {#serialization_making_virtual}
--------------------------------------

So far we have been dealing with types we know at compile time.
Serialization of such types can be resolved by the compiler, provided
that proper declarations for the serialization function templates are
available.

Imagine a class called `Garage` whose purpose is to store a collection
of `Cars`. `Car` is an (abstract) superclass for `Truck`, `Mazda`, and
`Volvo`. `Truck` is inherited by `Sisu` and a number of other makes
that will be available upon loading of dynamic libraries. The best
`Garage` can do is to keep a list of pointers to `Cars`, because it
has no way of knowing all possible types at compile time. The class
declarations look like this:

~~~
class Garage
{
public:
  // I'm a collector; there is no removeCar() function.
  void addCar(Car* car) { _lstCars.apppend(car); }
private:
  QList<Car*> _lstCars;
};

// We omit the public interface. Just private stuff is shown.
class Car
{
  int _iWheels;
};

class Truck : public Car
{
  bool _bHasTrailer;
};

class Sisu : public Truck
{
  double _dExtraWidthForEURoads;
};

class Mazda : public Car
{
  bool _bIsRusty;
};

class Volvo : public Car
{
  enum Sound { TractorSound, BulldozerSound, TruckSound };
  Sound _sound;
};
~~~

Now, how do we ensure that the members of the derived classes are
saved when a `Garage` is stored?

The Serialization library uses PiiMetaObject to store information
related to a type. Currently, such information include class name and
version number. Since requesting the meta object for `Car` would not
help in our situation, there must be a way of finding the "most
derived" class. The only way this can be performed across dynamic
library boundaries is to implement a virtual piiMetaObject()
function. (Yes, we have seen the vtable tricks in
boost::serialization.)

The downside of the virtual meta object function is that the
declaration of the base class must be accessible. If this is not the
case, one needs to create an empty subclass just for serialization
purposes. The empty subclass declares the virtual metaobject
function. To make `Cars` serializable through the base class, we make
the following change:

~~~
class Car
{
  friend struct PiiSerialization::Accessor;
  PII_VIRTUAL_METAOBJECT_FUNCTION

  // The serialization function
  template <class Archive> void serialize(Archive& archive, const unsigned int)
    {
      PII_SERIALIZE(archive, _iWheels);
    }

  int _iWheels;
};

// ...

class Volvo : public Car
{
  friend struct PiiSerialization::Accessor;
  PII_VIRTUAL_METAOBJECT_FUNCTION

  template <class Archive> void serialize(Archive& archive, const unsigned int)
    {
      PII_SERIALIZE_BASE(archive, Car);
      archive & PII_NVP("sound", PII_ENUM(_sound));
    }

  enum Sound { TractorSound, BulldozerSound, TruckSound };
  Sound _sound;
};

// Specialize meta object for this type to call the virtual function
PII_SERIALIZATION_VIRTUAL_METAOBJECT(Car)
~~~

The [PII_VIRTUAL_METAOBJECT_FUNCTION] macro must be present in all
class declarations that are serialized through a base class pointer,
and the base class itself. The base class must be annotated with the
[PII_SERIALIZATION_VIRTUAL_METAOBJECT] macro.  In our example, only
`Car` needs it. If the base class is abstract, one needs to use the
[PII_SERIALIZATION_ABSTRACT] macro.

Now that the Serialization library knows the meta object it must find
a serializer and a factory (an object that can create a class instance
given a key) for the corresponding class. The class name is used as a
look-up key for this. Every archive type has its own serializer map to
which serializers must be registered by the the class name. The
[PII_INSTANTIATE_SERIALIZER] macro registers a serializer for an
archive type. The [PII_INSTANTIATE_FACTORY] creates the factory, which
is not specific to any archive type. But before the factory and the
serializers can be created we need to name the type. This is done with
the [PII_SERIALIZATION_NAME] macro.

We need to instantiate the serializers and factories for all new types
derived from `Car:`

~~~
PII_SERIALIZATION_NAME(Car);
PII_INSTANTIATE_SERIALIZER(PiiTextInputArchive, Car);
PII_INSTANTIATE_SERIALIZER(PiiTextOutputArchive, Car);
PII_INSTANTIATE_FACTORY(Car);

// Repeat for other types
~~~


Making a Class Serializable the Easy Way {#serialization_making_easy}
----------------------------------------

Now that we are familiar with the concepts or registering serializable
types we can just forget the manual way of doing things. All one
really needs to do is to define some configuration values and include
a single file called PiiSerializableRegistration.h. `Car` could be
registered simply this way:

~~~
#define PII_SERIALIZABLE_CLASS Car
#define PII_VIRTUAL_METAOBJECT
#include <PiiSerializableRegistration.h>
~~~

PiiSerializableRegistration.h can be included as many times as
necessary. See the [complete reference](serializableregistration_h)
for details.


Versioning {#serialization_making_version}
----------

It sometimes happens that a class changes format while one still has
to support previously stored archives. This is possible if a class has
the PiiSerializationTraits::ClassInfo trait turned on (it is by
default). In deserialization, the serialize() function will be given
the version number of the stored object. The Serialization library
checks that the version number of valid and will fail if it is larger
than the current version number of the object.

Let us assume you add a `_iDoors` member to the `Car` class. The new
declaration and registration for the class is as follows:

~~~
class Car
{
  friend struct PiiSerialization::Accessor;
  PII_VIRTUAL_METAOBJECT_FUNCTION

  // The serialization function
  template <class Archive> void serialize(Archive& archive, const unsigned int version)
    {
      PII_SERIALIZE(archive, _iWheels);
      // Serialize _iDoors only if the version number is not zero
      if (version > 0)
        PII_SERIALIZE(archive, _iDoors);
    }

  int _iWheels;
  int _iDoors;
};

#define PII_SERIALIZABLE_CLASS Car
#define PII_VIRTUAL_METAOBJECT
// The current version number of Car is 1
#define PII_SERIALIZABLE_CLASS_VERSION 1
#include <PiiSerializableRegistration.h>
~~~

Whenever you make changes that affect the archive format and still
want to support old archives, increase the version number. Note that
the Serialization uses only eight bits (256 different values) to store
the version number to save space. If you need more, store the version
number as an integer in the beginning of your serialization function.

Non-default Constructors {#non_default_constructors}
------------------------

Although it is usually bad practice not to provide a default
constructor for a class, there may be situations where a serializable
class must be created with a non-default constructor.

The serialization library uses the PiiSerialization::Constructor
struct to create new class instances. The default implementation
returns `new Type` for a non-abstract `Type`, and a null pointer for
abstract types. Specializing the PiiSerialization::Constructor
structure either fully or partially makes it possible to change the
way object instances are created.  For convenience, the
[PII_SERIALIZATION_CONSTRUCTOR] and
[PII_SERIALIZATION_NORMAL_CONSTRUCTOR] macros are provided.
