Design Principles and Features
==============================

Design Principles {#serialization_principles}
-----------------

Although the Serialization library may seem rather complex and hard
to understand at first sight, most of its obscurity has good
reasons.

- **Performance**. Extensive use of templates makes it unnecessary to
  use virtual functions. Most serialization calls can be optimized to
  direct stream operations by modern compilers.

- **Flexibility**. The use of templates makes it possible to override
  any behavior without touching existing code and with no run-time
  overhead.

- **Ease of use**. Although the library itself is complex, its use is
  not. Serializable objects can be stored and restored with a single
  function. There is often no need to write separate save/load
  functions. All primitive data types and most Qt types can be
  serialized with a single line of code.


Features {#serialization_features}
--------

This section shortly summarizes the main features of the library.
Detailed documentation is provided in other places.

- **Format-independend serialization**. Serializable objects just
  write and read [serialization_concepts_archives] that define the
  format of data. The library comes with support for binary and plain
  text formats.

- **Versioning**. Objects and archives are independently
  versioned. The version number is automatically inspected for
  incompabilities, and serializable objects have an option to be
  backwards-compatible when needed.

- **Non-intrusive**. The library makes it possible to write
  serializers for types whose code is inaccessible.

- **Graph reconstruction**. The process of solving memory references
  (via pointers or references) is often called graph
  reconstruction. The library ensures that a memory location is only
  serialized once and that all references remain valid when read back
  (even with reference-counted pointers). The
  [tracking](serialization_tracking) of memory addresses can be
  selectively turned off for any object type.

- **Shared library support**. The library supports serialization of
  classes defined in dynamically loaded libraries.

The Serialization library owes much to boost::serialization, but has
some notable differences. While boost::serialization builds on stl, we
use Qt. The Serialization library supports DLLs, which the boost
counterpart does not (at least it did not at the time of writing
this). Since we are not supporting obsolete compilers (like MSVC 6.0),
the code also has fewer hacks.
