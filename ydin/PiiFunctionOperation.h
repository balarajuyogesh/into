/* This file is part of Into.
 * Copyright (C) Intopii 2014.
 * All rights reserved.
 *
 * Licensees holding a commercial Into license may use this file in
 * accordance with the commercial license agreement. Please see
 * LICENSE.commercial for commercial licensing terms.
 *
 * Alternatively, this file may be used under the terms of the GNU
 * Affero General Public License version 3 as published by the Free
 * Software Foundation. In addition, Intopii gives you special rights
 * to use Into as a part of open source software projects. Please
 * refer to LICENSE.AGPL3 for details.
 */

#ifndef _PIIFUNCTIONOPERATION_H
#define _PIIFUNCTIONOPERATION_H

#ifdef PII_CXX11

#include "PiiDefaultOperation.h"
#include <PiiFunctional.h>
#include <type_traits>
#include <functional>
#include <tuple>

#ifdef Q_MOC_RUN
// Moc fails to resolve the real type definition.
#  define PII_FUNCTION_OP_FOR(X) PiiDefaultOperation
#else
/**
 * Resolves a proper PiiFunctionOperation type for a function pointer
 * or function object. `FUNCTION` must be an expression that evaluates
 * to a callable entity (either a function pointer or a function
 * object instance).
 *
 * @code
 * int function(int);
 * struct Function
 * {
 *   int operator() (int) const;
 * };
 *
 * // PII_FUNCTION_OP_FOR(function) = PiiFunctionOperation<int(int), int>
 * // PII_FUNCTION_OP_FOR(Function()) = PiiFunctionOperation<Function, int>
 * @endcode
 *
 * @relates PiiFunctionOperation
 */
#  define PII_FUNCTION_OP_FOR(FUNCTION) Pii::Id<decltype(PiiFuncOpPrivate::resolveType(FUNCTION))>::Type
#endif

/// @hide
namespace PiiFuncOpPrivate
{
  template <class T> struct ParamTraits
  {
    typedef PiiInputSocket SocketType;
    typedef T ValueType;
    static T toParam(ValueType value) { return value; }
  };
  template <class T> struct ParamTraits<T*>
  {
    typedef PiiOutputSocket SocketType;
    typedef T ValueType;
    static T* toParam(ValueType& value) { return &value; }
  };
  template <class T> struct ParamTraits<T&>
  {
    typedef PiiOutputSocket SocketType;
    typedef T ValueType;
    static T& toParam(ValueType& value) { return value; }
  };
  template <class T> struct ParamTraits<const T*>
  {
    typedef PiiInputSocket SocketType;
    typedef T ValueType;
    static const T* toParam(ValueType& value) { return &value; }
  };
  template <class T> struct ParamTraits<const T&>
  {
    typedef PiiInputSocket SocketType;
    typedef T ValueType;
    static const T& toParam(const ValueType& value) { return value; }
  };

  // The holder type must be resolved based on function argument
  // types. At that phase, it is not possible to know whether a getter
  // function or an input socket is used to retrieve the value at run
  // time. Therefore, this struct separates the two cases at run time.
  // This costs one comparison, which would be needed anyway since we
  // need to support optional input sockets.
  template <class Object, class T> struct InputHolder
  {
    // Input parameters can be initialized by reading a socket...
    InputHolder(const char* socketName) :
      getter(0),
      pSocket(new PiiInputSocket(socketName)),
      bReadSocket(true)
    {}
    // ... or by calling a function. This version accepts any member
    // function pointer from a class derived from Object.
    template <class Derived> InputHolder(T (Derived::* getter)() const) :
      getter(static_cast<T (Object::*)() const>(getter)),
      pSocket(0),
      bReadSocket(false)
    {}

    void readValue(Object* obj, T& value)
    {
       if (bReadSocket)
        {
          if (pSocket->firstObject().type() != Pii::typeId<T>())
            PII_THROW_UNKNOWN_TYPE(pSocket);
          value = pSocket->firstObject().valueAs<T>();
        }
      else
        value = (obj->*getter)();
    }

    void emitValue(typename ParamTraits<T>::ValueType&) {}

    T (Object::* getter)() const;
    PiiInputSocket* pSocket;
    bool bReadSocket;
  };

  template <class Object, class T> struct OutputHolder
  {
    OutputHolder(const char* socketName) :
      pSocket(new PiiOutputSocket(socketName))
    {}

    void readValue(Object*, ...) {}
    void emitValue(typename ParamTraits<T>::ValueType& value)
    {
      pSocket->emitObject(value); // may throw
    }

    PiiOutputSocket* pSocket;
  };

  // ParamHolder::Type is either InputHolder or OutputHolder,
  // depending on the type of the function parameter. Should use a
  // template alias, but gcc doesn't support them until 4.7.
  template <class Object, class T>
  struct ParamHolder
  {
    typedef typename Pii::IfClass<Pii::IsSame<typename ParamTraits<T>::SocketType, PiiInputSocket>,
                                  InputHolder<Object, T>,
                                  OutputHolder<Object, T>>::Type Type;
  };

  struct ValueReader
  {
    template <class Object, class Holder, class T>
    void operator() (Object operation,
                     Holder& holder,
                     T& value)
    {
      holder.readValue(operation, value);
    }
  };

  struct ValueEmitter
  {
    template <class Holder, class T>
    void operator() (Holder& holder,
                     T& value)
    {
      holder.emitValue(value);
    }
  };

  struct SocketAdder
  {
    template <class Operation, class Holder>
    void operator() (Operation* op, Holder holder)
    {
      if (holder.pSocket)
        op->addSocket(holder.pSocket);
    }
  };

  struct DefaultValueSetter
  {
    template <class Object, class T>
    void operator() (InputHolder<Object,T>& holder,
                     const QString& name,
                     T (Object::* getter)() const) const
    {
      if (holder.pSocket->objectName() == name)
        {
          holder.getter = getter;
          holder.pSocket->setOptional(true);
        }
    }

    template <class Object, class T>
    void operator() (InputHolder<Object,T>& holder,
                     const QString& name,
                     nullptr_t) const
    {
      if (holder.pSocket->objectName() == name)
        {
          holder.getter = nullptr;
          holder.pSocket->setOptional(false);
        }
    }

    template <class... Args> void operator() (Args...) const {}
  };

  struct OptionalInputChecker
  {
    template <class Object, class T>
    void operator() (InputHolder<Object,T>& holder)
    {
      holder.bReadSocket = holder.pSocket && holder.pSocket->isConnected();
    }

    template <class... Args> void operator() (Args...) const {}
  };

}
/// @endhide

/**
 * A wrapper operation that makes it easy to make existing functions
 * runnable by [PiiEngine].
 *
 *
 * ~~~(c++)
 * PiiMatrix<uchar> detectEdges(PiiMatrix<uchar> image,
 *                              int threshold,
 *                              PiiMatrix<float>* gradient);
 *
 * class MyEdgeDetector : public PII_FUNCTION_OP_FOR(detectEdges)
 * {
 * public:
 *   typedef PII_FUNCTION_OP_FOR(detectEdges) SuperType;
 *   MyEdgeDetector();
 *   // ...
 * };
 *
 * MyEdgeDetector::MyEdgeDetector() :
 *   SuperType(&detectEdges,
 *             "edges",     // output for the return value
 *             "image",     // input for the first parameter
 *             "threshold", // input for the second parameter
 *             "gradient")  // output for the output-value parameter
 * {}
 * ~~~
 */
template <class Function, class... Args>
class PiiFunctionOperation : public PiiDefaultOperation
{
public:
  void check(bool reset)
  {
    PiiDefaultOperation::check(reset);

    Pii::callWithTuples(PiiFuncOpPrivate::OptionalInputChecker(),
                        _d()->holderPack);
  }
protected:
  /// @hide
  typedef PiiFunctionOperation<Function, Args...> ThisType;
  typedef Function FunctionType;

  template <class T> struct ParamHolder
  {
    typedef typename PiiFuncOpPrivate::ParamHolder<ThisType, T>::Type Type;
  };

  typedef std::tuple<typename ParamHolder<Args>::Type...> HolderPack;
  typedef std::tuple<typename PiiFuncOpPrivate::ParamTraits<Args>::ValueType...> ValuePack;
  typedef typename std::result_of<Function(Args...)>::type ReturnType;

  class NonVoidData : public PiiDefaultOperation::Data
  {
  public:
    NonVoidData(Function function,
                const char* outputName,
                typename ParamHolder<Args>::Type... holders) :
      function(function),
      pReturnOutput(new PiiOutputSocket(outputName)),
      holderPack{holders...}
    {}

    void addReturnOutput(ThisType* op) const { op->addSocket(pReturnOutput); }

    void callAndEmit(typename PiiFuncOpPrivate::ParamTraits<Args>::ValueType&... values) const
    {
      pReturnOutput->emitObject(function(PiiFuncOpPrivate::ParamTraits<Args>::toParam(values)...));
    }

    Function function;
    PiiOutputSocket* pReturnOutput;
    HolderPack holderPack;
  };

  class VoidData : public PiiDefaultOperation::Data
  {
  public:
    VoidData(Function function,
             typename ParamHolder<Args>::Type... holders) :
      function(function),
      holderPack{holders...}
    {}

    void addReturnOutput(ThisType*) const {}

    void callAndEmit(typename PiiFuncOpPrivate::ParamTraits<Args>::ValueType&... values) const
    {
      function(PiiFuncOpPrivate::ParamTraits<Args>::toParam(values)...);
    }

    Function function;
    HolderPack holderPack;
  };
  typedef typename Pii::IfClass<Pii::IsVoid<ReturnType>, VoidData, NonVoidData>::Type Data;
  PII_D_FUNC;
  /// @endhide

  template <class... Params>
  PiiFunctionOperation(Function function,
                       Params&&... params) :
    PiiDefaultOperation(new Data(function, std::forward<Params>(params)...))
  {
    PII_D;
    d->addReturnOutput(this);

    Pii::callWithTuples(PiiFuncOpPrivate::SocketAdder(),
                        Pii::makeTuple<sizeof...(Args)>(this),
                        d->holderPack);
  }

  /**
   * Sets a default value for the given *input*. If the input is not
   * connected, and the given *getter* function is not `nullptr`, the
   * value returned by *getter* will be used instead of the input
   * socket. Usually, *getter* is the READ accessor of a property with
   * the same name as the input.
   *
   * Setting the default to a non-null value automatically marks the
   * socket optional. If *getter* is `nullptr`, the socket will be
   * made non-optional.
   *
   * ~~~(c++)
   * setDefaultValue("threshold", // the name of an optional input
   *                 &MyOperation::threshold); // pointer to member function
   *
   * setDefaultValue("threshold", nullptr); // removes default value
   * ~~~
   *
   * ! The return type of *getter* must match the corresponding
   *   function parameter type. Otherwise, the function call does
   *   nothing. If the function takes in a const reference or pointer,
   *   *getter* must return the corresponding value type.
   */
  template <class T, class Object>
  void setDefaultValue(const QString& input, T (Object::* getter)() const)
  {
    Pii::callWithTuples(PiiFuncOpPrivate::DefaultValueSetter(),
                        _d()->holderPack,
                        Pii::makeTuple<sizeof...(Args)>(input),
                        Pii::makeTuple<sizeof...(Args)>(static_cast<T (ThisType::*)() const>(getter)));
  }

  void setDefaultValue(const QString& input, nullptr_t)
  {
    Pii::callWithTuples(PiiFuncOpPrivate::DefaultValueSetter(),
                        _d()->holderPack,
                        Pii::makeTuple<sizeof...(Args)>(input),
                        Pii::makeTuple<sizeof...(Args)>(nullptr));
  }

  void process()
  {
    PII_D;
    // Store function call parameters in the stack
    ValuePack values;
    // Initialize values prior to the call. This fills the values by
    // reading input sockets or calling property getters.
    Pii::callWithTuples(PiiFuncOpPrivate::ValueReader(),
                        Pii::makeTuple<sizeof...(Args)>(this),
                        d->holderPack,
                        values);

    // Call the actual function and emit its return value (if any).
    // TODO: const handling
    Pii::callWithTuple(Pii::memberFunction(d, &Data::callAndEmit), values);

    // Emit all other output arguments.
    Pii::callWithTuples(PiiFuncOpPrivate::ValueEmitter(),
                        d->holderPack,
                        values);
  }

private:
  friend class PiiFuncOpPrivate::SocketAdder;
};

namespace PiiFuncOpPrivate
{
  // Template functions that resolve a suitable PiiFunctionOperation
  // type given a function pointer or a function object.

  // Resolves a function pointer. Uses the function type itself as
  // "Function" to PiiFunctionOperation.
  template <class ReturnType, class... Args>
  PiiFunctionOperation<ReturnType (*)(Args...), Args...> resolveType(ReturnType(Args...));

  // Resolves a member function pointer. Uses the object type as
  // "Function". Not used directly but just as a helper for the
  // callable object resolver below.
  template <class ReturnType, class T, class... Args>
  PiiFunctionOperation<T, Args...> resolveType(ReturnType (T::*)(Args...));

  // Resolves any other callable object type. Uses the object type as
  // "Function".
  template <class T>
  decltype(resolveType(&T::operator())) resolveType(T);
}

#endif

#endif //_PIIFUNCTIONOPERATION_H
