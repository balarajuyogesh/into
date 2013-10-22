Interpreting Archived Data
==========================

The format of an archive depends on the archive type being used, but
serializable objects do not need to care about the underlying
format. Sometimes, it is however useful to know a bit more about
generic structures present in all archive formats. This section
describes how PiiOutputArchive stores data, independent of archive
format. The following (loosely) describes the contents of an archive
in EBNF notation.

~~~(ebnf)
archive           ::= header data*
data              ::= object | pointer
object            ::= trackedObject | nonTrackedObject
trackedObject     ::= pointerIndex classData
nonTrackedObject  ::= classData
pointer           ::= trackedPointer | nonTrackedPointer
trackedPointer    ::= pointerIndex className classData
nonTrackedPointer ::= className classData
classData         ::= version? data* parentClassData* data*
parentClassData   ::= classData
~~~

- `header` - each archive implementation has its own header, which may
  include any data. The header should contain a magic string/number to
  identify archive format, and major and minor archive version
  numbers.

- `pointerIndex` - the archive keeps track of addresses to saved
  objects. Whenever a tracked object is saved, its address will be
  inserted to a tracking list. If a reference to the same memory
  location is found, the pointer object will not be saved again.
  Instead, an index to the list will be saved. -1 means a null
  pointer.

- `className` - the name of a class as set by
  PiiSerializationTraits::ClassName. If a serializable type is not
  named, an empty string will be stored. "0" means a null pointer. In
  a text archive, string is preceded by its length. Thus, an empty
  string is stored as "0". A null pointer is "1 0" (length 1, contents
  "0").

- `version` - the version number (see PiiSerializationTraits::Version)
  always precedes class data. The version number is stored only if the
  PiiSerializationTraits::ClassInfo trait is set to `true`.

- `parentClassData` - the parent class' data, usually saved with
  [PII_SERIALIZE_BASE]. It is up to the programmer to actually save
  the parent class/classes, and their data can be preceded by other
  data, although this is usually a bad practice.

Now, consider the following example, taken from the beginning of a
real archive:

~~~(txt)
Into Txt 1 0
0 9 PiiEngine 1 0 0 0
0 11 1 15 PiiImageCropper 0 0 0
5 10 objectName 10 7 cropper
~~~

The first line is the header, which identifies the archive as a text
archive. The major and minor version numbers are 1 and 0.

The second line is a tracked pointer, whose tracking index is zero.
Its class name (PiiEngine) is nine characters long. The class name is
followed by its version number (1). The following three numbers are
base class version numbers (PiiOperationCompound, PiiOperation, and
QObject). The last two parent classes store no data.

On the third line, PiiOperationCompound has serialized its internal
list of operations. The first integer (0) is the version number of
QList, and 11 is the number of operations on the list. The first one
is a tracked pointer (tracking index one) with a 15-character class
name (PiiImageCropper). Its version number is zero, and so are the
versions of its bases (PiiOperation and QObject). Note that
serialization of PiiImageCropper skips a couple of intermediate
classes.

On the last line, PiiImageCropper has stored its properties. It has
five of them, and the first one is named "objectName" (10 chars).  The
value of the property is a QVariant, whose type id is 10
(QString). The string is "cropper". The rest of the properties and
operations are stored in a similar manner.

It is easy to see that interpreting the archive without knowledge of
the actual data types being stored is a tedious task. One may need the
source code to decrypt the storage format. In fact, text archives
aren't even as clear as shown here. They don't really have linefeeds
to aid interpretation.
