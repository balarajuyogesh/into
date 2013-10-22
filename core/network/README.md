Networking
==========

The networking module contains classes for building client/server
applications.

The most important feature in this module is the remote object
framework implemented by PiiObjectServer and PiiRemoteObject. It
provides a simple and easy-to-use HTTP-based alternative to QtDBus.
The framework doesn't try to be universal and support any programming
language (although JavaScript works). That said, the HTTP-based
protocol is easy to implement and provides a natural interface to web
applications, without the need for external daemons/services.
Furthermore, the "native" Qt implementation has the advantage that no
stub compilers or adaptors are needed.

The networking module also provides PiiNetworkServer, a generic
multi-threaded server for network applications. It is used to
implement [a multi-threaded web server](PiiHttpServer). HTTP clients
and servers can be implemented easily with the aid of
PiiHttpDevice. PiiMultipartDecoder makes it easy to parse multi-part
MIME messages such as form submissions.
