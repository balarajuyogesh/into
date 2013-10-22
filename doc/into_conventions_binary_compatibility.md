Library Code and Binary Compatibility
=====================================

For an overview of good library programming practices please
familiarize yourself with the following:

- [KDE library code policy](http://techbase.kde.org/Policies/Library_Code_Policy)

- [Binary Compatibility Issues With C++](http://techbase.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B)

Although Into doesn't exactly follow the KDE policy, the documents
give important insight on the matter.

D pointers in Into are called simply "d" just like in KDE. An
important difference is that the "private" class (in Into we speak
about a data class) is usually an inner class:

~~~
class MY_EXPORT MyBaseClass
{
public:
  MyBaseClass();
  virtual ~MyBaseClass();

  int intMember() const;

protected:
  class Data
  {
  public:
    Data();
    virtual ~Data();
    int iMember;
  } *d;
  MyBaseClass(Data* data);
};

// In .cc
MyBaseClass::Data::Data() : iMember(0) {}
MyBaseClass::Data::~Data() {}

MyBaseClass::MyBaseClass() : d(new Data) {}
MyBaseClass::MyBaseClass(Data* data) : d(data) {}
MyBaseClass::~MyBaseClass() { delete d; }

int MyBaseClass::intMember() const { return d->iMember; }
~~~

A derived class inherits the inner data class of its parent class. 
Therefore, the both the class and its data class must have a
virtual destructor. The getter function `intMember`() refers to
the data class directly by its name, `d`.

~~~
class MY_EXPORT MyDerivedClass
{
public:
  MyDerivedClass();
  ~MyDerivedClass();
  double doubleMember() const;
  void calculate();

private:
  class Data : public MyBaseClass::Data
  {
  public:
    Data();
    double dMember;
  };
  PII_D_FUNC;
};

// In .cc
MyDerivedClass::Data::Data() : dMember(0) {}
MyDerivedClass::MyDerivedClass() : MyBaseClass(new Data) {}
MyDerivedClass::~MyDerivedClass() {}

double MyDerivedClass::doubleMember() const { return _d()->dMember; }
void MyDerivedClass::calculate()
{
  PII_D;
  d->dMember *= 1.1;
}
~~~

Since the base class is responsible for deleting the data class, 
`MyDerivedClass` doesn't do it in its destructor. It should however
have one to retain binary compatibility in case you later need it.

The `doubleMember`() member function cannot directly access 
`d`->dMember because the type of `d` is `MyBaseClass::Data`, which
only has `iMember`. Instead, it uses the `_d`() inline function
defined by the `PII_D_FUNC` macro. The function casts `d` to the
type of derived class' data, in this case `MyBaseClass::Data`.

If a member function needs the `d` pointer many times, it is
faster to store the `d` pointer on the stack. To do this, place
the `PII_D` macro at the beginning of a function body. In a 
`const` function, use `const PII_D`. The macro defines a local
variable called (surprise) `d`, which hides the class member.
