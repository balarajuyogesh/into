Saving and Loading Configurations
=================================

Saving and loading configurations is straightforward. All one needs to
do is to invoke the *serialization* of a PiiEngine instance.
Serialization is a process in which the members of a class instance
are recursively marhalled and written to an I/O device. In Ydin,
serialization happens through *archives*, objects that write and read
primitive data types in some specific format. Currently, there are two
actual implementations, one for plain text and another for binary
data. The choice between these formats is left to the user.  Binary
format is faster to read and write, but it is not as portable as plain
text, and non-readable to a human. Plain text is not very readable,
either, but can be edited with a text editor. It is a bit slower to
read and write, but often results in smaller files.

To save a configuration, one needs to open an I/O device, create an
archive that writes data to the device and write the data. Here is
how:

~~~
QFile file("file.cft");
file.open(QIODevice::WriteOnly);
PiiGenericTextOutputArchive ar(&file);
ar << engine;
~~~

It is possible to store any serializable data in a similar manner.
See documentation [chapter on serialiation](Serialization) for a
thorough explanation. Reading a configuration is just as simple:

~~~
QFile file("file.cft");
file.open(QIODevice::ReadOnly);
PiiGenericTextInputArchive ar(&file);
ar >> engine;
~~~

Any data written into an archive can be read back in the same order it
was written.

All bundled operations can be saved and restored with the shown
technique. If you need to serialize your custom operations, they need
to be made serializable. Placing them into a plug-in is the easiest
way (see [creating_plugins]), but making them serializable "manually"
is not hard either (see [Serialization]).
