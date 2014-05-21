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
#include <PiiMetaTemplate.h>
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

#define PII_FUNCOP_SET_CONVERTER(TYPE,CONVERTER) \
  namespace PiiFuncOpPrivate                     \
  {                                              \
    template <> struct Converter<TYPE>           \
    {                                            \
      typedef CONVERTER Type;                    \
    };                                           \
  }

#define PII_FUNCOP_SET_RETURN_CONVERTER(TYPE,CONVERTER) \
  namespace PiiFuncOpPrivate                     \
  {                                              \
    template <> struct ReturnConverter<TYPE>     \
    {                                            \
      typedef CONVERTER Type;                    \
    };                                           \
  }

/// @hide
namespace PiiFuncOpPrivate
{
  template <class T> struct DefaultInputConverter
  {
    typedef T ValueType;
    static bool initialize(const PiiVariant& var, T& val) { return var.convertTo(val); }
    static T toParam(T& val) { return val; }
  };

  template <class T> struct DefaultOutputConverter
  {
    typedef T ValueType;
    static bool initialize(T&) { return true; }
    static PiiVariant toVariant(const T& val) { return PiiVariant(val); }
  };

  template <class T> struct DefaultConverter : DefaultInputConverter<T> {};
  template <class T> struct DefaultConverter<T*> : DefaultOutputConverter<T>
  {
    static T* toParam(T& value) { return &value; }
  };
  template <class T> struct DefaultConverter<T&> : DefaultOutputConverter<T>
  {
    static T& toParam(T& value) { return value; }
  };
  template <class T> struct DefaultConverter<const T*> : DefaultInputConverter<T>
  {
    static const T* toParam(const T& value) { return &value; }
  };
  template <class T> struct DefaultConverter<const T&> : DefaultInputConverter<T>
  {
    static const T& toParam(const T& value) { return value; }
  };
  template <class T> struct DefaultReturnConverter : DefaultConverter<T*> {};

  // Traits
  template <class T> struct Converter { typedef DefaultConverter<T> Type; };
  template <class T> struct ReturnConverter { typedef DefaultReturnConverter<T> Type; };
  template <class T> struct IsInput : Pii::True {};
  template <class T> struct IsInput<T*> : Pii::False {};
  template <class T> struct IsInput<T&> : Pii::False {};
  template <class T> struct IsInput<const T*> : Pii::True {};
  template <class T> struct IsInput<const T&> : Pii::True {};

  // The holder type must be resolved based on function argument
  // types. At that phase, it is not possible to know whether a getter
  // function or an input socket is used to retrieve the value at run
  // time. Therefore, this struct uses a function pointer to select
  // the appropriate initializer.
  template <class Object, class T> struct InputHolder
  {
    typedef typename Converter<T>::Type Conv;
    typedef typename Conv::ValueType ValueType;

    struct CastWrapper
    {
      virtual ~CastWrapper() {}
      virtual void init(Object*, ValueType& value) const = 0;
    };
    template <class U> struct CastWrapperImpl : CastWrapper
    {
      CastWrapperImpl(U (Object::* getter)() const) : getter(getter) {}
      void init(Object* obj, ValueType& value) const
      {
        value = static_cast<ValueType>((obj->*getter)());
      }
      U (Object::* getter)() const;
    };


    InputHolder(InputHolder&& other) :
      pSocket(other.pSocket),
      getter(other.getter),
      pWrapper(other.pWrapper),
      initializer(other.initializer)
    {
      other.pWrapper = nullptr;
    }
    // Input parameters can be initialized by reading a socket...
    InputHolder(const char* socketName) :
      pSocket(new PiiInputSocket(socketName)),
      getter(nullptr),
      pWrapper(nullptr),
      initializer(&InputHolder::initFromSocket)
    {}
    // ... or by calling a function. This version accepts any member
    // function pointer from a class derived from Object.
    template <class Derived>
    InputHolder(ValueType (Derived::* getter)() const,
                const char* socketName = nullptr) :
      pSocket(socketName ? new PiiInputSocket(socketName) : nullptr),
      getter(static_cast<ValueType (Object::*)() const>(getter)),
      pWrapper(nullptr),
      initializer(&InputHolder::initByGetter)
    {
      if (pSocket)
        pSocket->setOptional(true);
    }
    // Finally, this version accepts any getter whose return value is
    // convertible to the target type.
    template <class Derived, class U>
    InputHolder(U (Derived::* getter)() const, const char* socketName = nullptr) :
      pSocket(socketName ? new PiiInputSocket(socketName) : nullptr),
      getter(nullptr),
      pWrapper(new CastWrapperImpl<U>(static_cast<U (Object::*)() const>(getter))),
      initializer(&InputHolder::initByCastedGetter)
    {
      if (pSocket)
        pSocket->setOptional(true);
    }

    ~InputHolder()
    {
      delete pWrapper;
    }

    template <class Derived>
    void setGetter(ValueType (Derived::* get)() const)
    {
      delete pWrapper;
      pWrapper = 0;
      getter = static_cast<ValueType (Object::*)() const>(get);
    }

    template <class Derived, class U>
    void setGetter(U (Derived::* get)() const)
    {
      getter = 0;
      delete pWrapper;
      pWrapper = new CastWrapperImpl<U>(get);
    }

    void setGetter(std::nullptr_t)
    {
      getter = 0;
      delete pWrapper;
      pWrapper = 0;
    }

    void initialize(Object* obj, ValueType& value) const
    {
      (this->*initializer)(obj, value);
    }

    void initFromSocket(Object*, ValueType& value) const
    {
      if (!Conv::initialize(pSocket->firstObject(), value))
        PII_THROW_UNKNOWN_TYPE(pSocket);
    }

    void initByGetter(Object* obj, ValueType& value) const
    {
      value = (obj->*getter)();
    }

    void initByCastedGetter(Object* obj, ValueType& value) const
    {
      pWrapper->init(obj, value);
    }

    void check()
    {
      if (pSocket && pSocket->isConnected())
        initializer = &InputHolder::initFromSocket;
      else if (getter)
        initializer = &InputHolder::initByGetter;
      else if (pWrapper)
        initializer = &InputHolder::initByCastedGetter;
      else
        PII_THROW(PiiExecutionException, QCoreApplication::translate("PiiFunctionOperation", "Input parameter has no data source."));
    }

    PiiInputSocket* pSocket;
    ValueType (Object::* getter)() const;
    CastWrapper* pWrapper;
    void (InputHolder::* initializer)(Object*, ValueType&) const;
  };

  template <class Object, class T> struct OutputHolder
  {
    typedef typename Converter<T>::Type Conv;
    typedef typename Conv::ValueType ValueType;

    OutputHolder(const char* socketName) :
      pSocket(new PiiOutputSocket(socketName))
    {}

    void initialize(Object*, ValueType& value) { Conv::initialize(value); }
    void emitValue(ValueType& value)
    {
      pSocket->emitObject(Conv::toVariant(value)); // may throw
    }

    PiiOutputSocket* pSocket;
  };

  // ParamHolder::Type is either InputHolder or OutputHolder,
  // depending on the type of the function parameter. Should use a
  // template alias, but gcc doesn't support them until 4.7 or so.
  template <class Object, class T>
  struct ParamHolder
  {
    typedef typename Pii::IfClass<IsInput<T>,
                                  InputHolder<Object, T>,
                                  OutputHolder<Object, T>>::Type Type;
  };

  struct Initializer
  {
    template <class Object, class Holder, class T>
    void operator() (Object operation,
                     Holder& holder,
                     T& value)
    {
      holder.initialize(operation, value);
    }
  };

  struct ValueEmitter
  {
    template <class Object, class T, class U>
    void operator() (OutputHolder<Object,T>& holder, U& value)
    {
      holder.emitValue(value);
    }
    template <class... Args> void operator() (Args&...) const {}
  };

  struct SocketAdder
  {
    template <class Operation, class Holder>
    void operator() (Operation* op, Holder& holder)
    {
      if (holder.pSocket)
        op->addSocket(holder.pSocket);
    }
  };

  struct DefaultValueSetter
  {
    template <class Object, class T, class U>
    void operator() (InputHolder<Object,T>& holder,
                     const QString& name,
                     U (Object::* getter)() const) const
    {
      if (holder.pSocket && holder.pSocket->objectName() == name)
        {
          holder.setGetter(getter);
          holder.pSocket->setOptional(true);
        }
    }

    template <class Object, class T>
    void operator() (InputHolder<Object,T>& holder,
                     const QString& name,
                     std::nullptr_t) const
    {
      if (holder.pSocket && holder.pSocket->objectName() == name)
        {
          holder.setGetter(nullptr);
          holder.pSocket->setOptional(false);
        }
    }

    template <class... Args> void operator() (Args&...) const {}
  };

  struct Checker
  {
    template <class Object, class T>
    void operator() (InputHolder<Object,T>& holder)
    {
      holder.check();
    }

    template <class... Args> void operator() (Args&...) const {}
  };

  template <class Function> struct FunctionCaller
  {
    FunctionCaller(Function f) : f(f) {}
    Function f;

    template <class Object, class T>
    void operator() (InputHolder<Object,T>& holder)
    {
      if (holder.pSocket)
        f(holder.pSocket,
          static_cast<T*>(nullptr),
          static_cast<typename Converter<T>::Type::ValueType*>(nullptr));
    }

    template <class Object, class T>
    void operator() (OutputHolder<Object,T>& holder)
    {
      f(holder.pSocket,
        static_cast<T*>(nullptr),
        static_cast<typename Converter<T>::Type::ValueType*>(nullptr));
    }
  };

  template <class T> struct ValuePack : T
  {
    template <std::size_t I>
    typename std::tuple_element<I,T>::type& at() { return std::get<I>(*this); }
    template <std::size_t I>
    const typename std::tuple_element<I,T>::type& at() const { return std::get<I>(*this); }
  };

  // func returns void
  template <class Function, class Args> bool callCustomInit(Function func, Args&& args,
                                                            typename std::enable_if<
                                                              std::is_same<typename std::result_of<Function(Args)>::type,
                                                                           void>::value, int>::type = 0)
  {
    func(std::forward<Args>(args));
    return true;
  }

  // func returns bool
  template <class Function, class Args> bool callCustomInit(Function func, Args&& args,
                                                            typename std::enable_if<
                                                              std::is_same<typename std::result_of<Function(Args)>::type,
                                                                           bool>::value, int>::type = 0)
  {
    return func(std::forward<Args>(args));
  }
}
/// @endhide

/**
 * A wrapper operation that makes it easy to make existing functions
 * runnable by [PiiEngine].
 *
 * ~~~(c++)
 * PiiMatrix<uchar> detectEdges(const PiiMatrix<uchar>& image,
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
 *
 * Each parameter type is associated with an *converter* that is
 * responsible for creating a temporary object on the stack and for
 * performing the required conversions from the temporary object to
 * parameter type and PiiVariant. The converter performs slightly
 * different tasks for input and output parameters. Every type except
 * non-const pointers and non-const references is considered an input
 * parameter.
 *
 * The process of calling a function is as follows:
 *
 * 1.  A temporary variable (`converter::ValueType`) is created for
 *     each input parameter on the stack. The default constructor is
 *     used.
 *
 *     Note that the type of the temporary variable may or may not be
 *     the same as the actual parameter type. The default converter
 *     uses a default-constructed value for output parameters and
 *     initializes input parameters with the value read from a socket
 *     or returned by a getter function. For pointers and references,
 *     the corresponding value type is used.
 *
 *      In the example above, the `gradient` parameter would point to
 *      a default-constructed [PiiMatrix<float>] on the stack.
 *      Similarly, `image` would refer to a [PiiMatrix<uchar>] whose
 *      value would be initialized with data read from an input
 *      socket.
 *
 * 2.  The temporary variables are initialized using
 *     `converter::initialize()`. The default converters do nothing
 *     for output parameters. The value for an input parameter is
 *     copied (and converted, if needed) from an input socket or a
 *     getter function.
 *
 * 3.  The temporary variables are passed to the actual function
 *     through `converter::toParam()`.
 *
 * 4.  When the function returns, all output parameters are converted
 *     to [PiiVariant]s using `converter::toVariant()` and passed to
 *     the corresponding output socket.
 *
 * ~~~(c++)
 * struct MyType
 * {
 *   int number;
 * };
 *
 * struct MyInputConverter
 * {
 *   typedef const MyType* ParamType; // Not actually required
 *   typedef PiiSharedPtr<MyType> ValueType; // Required
 *
 *   // Converts a variant read from an input socket to the
 *   // temporary type.
 *   static bool initialize(const PiiVariant& variant, ValueType& value)
 *   {
 *     bool bOk = false;
 *     value = new MyType{variant.convertTo<int>(&bOk)};
 *     return bOk;
 *   }
 *   // Converts the temporary value to the actual parameter
 *   // type so that it can be used in the function call.
 *   static ParamType toParam(ValueType& value)
 *   {
 *     return value; // PiiSharedPtr automatically casts
 *   }
 * };
 *
 * struct MyOutputConverter
 * {
 *   typedef MyType* ParamType;
 *   typedef PiiSharedPtr<MyType> ValueType;
 *
 *   static bool initialize(ValueType& value)
 *   {
 *     value = new MyType{42};
 *   }
 *   static ParamType toParam(ValueType& value)
 *   {
 *     return value;
 *   }
 *   static PiiVariant toVariant(const ValueType& value)
 *   {
 *     return PiiVariant(value->number);
 *   }
 * };
 *
 * PII_FUNCOP_SET_CONVERTER(const MyType*, MyInputConverter);
 * PII_FUNCOP_SET_CONVERTER(MyType*, MyOutputConverter);
 * ~~~
 */
template <class Function, class... Args>
class PiiFunctionOperation : public PiiDefaultOperation
{
public:
  void check(bool reset)
  {
    PiiDefaultOperation::check(reset);

    Pii::callWithTuples(PiiFuncOpPrivate::Checker(),
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

  template <class T> struct Converter
  {
    typedef typename PiiFuncOpPrivate::Converter<T>::Type Type;
    typedef typename Type::ValueType ValueType;
  };

  typedef std::tuple<typename ParamHolder<Args>::Type...> HolderPack;
  typedef PiiFuncOpPrivate::ValuePack<std::tuple<typename Converter<Args>::ValueType...>> ValuePack;
  typedef typename std::result_of<Function(Args...)>::type ReturnType;

  class VoidData : public PiiDefaultOperation::Data
  {
  public:
    VoidData(Function function,
             typename ParamHolder<Args>::Type&&... holders) :
      function(function),
      holderPack{std::forward<typename ParamHolder<Args>::Type>(holders)...}
    {}

    void addReturnOutput(ThisType*) const {}

    void callAndEmit(typename Converter<Args>::ValueType&... values) const
    {
      function(Converter<Args>::Type::toParam(values)...);
    }

    template <class BinaryFunction>
    void forEachSocket(BinaryFunction function)
    {
      Pii::callWithTuples(PiiFuncOpPrivate::FunctionCaller<BinaryFunction>(function),
                          holderPack);
    }

    template <std::size_t I>
    decltype(std::declval<typename std::tuple_element<I, HolderPack>::type>().pSocket)
      socketAt() const
    {
      return std::get<I>(holderPack).pSocket;
    }

    Function function;
    HolderPack holderPack;
  };

  class NonVoidData : public VoidData
  {
  public:
    NonVoidData(Function function,
                const char* outputName,
                typename ParamHolder<Args>::Type&&... holders) :
      VoidData(function, std::forward<typename ParamHolder<Args>::Type>(holders)...),
      pReturnOutput(new PiiOutputSocket(outputName))
    {}

    void addReturnOutput(ThisType* op) const { op->addSocket(pReturnOutput); }

    void callAndEmit(typename Converter<Args>::ValueType&... values) const
    {
      pReturnOutput->emitObject(PiiFuncOpPrivate::ReturnConverter<ReturnType>::Type::
                                toVariant(this->function(Converter<Args>::Type::toParam(values)...)));
    }

    template <class BinaryFunction>
    void forEachSocket(BinaryFunction function)
    {
      function(pReturnOutput, static_cast<ReturnType*>(nullptr), static_cast<ReturnType*>(nullptr));
      VoidData::forEachSocket(function);
    }

    template <std::size_t I> struct GetParam
    {
      static decltype(std::declval<VoidData>().template socketAt<I>()) call(const VoidData* d) { return d->socketAt<I>(); }
    };
    struct GetReturn
    {
      static PiiOutputSocket* call(const NonVoidData* d) { return d->pReturnOutput; }
    };

    template <std::size_t I>
    decltype(Pii::If<I != 0, GetParam<I-1>, GetReturn>::Type::call(std::declval<const NonVoidData*>()))
      socketAt() const
    {
      return Pii::If<I != 0, GetParam<I-1>, GetReturn>::Type::call(this);
    }

    PiiOutputSocket* pReturnOutput;
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

    Pii::callWithTuples(std::bind(PiiFuncOpPrivate::SocketAdder(),
                                  this, std::placeholders::_1),
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
    /* TODO bind() doesn't work here. Why?
    using namespace std::placeholders;
    Pii::callWithTuples(std::bind(PiiFuncOpPrivate::DefaultValueSetter(), _1,
                                  input,
                                  static_cast<T (ThisType::*)() const>(getter)),
                        _d()->holderPack);
    */
    Pii::callWithTuples(PiiFuncOpPrivate::DefaultValueSetter(),
                        _d()->holderPack,
                        Pii::makeTuple<sizeof...(Args)>(input),
                        Pii::makeTuple<sizeof...(Args)>(static_cast<T (ThisType::*)() const>(getter)));
  }

  void setDefaultValue(const QString& input, std::nullptr_t)
  {
    Pii::callWithTuples(PiiFuncOpPrivate::DefaultValueSetter(),
                        _d()->holderPack,
                        Pii::makeTuple<sizeof...(Args)>(input),
                        Pii::makeTuple<sizeof...(Args)>(nullptr));
  }

  /**
   * Calls *function* for each auto-generated socket.
   *
   * @param function a function object that takes three parameters: a
   * pointer to a socket, a null pointer to an object that has the
   * same type as the corresponding input parameter and a null pointer
   * to an object that has the same type as the stack-allocated
   * temporary object.
   *
   * ~~~(c++)
   * struct Func
   * {
   *   template <class Actual, class Temporary>
   *   void operator() (PiiSocket* socket, Actual*, Temporary*)
   *   {
   *     // ...
   *   }
   * };
   *
   * forEachSocket(Func());
   * ~~~
   *
   * It is possible to create overloaded versions of `operator()` in
   * the function object. For example, there could be a different
   * function function for [PiiInputSocket] and [PiiOutputSocket].
   *
   * If the function wrapped by PiiFunctionOperation has a return
   * value, it is handled specially. Since there is no temporary
   * value, `Actual` and `Temporary` will be the same type.
   */
  template <class BinaryFunction>
  void forEachSocket(BinaryFunction function)
  {
    _d()->forEachSocket(function);
  }

  template <class Derived, class T>
  typename ParamHolder<T>::Type optional(const char* name,
                                         T (Derived::* getter)() const) const
  {
    return typename ParamHolder<T>::Type(getter, name);
  }

  template <class T, class Derived, class U>
  typename ParamHolder<T>::Type optional(const char* name,
                                         U (Derived::* getter)() const) const
  {
    return typename ParamHolder<T>::Type(getter, name);
  }

  template <class T, class Derived, class U>
  typename ParamHolder<T>::Type fixed(U (Derived::* getter)() const) const
  {
    return typename ParamHolder<T>::Type(getter);
  }

  /**
   * Returns the socket corresponding to the Ith function parameter.
   * If the function has a return value, it is regarded as the first
   * parameter. Depending on the type of the parameter, the returned
   * value is either [PiiInputSocket]* or [PiiOutputSocket]*. The
   * important difference between this function and e.g.
   * [PiiBasicOperation::inputAt()] is that this function is evaluated
   * at compile time and can be inlined.
   *
   * ~~~(c++)
   * void MyDerivedOperation::func()
   * {
   *   // If the wrapped function has a return value, this returns the
   *   // corresponding output socket.
   *   if (!socketAt<0>()->isConnected())
   *     qDebug("Output not connected!");
   * }
   * ~~~
   */
  template <std::size_t I>
  decltype(std::declval<Data>().template socketAt<I>()) socketAt() const
  {
    return _d()->socketAt<I>();
  }

  /**
   * Calls the wrapped function.
   */
  void process()
  {
    process(&ThisType::nullInit);
  }

  /**
   * Calls the wrapped function, but passes the stack-allocated
   * function call arguments to *customInit* before the call.
   *
   * @param customInit any callable object that takes a `ValuePack&`
   * as its only argument. The value pack contains all function
   * parameters (but not the return value) in declaration order.
   * Parameters can be accessed using the at() function as shown
   * below.
   *
   * ~~~(c++)
   * void MyDerivedOperation::init(ValuePack& args) // static member
   * {
   *   args.at<0>() = 1; // modify first argument
   * }
   *
   * void MyDerivedOperation::process()
   * {
   *   SuperType::process(&MyDerivedOperation::init);
   * }
   * ~~~
   *
   * You can use [Pii::MemberFunction] to make a non-static member
   * function callable.
   *
   * If the *customInit* function returns a `bool`, the wrapped
   * function is called only if the return value is `true`. This makes
   * it possible to do special processing based on input.
   */
  template <class InitFunc> void process(InitFunc customInit)
  {
    PII_D;
    // Store function call parameters in the stack and initialize them
    // prior to the call. This fills the values by reading input
    // sockets or calling property getters. We cannot use an uniform
    // initialization because any of the conversions can throw an
    // exception, which would leave the value pack into an
    // inconsistent state and potentially leak memory.
    ValuePack values;
    using namespace std::placeholders;
    Pii::callWithTuples(std::bind(PiiFuncOpPrivate::Initializer(), this, _1, _2),
                        d->holderPack,
                        values);

    if (PiiFuncOpPrivate::callCustomInit(customInit, values))
      {
        // Call the actual function and emit its return value (if any).
        // TODO: const handling
        Pii::callWithTuple(Pii::memberFunction(d, &Data::callAndEmit), values);

        // Emit all other output arguments.
        Pii::callWithTuples(PiiFuncOpPrivate::ValueEmitter(),
                            d->holderPack,
                            values);
      }
  }

private:
  friend class PiiFuncOpPrivate::SocketAdder;

  static void nullInit(ValuePack&) {}
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
