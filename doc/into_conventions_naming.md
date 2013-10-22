Naming
======

*Much of the material in this section is copied from the "C++ Coding
Standard" text back in 2002 by Todd Hoff. Copyright (C) 1995-2002 Todd
Hoff.*

Names are the heart of programming. Only a programmer who understands
the system as a whole can create a name that "fits" with the system. 
If the name is appropriate everything fits together naturally,
relationships are clear, meaning is derivable, and reasoning from
common human expectations works as expected.

Classes are often nouns. By making function names verbs and following
other naming conventions programs can be read more naturally.

- Name the class after what it is. If. you can't think of what it is
  that is a clue you have not thought through the design well enough.

- Compound names of over three words are a clue your design may be
  confusing various entities in your system. Revisit your design. Try
  a CRC card session to see if your objects have more responsibilities
  than they should.

- Avoid the temptation of bringing the name of the class a class
  derives from into the derived class's name. A class should stand on
  its own. It doesn't matter what it derives from. Generic suffices
  such as "Driver" or "Model" are OK.

- Usually every function performs an action, so the name should make
  clear what it does: checkForErrors() instead of errorCheck(),
  dumpDataToFile() instead of dataFile().

- When confronted with a situation where you could use an all upper
  case abbreviation instead use an initial upper case letter followed
  by all lower case letters. We use PiiRgbNormalizer instead of
  PiiRGBNormalizer and PiiFft instead of PiiFFT.

The Into style of naming types and functions closely follows the Java
standard, which is also similar to the Qt style. In this chapter, the
following topics are considered:

- [Class Names](into_conventions_classnames)
- [Function Names](into_conventions_functionnames)
- [Class Attribute Names](into_conventions_attrnames)
- [Function Parameters](into_conventions_margnames)
- [Type Prefixes](into_conventions_type_prefixes)
- [Variable Names on the Stack](into_conventions_stacknames)
- [Type Names](into_conventions_tnames)
- [\[defines] and Macro Names](into_conventions_mnames)
- [Enumerations](into_conventions_enames)
- [Templates](into_conventions_tempnames)
- [File Names](into_conventions_filenames)
- [Exceptions](into_conventions_exceptions)


Class Names {#into_conventions_classnames}
-----------

- Use upper case letters as word separators, lower case for the rest
  of a word (e.g. PiiOperationCompound, PiiInputSocket).

- First character in a name is upper case

- Usually, there are no namespaces for classes. Instead, each class
  name is preceded by "Pii". Namespaces are used in plug-ins (see
  PiiImage, for example) to wrap functions that do not logically
  belong to any class. If a class/struct is defined within a
  namespace, its name must not be prepended with "Pii"
  (e.g. Pii::IsInteger).


Function Names {#into_conventions_functionnames}
--------------

- Use the same rule as for class names, except for the first letter
  which must be lower case. This rule makes it easy to distinguish
  between function calls and constructor calls.

- Property getters and setters should be named according to Qt
  style. That is, a property "fooBar" should have a getter called
  `fooBar`() and a setter called `setFooBar`().

Type Prefixes {#into_conventions_type_prefixes}
-------------

- Type prefixes should be used to indicate the type of variables, both
  on the stack and as class attributes. The following is an incomplete
  list of suitable prefixes:

- `i` - `int`
- `l` - `long`
- `d` - `double`
- `f` - `float`
- `str` - QString
- `lst` - QList
- `vec` - QVector
- `map` - QMap
- `mat` - PiiMatrix
- `clr` - PiiColor
- `p` - pointer. Use this for real pointers and for classes that
  masquerade as pointers, such as PiiSmartPtr.

- In some occasions, it is useful to be able to easily change a type
  to another. For example, if there is no need to know that a storage
  is a QVector instead of QList, `lst` could be used for both.

- Do not exaggerate. A PiiMatrix<PiiColor<int> >* does not need to be
  `piclrmatImage`. `pColorImage` is a better name.


Class Attribute Names {#into_conventions_attrnames}
---------------------

- All private and protected attribute names must be prepended with an
  underscore (_), followed by a type prefix.

- After the underscore and the type prefix, use the same rules as for
  class names.

Examples:

- `_iChildren` - number of children, an `int`
- `_lstChildren` - the children, a QList, whose content type can be
  anything. We won't go into extremes: template types can be left
  unspecified.
- `_strName` - name, a QString
- `_pData` - pointer to some data
- `_dSpeed` - a double
- `_fSpeed` - a float

**Exception**: the *d* pointer of a class should be called just 
`d`, not `_d`.


Function Parameters {#into_conventions_margnames}
-------------------

- The first character should be lower case.

- All word beginnings after the first letter should be upper case as
  with class names.

- Function parameters do not need to be prefixed.


Variable Names on the Stack {#into_conventions_stacknames}
---------------------------

- Follow the same convention as with class attributes, just without
  the initial underscore.


Type Names {#into_conventions_tnames}
----------

- Use the same naming policy as for classes.

Example: typedef PiiSharedPtr<MyObject> MyObjectPtr;


Defines and Macro Names {#into_conventions_mnames}
-----------------------

- Put defines and macros in all upper case using '_' as a word
  separator. Macro parameters should follow the same convention.

Example: [PII_REGISTER_OPERATION]


Enumerations {#into_conventions_enames}
------------

- For both the type and the enumerated values use the same naming
  policy as for classes.

- Repeat a keyword in all enumerated values. See @ref
  Pii::PropertyFlag for an example.

- Prefer enums for boolean values in function parameters to make the
  code more readable.

~~~
// Instead of this
void drive(bool careful);

// Prefer this
enum DrivingMode { DriveCarefully, DriveCarelessly };
void drive(DrivingMode mode);

// See the difference:
drive(true);
drive(DriveCarefully);
~~~


Templates {#into_conventions_tempnames}
---------

- Generic types with no special meaning are denoted with capital
  letters. The convention is to name the first type `T`, the second
  one `U` and so on, aplhabetically. If the types have similar
  function, they can also be named `T1`, `T2` etc.

- If a type has a clear role, the role should be reflected in the type
  name, which should be formatted according to class name naming
  conditions.

- The names of non-typename template parameters should follow function
  name naming conditions.

- Leave a space after the `template` keyword.

~~~
template <class T> PiiMatrix;
template <bool condition, class Then, class Else> struct If;
~~~


File Names {#into_conventions_filenames}
----------

- Class *declaration* is always placed into a file whose name equals
  the class name, and has a .h extension. The same applies to
  namespaces. Examples: PiiOperation.h, PiiImage.h.

- Class definition is placed into a .cc file whose name matches the
  header file. If the class is a template, the definition is placed
  into a file that ends with -templates.h. Example:
  PiiLbp-templates.h. Both .cc and -templates.h may be present.


Exceptions {#into_conventions_exceptions}
----------

- It is a good idea to make iterators and function objects compatible
  to stl. In these contexts, function and type names should match stl,
  whose naming conventions are different. For example, [adaptable
  unary function objects](Pii::UnaryFunction) must contain `typedefs`
  for `argument_type` and `result_type` (not `ArgumentType` and
  `ResultType`).

@see PiiMatrix::begin()
@see PiiMatrix::constEnd()
@see Pii::Abs
@see Pii::Hypotenuse
