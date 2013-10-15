Handling Errors
===============

Errors in serialization are handled with exceptions. Typically, the
serialize() functions do not catch serialization exceptions but let
them flow to the caller. Most exceptions are thrown by the archives,
but it is possible to generate custom errors in serialization
functions. The type used for serialization errors is
PiiSerializationException.

Custom serialization errors are most conveniently handled with the
[PII_SERIALIZATION_CUSTOM_ERROR] macro declared in
[PiiSerializationException.h].

~~~
template <class Archive> void MyClass::serialize(Archive& ar, const unsigned int)
  {
    PII_SERIALIZATION_CUSTOM_ERROR("MyClass is not serializable!");
  }
~~~

Errors must be handled in the application that invokes serialization
code by catching PiiSerializationException. The error code tells the
reason of failure. If you want textual output, use the
[message()](PiiSerializationException::message()) function.
PiiSerializationException also has a
[PiiSerializationException::info()] function that returns extra
information for debugging purposes. It is filled with a class name
when no serializer or no factory is found for an object.

~~~
MyClass obj;
QFile textFile(fileName);
textFile.open(QIODevice::WriteOnly);
PiiTextOutputArchive ar(&textFile);

try
  {
    PII_SERIALIZE(ar, obj);
  }
catch (PiiSerializationException& ex)
  {
    qDebug("Error code: %d\nMessage: %s",
           ex.code(), qPrintable(ex.message()));
  }
~~~

Note that the archive may not always be able to find out the real
reason of an error. If stored data is corrupted, it may in principle
result in any type of an error. `InvalidDataFormat` is only thrown
when the archive notices it is reading something it isn't expecting.
