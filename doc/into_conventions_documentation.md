Documenting Code
================
 
When another person is developing your code, the thing he/she needs
most is documentation. Working code comes only second. If the code
does not work, the user still has a document on what to except. If the
code works but has no documentation, it is not trivial to verify that
the code really works as intended. The value of undocumented buggy
code approaches zero.

The most general principle is simple: everything that can be
considered as interface to the outer world must be formally
documented. This includes types (enum, class, typedef), preprocessor
macros, namespaces, and functions and variables in namespaces or in
public or protected sections of a class declaration. For the private
interface and for the code itself, informal documentation is
sufficient. But it must exist. It is a good practice to comment first
and then code.

In Qt, documentation is placed into .cc files. This has the advantage
that documentation has less effect on compilation time compared to it
being placed into the header. The disadvantage is that inline
functions, typedefs, class declarations, templates etc. are not in
.cc. This either splits the documentation into many locations or
separates documentation from what is being documented.

In Into, interface documentation is always in the header file. The
problem with compilation overhead can be tackled with
[ccache](http://ccache.samba.org/). Documentation that is not directly
related to the public API, is placed in separate text files. You are
currently reading such documentation.

The style of documentation tags is that of JavaDoc. Formal comment
blocks begin with a slash (/) plus a double asterisk (**). Each line
within the block begins with an asterisk. The end of a block is marked
with a single asterisk plus a slash. Here's how:

~~~
/**
 * Function description.
 *
 * @param param_name description
 *
 * @return description
 *
 * @exception PiiException& on error
 */
~~~

Often, however, it is not necessary to formally document every
parameter and return value. If the purpose of a function can be
clearly told with a sentence or two, the full format just
complicates things. An example:

~~~
/**
 * Returns the square root of *x*.
 */
double sqrt(double x);
~~~

Use [markdown syntax](http://daringfireball.net/projects/markdown/syntax)
to format text in documentation comments.

Inputs and Outputs
------------------

In the documentation of a class derived from PiiOperation one
should use special documentation commands for inputs and outputs.
Here's how:

~~~
/**
 * An operation for reading images from files. The operation provides
 * an UI component for selecting the image files.
 *
 * Inputs
 * ------ 
 *
 * @in trigger - an optional trigger input. A new image is emitted
 * whenever any object is received in this input.
 *
 * @in filename - an optional filename input (QString). If this input
 * is connected, file names are read from it instead of the #fileNames
 * property.
 *
 * Outputs
 * -------
 *
 * @out image - the image output. Emits either four-channel color (the
 * default, PiiMatrix<PiiColor4<unsigned char> >) or grayscale images
 * in 8-bit (PiiMatrix<unsigned char>) channel format.
 *
 * @out filename - the file name of the current image (QString)
 */
class PII_IMAGE_EXPORT PiiImageFileReader : public PiiImageReaderOperation {};
~~~

Informal Documentation
----------------------

Whenever you think the the purpose of your code isn't obvious, write a
comment. The format of comments within the code is more or less
free. The only requirement is that it is written in English.

If you leave a feature unimplemented or there is something that is not
completely clear to you, mark it like this:

/* TODO
 * Explain what is missing or what needs to be checked.
 */

If the code works incorrectly, mark it like this:

/* FIXME
 * Bug description.
 */

In case a piece of code needs special care, works in an unconventional
way, or affects something else in a non-apparent way, let others know:

/* NOTE
 * Changing this will break that.
 */

If a solution to a problem requires techniques that won't stand
critical judgement, save your face:

/* HACK
 * A really good excuse for using duct tape for repair.
 */
