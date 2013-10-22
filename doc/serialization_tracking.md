Tracking
========

The Serialization library tracks the memory address (a.k.a pointers)
of serialized objects to make sure objects are serialized only
once. Whenever saving of a tracked object is requested, the library
checks the memory address of the object. If it matches that of a
previously saved object, only a reference to the previous object will
be stored. When the objects are read back the library makes sure all
pointer and references remain valid independent of the saving
order. The programmer may however affect the performance of
deserialization by the saving order.

If an object is first saved through a pointer, the Serialization
library has no way of knowing whether it will later be saved as an
object. Thus, if the object is later saved again as an object, it will
be saved again. When reading the archive, the object will be first a
memory location through the pointer. Later, the library will notice
that the object itself was stored. The contents of the previously
allocated object must be released. Better performance would be
achieved by first storing the object itself.

To be able to preserve memory references, the Serialization library
must know the *final* location of deserialized objects. Therefore, if
a tracked object is deserialized into a temporary memory location and
subsequently moved to its final location, the move must be told to the
deserializer. The following example illustrates this. Note that
[PiiSerializationUtil.h] provides a function for saving QLists, so you
don't actually need to do this manually.

~~~
QList<MyObj> lst; // filled somehow

// Read elements in sequence in your load() function:
for (int i=0; i<elementCount; i++)
  {
    MyObj obj;
    archive >> obj;
    lst << obj;
    // "archive" now thinks &obj is the address of the deserialized object.
    // If a pointer refers to this object, it must be updated
    archive.objectMoved(obj, lst.last());
  }
~~~

The PiiInputArchive::objectMoved() function updates all references
currently pointing to a certain memory address to a new address. If
you don't signal the move, any subsequently restored pointer to the
object will point to the local variable.

Tracking can be selectively turned off with the
PiiSerializationTraits::Tracking trait.

The tracking mechanism is implemented in PiiOutputArchive and
PiiInputArchive. The former uses PiiTrackedPointerHolder objects to
store pointers to tracked objects. Please refer to the
PiiSerialization::createTrackedPointerHolder() and
PiiSerialization::rereferencePointer() functions if you want to
serialize reference-counted objects or customize the tracking
behaviour in other ways.
