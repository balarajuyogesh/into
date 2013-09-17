Miscellaneous Conventions
=========================

- Function name is immediately followed by opening paren, which is
  immediately followed by the first parameter. Parameters are
  separated with a comma *and* a space.

~~~
functionCall(firstParam, secondParam);
~~~

- Control statements (`if`, `for`, `while`, `switch`, `catch`) and the
  opening paren are separated with a space. This distinguishes them
  from function calls.

~~~
if (_dAngularVelocity == M_PI)
  halfRevolutionPerSec();
~~~

- Use spaces to separate statements in `for` clauses.

~~~
// i = 0, 1, 2
for (int i=0; i<3; ++i) {}
// i = 2, 1, 0
for (int i=3; i--; ) {}
~~~

- Favor pre-increment/decrement to post-increment/decrement.

~~~
while (true)
  {
     // Use this ...
     ++_iCounter;
     // ... instead of this
     _iCounter++;
  }
~~~

- The `const` keyword is there to help you and the compiler. Use it
  whenever possible. A member function that does not alter the state
  of a class should always be `const`, even if it returned a pointer
  or a reference to a member. A complex type as a function parameter
  is usually passed as a `const&`.

- The order of class scopes is interface first. That is, the first
  scope is public, then protected, then private. In all scopes the
  order of declarations is types, member functions, attributes.

~~~
class MyClass
{
public:
  enum MyEnum { Value0, Value1 };
  void pubFunc();

protected:
  void protFunc();
  int _iValue;

private:
  typedef int Scalar;
  int privFunc(void);
  float _fValue;
};
~~~

- Never use `NULL`. It is not C++. When you check for a null pointer,
  compare to zero.

~~~
if (pData != NULL) {} // NO!
if (pData) {} // Rather not.
if (pData != 0) {} // Yes.
~~~

- Throw and catch exceptions as references.

~~~
try
  {
    throw PiiException(tr("An error occured."));
    // Or better yet:
    PII_THROW(PiiException, tr("An error occured."));
  }
catch (PiiException& ex)
  {
  }
~~~

- Leave at least one empty line between function/class definitions.

~~~
// This is incorrect
void funcA()
{
}
void funcB()
{
}

// This is correct
void funcC()
{
}

void funcD()
{
}
~~~

- We use the linefeed character (\n) as a line terminator.
  Windows-style (\r\n) terminators are strictly prohibited.

- Keep lines shorter than 120 characters.
