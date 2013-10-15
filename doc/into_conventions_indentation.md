Indentation
===========

Indentation may be the most common flavor of holy wars when it comes
to programming style, and that is why we start with it. Since the
other holy war between Emacs and Vi has a winner and the default
indentation style in Emacs is the [GNU
style](http://www.gnu.org/prep/standards/standards.html), the winner
of the indentation holy war is clear. Here's what Into code looks
like:

~~~
class IndentedClass
{
public:
  void indentedFunc();
};

void IndentedClass::indentedFunc()
{
  if (avoidEternalLoop())
    {
      for (int i=0; i<100; ++i)
	std::cout << i << '\n';
    }
  else
    {
      bool bDone = false;
      do
        {
	  std::cout << "I'm still here.\n";
        }
      while (!bDone);
    }
}
~~~

Indentations are spaces, not tabs. And there are two of them. No
more, no less.

! The GNU coding standard is followed only when it comes to
indentation. The GNU standard is for C, and Into is C++.
