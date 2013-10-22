Common Problems
===============

The most common problems with the serialization library are caused by
wrong declaration order. It is sometimes tricky to find correct
ordering. The following tips may help if you run into trouble.

- Include PiiSerializationUtil.h first. It contains serializers for
  many Qt types that you will need in many cases. If a serialization
  code later refers to a Qt type whose serializer hasn't been
  declared, compilation will fail with error messages such as "Class
  'QObject' has no member 'serialize'".

- The same applies to any other type: the serializer must be declared
  before the serialization headers are included. If you can't
  serialize a PiiMatrix, for example, include PiiMatrixSerialization.h
  first.

- The general rule of thumb to avoid declaration order problems:
  always include archive headers
  (`Pii{Generic,Text,Binary},{Input,Output}Archive.h`) and files that
  include archive headers ( `PiiSerializableRegistration`.h and
  `PiiSerializableExport`.h) last.  Otherwise, serialization functions
  may not find the declarations for serializing custom objects and
  fall back to default serialization. Symptom: the serialization
  library "cannot find serializer".

- Do not serialize temporary tracked objects. If you serialize a
  pointer to a tracked object and delete it while the archive still
  exists, chances are that another tracked object will later get the
  same memory address on the heap. This plays havoc with the tracking
  mechanism. See PiiTrackedPointerHolder on how to work around
  problems with automatic deletion of reference-counted objects.
