Dealing with Archives
=====================

Archive Implementations {#serialization_archives_impl}
-----------------------

The Serialization library provides two implementations of the archive
interface. (Note that the word *interface* here refers to a template
interface, not a declaration of an abstract class.)  Since input and
output archives are separated, the result is four classes, named
PiiBinaryInputArchive, PiiBinaryOutputArchive, PiiTextInputArchive,
and PiiTextOutputArchive.

Depending on archive "direction", the implementation must provide
operators for either reading (>>) or writing (<<) all primitive types
plus a fallback mechanism for all other types. The fallback mechanism
is a template function that the compiler selects if it cannot find an
explicit specialization. The archive implementations should just
redirect calls to the template function to PiiInputArchive and
PiiOutputArchive.

The usage of the built-in archive types is easy:

~~~
MyClass obj;

// Writing
QFile textFile(fileName);
textFile.open(QIODevice::WriteOnly);
PiiTextOutputArchive oa(&textFile);
oa << obj; // replace << with & if you wish

// Reading
QFile textFile(fileName);
textFile.open(QIODevice::ReadOnly);
PiiTextInputArchive ia(&textFile);
ia >> obj;
~~~

Any QIODevice works as the output or input device. This makes it
easy to (de)marshal data across network connections, for example.

! Reading and writing must occur in the same order.  Furthermore, if
you store an object as a pointer, you must read it back as a pointer.

Generic Archive Types {#serialization_archives_generic}
---------------------

Since serialization functions are templates, serializers must be bound
to an archive type. Although this approach results in efficient code,
it has a drawback: one must know all supported archive types at
compile time. As a solution, archive implementations with virtual
serialization functions are provided (PiiGenericInputArchive and
PiiGenericOutputArchive). If serializers are registered to the generic
archives, any archive implementing the interface (interface in the
traditional sense) can be used at run time. The downside is that
serialization must go through virtual function call resolution, which
leads in less efficient code. (It is still fast, don't worry.)

The PiiGenericInputArchive::Impl and PiiGenericOutputArchive::Impl
provide an easy way of converting any archive type to a generic
archive. They derive from the generic archive interfaces and also from
an implementation of the template interface. The classes just work as
proxies that forward virtual function calls to the corresponding
implementations in the template interface.

Generic versions of the basic archive types are provided as typedefs:
[PiiGenericTextInputArchive], [PiiGenericTextOutputArchive],
[PiiGenericBinaryInputArchive], and [PiiGenericBinaryOutputArchive].
