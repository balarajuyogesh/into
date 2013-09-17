Serialization Wrappers
======================

If a type needs special handling in serialization, it is often
convenient to create a wrapper and write the serialization functions
for it. The Serialization library provides three built-in wrappers:
one for serializing binary data, another for storing name-value pairs
and the last one for serializing a base class.
[PiiSerializationUtil.h] defines some handy macros for dealing with
serialization wrappers.

Binary Objects {#serialization_wrappers_binary}
--------------

It is often necessary to store binary data. Since there is no way to
find the size of a data array given just a pointer, one needs to use
the PiiBinaryObject wrapper for serialization. PiiBinaryObject needs a
pointer to an array and the number of elements in it. Since the
wrapper does not know whether you are actually writing or reading the
data, you need to provide both values as references. In reading, the
values will be filled by the serialization library.

~~~
// Somewhere else
MyClass::MyClass() : _array(new float[5]) {}

template <class Archive> void MyClass::serialize(Archive& ar, const unsigned int)
  {
    int bytes = 5*sizeof(float);
    ar & Pii::binaryObject(_array, bytes);
  }
~~~


Name-Value Pairs {#serialization_wrappers_nvp}
----------------

Some archive formats such as XML accompany stored values with
names. By default, data is stored as such without tagging it in any
way. To support named data one should, however, always use the
PiiNameValuePair wrapper. Archive types that don't support named data
just ignore the name attribute.

A name-value pair can be created in a number of different ways. The
easiest one is to use the [PII_NVP] macro:

~~~
template <class Archive> void MyClass::serialize(Archive& ar, const unsigned int)
  {
    // _iValue is an int member
    ar & PII_NVP("value", _iValue);
    int elements = 5;
    // wrappers can be chained
    ar & PII_NVP("array", Pii::binaryObject(array, elements));
  }
~~~

To make variable naming easy, the library defines the [PII_SERIALIZE]
and [PII_SERIALIZE_NAMED] macros. If you want to store a variable with
its name as it appears in the class declaration, just do this:

~~~
template <class Archive> void MyClass::serialize(Archive& ar, const unsigned int)
  {
    PII_SERIALIZE(ar, _iValue);
    PII_SERIALIZE_NAMED(ar, Pii::binaryObject(array, elements), "_array");
  }
~~~


Base Class {#serialization_wrappers_baseclass}
----------

PiiBaseClass is the last built-in wrapper. Its purpose is to invoke
the [serialization of a base class](serialization_making_base). The
PiiBaseClass wrapper makes it possible to use different serialization
mechanisms for different levels of inheritance hierarchy. There is no
need to make the serialization functions virtual and directly call the
superclass' implementation.

! It is possible to skip parents in the hierarchy if you want to
override their default serialization behavior. For example, B derives
from A and C derives from B (A -> B -> C), it is possible to invoke
the serialization of A from C. It is also possible to totally omit the
parents.
