Compound Operations
===================

Compound operations are composed of sub-operations that together form
a logical processing unit. Compound operations are derived from
PiiOperationCompound, and PiiEngine is in fact just a compound
operation.

Inputs and Outputs {#compound_inputs_and_outputs}
------------------

The input and output sockets in a compound can be either *aliased* or
*proxied*. An alias is more efficient as it just reveals a socket that
is actually in one of the sub-operations. If you delete either the
sub-operation or the socket, the alias disappears.

Using a proxy (PiiSocketProxy) has the advantage that it won't
disappear unless explicitly told so. An alias will be destroyed if the
operation containing the aliased socket is destroyed. This will
obviously break all connections to the socket. Furthermore, a proxy
input can be connected to many input sockets within the compound.
This is not possible with an alias. The downside is that passing
objects through a proxy is slightly slower than through an alias.

Accessing Components in a Compound {#compound_accessing_components}
----------------------------------

All operations added as childs to a compound with
[PiiOperationCompound::addOperation()] will be placed under the
compound in QObject's object hierachy. That is, the
[parent()](QObject::parent()) function of the child operation will
return a pointer to the compound. Therefore, QObject::findChild() can
be used to find child operations by name. All direct child operations
can be retrieved with PiiOperationCompound::childOperations().

It is a common task to assign properties to the child operations of a
compound. To help this, the non-virtual member function
QObject::setProperty() is overridden by [PiiOperation::setProperty()].
The property() function is treated similarly. Although this breaks C++
conventions, it makes it easy to set properties to child
objects. Assuming a compound has a child operation with an object name
"foo" that has a property called "bar" one can access the property
directly through the compound by calling
`compound->property("foo.bar")`. The nesting can, of course, be deeper
if needed. "foo.baz.bar" is a valid property name if "foo" is a
compound that contains "baz". Care must however be taken that the
operations are accessed through PiiOperation pointers, not QObject
pointers. The same approach is used when accessing sockets within the
compound. See PiiOperationCompound for examples.

Serializing a Compound {#compound_serialization}
----------------------

Normally, when a compound is created, its default constructor creates
a number of child operations and connects them. This is unnecessary
when reading a serialized compound, because the child operations and
their connections will be retrieved from the archive.

The solution is to provide a special constructor that is only used
when an instance of the compound is created by the serialization
library.

~~~
class MyCompound : public PiiOperationCompound
{
  Q_OBJECT

  friend struct PiiSerialization::Accessor;
  PII_DECLARE_VIRTUAL_METAOBJECT_FUNCTION;

  // Does not create child operations
  MyCompound(PiiSerialization::Void);

  template <class Archive>
  void serialize(Archive& archive, const unsigned int /*version*/)
  {
    PII_SERIALIZE_BASE(archive, PiiOperationCompound);
    PiiSerialization::serializeProperties(archive, *this);
  }
public:
  // Creates child operations
  MyCompound();
};
~~~

To let the serialization library know it needs to use the special
constructor, use the following macro in the plug-in that registers the
operation:

~~~
PII_REGISTER_COMPOUND(MyCompound);
~~~


Compound Example {#compound_example}
----------------

TODO
