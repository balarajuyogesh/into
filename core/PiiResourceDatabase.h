/* This file is part of Into. 
 * Copyright (C) Intopii 2013.
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

#ifndef _PIIRESOURCEDATABASE_H
#define _PIIRESOURCEDATABASE_H

#include "PiiGlobal.h"
#include "PiiResourceStatement.h"
#include "PiiTypeTraits.h"
#include "PiiUtil.h"

#include <QStringList>

#include <functional>

/**
 * A database that stores statements about resources. 
 * PiiResourceDatabase is based upon the idea of making statements
 * about resources in the form of subject-predicate-object
 * expressions, just like in RDF (http://www.w3.org/RDF/). These
 * expressions are known as triples in RDF terminology, and the term
 * is adopted here as well. The subject denotes the resource, and the
 * predicate denotes traits or aspects of the resource and expresses a
 * relationship between the subject and the object. For example, one
 * way to represent the notion "This class was created at Intopii" in
 * RDF is as a triple: a subject denoting "PiiResourceDatabase", a
 * predicate denoting "creator", and an object denoting "Intopii". The
 * shorthand form for such a triplet could be something like
 * `(PiiResourceDatabase,creator,"Intopii")`.
 *
 * Unlike W3C's RDF, PiiResourceDatabase does not require that
 * resources and predicates are identified by URIs. Any string
 * containing no white spaces will work as long as it is unique in the
 * context of the database. In particular, class names are excellent
 * resource identifiers for already being unique due to symbol naming
 * restrictions. For predicates, namespaces may be used if needed. An
 * example of a statement with a namespace could be
 * `(PiiResourceDatabase,my:opinion,"Confusing")`, which
 * states that "In my opinion, PiiResourceDatabase is confusing." The
 * "my:" namespace avoids polluting the global namespace with my
 * custom predicates.
 *
 * Instances of PiiResourceStatement are used to store the triples in
 * PiiResourceDatabase. The object of each triple may be either a
 * reference to another resource or a string literal. In the examples
 * above, double quotes were used to illustrate that the object was a
 * literal. But a triplet can describe a relationship between two
 * resources, for example that "PiiResourceDatabase uses
 * PiiResourceStatement":
 * `(PiiResourceDatabase,uses,PiiResourceStatement)`.
 *
 * Statements in PiiResourceDatabase are also resources, and they
 * have automatically assigned ids. Therefore, it is possible to 
 * *reify* statements in RDF style. In RDF terminology, reification
 * means staments about statements. Resource ids of the form "[123]"
 * (a hash followed by an integer) are reserved for statements.
 *
 */
class PII_CORE_EXPORT PiiResourceDatabase
{
public:
  PiiResourceDatabase();
  ~PiiResourceDatabase();

  static PiiResourceStatement resource(const char* subject, const char* predicate, const char* object);
  static PiiResourceStatement literal(const char* subject, const char* predicate, const char* object);
  static PiiResourceStatement resource(const QString& subject, const QString& predicate, const QString& object);
  static PiiResourceStatement literal(const QString& subject, const QString& predicate, const QString& object);
  static PiiResourceStatement resource(const char* subject, const char* predicate, void* object);
  static PiiResourceStatement resource(const QString& subject, const QString& predicate, void* object);
  
  /**
   * Creates a resource statement that refers to another
   * statement. The subject will be converted to "#subject". The
   * following two statements are equivalent:
   *
   * ~~~(c++)
   * PiiResourceDatabase::resource("#123", "pii:connector", "MyConnector");
   * PiiResourceDatabase::resource(123, "pii:connector", "MyConnector");
   * ~~~
   */
  static PiiResourceStatement resource(int subject, const QString& predicate, const QString& object);
  static PiiResourceStatement literal(int subject, const QString& predicate, const QString& object);
  
  /**
   * Adds a statement to the database. Returns the id of the
   * statement.
   */
  int addStatement(const PiiResourceStatement& statement);

  /**
   * A statements to the database. This function adds the given 
   * `statements` to the database, starting from the first one. If the
   * subject of a statement is "#", the statement is treated as a
   * reification of the previous statement (a statement about
   * statement). The subject ("#") will be replaced with the resource
   * id of the previous statement. Note that all successive
   * reifications refer to the same (non-reified) statement.
   *
   * ~~~(c++)
   * PiiResourceDatabase db;
   * // In my opinion, PiiResourceDatabase is flexible
   * // The truth value of the previous statement is true.
   * db.addStatements(QList<PiiResourceStatement>() <<
   *                  PiiResourceStatement("PiiResourceDatabase", "my:opinion", "flexible") <<
   *                  PiiResourceStatement("#", "my:truth value", "true"));
   * ~~~
   */
  QList<int> addStatements(const QList<PiiResourceStatement>& statements);

  /**
   * Remove the statement with the given id.
   */
  void removeStatement(int id);
  /**
   * Remove all statements whose id is in the given list.
   */
  void removeStatements(const QList<int>& ids);

  /**
   * Returns all stored statements as a list.
   */
  QList<PiiResourceStatement> statements() const;

  /**
   * Returns the number of stored statements.
   */
  int statementCount() const;
  
  /**
   * Returns a list of statements matching the given *filter*.
   *
   * ~~~(c++)
   * QList<PiiResourceStatement> lstResult;
   * // Fetch all statements that specify a parent-child relationship
   * lstResult = db->select(Pii::predicate == "pii:parent");
   * // Fetch all statements about MyOperation
   * lstResult = db->select(Pii::subject == "MyOperation");
   * // Fetch all literal statements about MyOperation
   * lstResult = db->select(Pii::subject == "MyOperation");
   * // Fetch all statements in which MyOperation is either the subject or the object
   * lstResult = db->select(Pii::subject == "MyOperation" || Pii::object == "MyOperation);
   * // Get all statements that specify child resources of MyPlugin
   * lstResult = db->select(Pii::predicate == "pii:parent" && Pii::object == "MyPlugin");
   * // Same, but shorter
   * lstResult = db->select(Pii::attribute("pii:parent") == "MyPlugin");
   * ~~~
   *
   * In the examples, `Pii::subject`, `Pii::predicate` etc. are
   * global instances of *selector* *structures* Pii::Subject,
   * Pii::Predicate etc. One can equally well use Pii::Subject(),
   * Pii::Predicate() etc. in their place.
   */
  template <class Filter> QList<PiiResourceStatement> select(Filter filter) const;

  /**
   * Returns a list of subjects, predicates, or objects whose
   * containing statement matches *filter*. A selector is a function
   * object that picks one field of a statement. The type of the
   * returned list depends on the return value of the selector. 
   * Supported selectors are:
   *
   * - `subject` - the subject of a statement, QString
   *
   * - `predicate` - the predicate of a statement, QString
   *
   * - `object` - the object of a statement, QString
   *
   * - `attribute`("name") - named attribute of a statement, QString. 
   * The attribute selector is equal to a logical AND filter that
   * first matches the predicate to `name` and then compares the
   * object.
   *
   * - `statementId` - the id of a statement, `int`.
   *
   * - `resourceType` - the type of the object,
   * PiiResourceStatement::Type.
   *
   * ~~~(c++)
   * QList<QString> lstResult;
   * // Find all child resources of PiiImagePlugin
   * lstResult = db->select(Pii::subject, Pii::attribute("pii:parent") == "PiiImagePlugin");
   * // Find all cool resources
   * lstResult = db->select(Pii::subject, Pii::attribute("my:evaluation") == "cool");
   * // Find all cool operations
   * lstResult = db->select(Pii::subject, Pii::attribute("my:evaluation") == "cool") &&
   *             db->select(Pii::subject, Pii::attribute("pii:class") == "PiiOperation");
   * // Get the names of all attributes attached to MyOperation
   * lstResult = db->select(Pii::predicate, Pii::subject == "MyOperation");
   * // Find cool superclasses
   * lstResult = db->select(Pii::object, Pii::predicate == "pii:class") &&
   *             db->select(Pii::subject, Pii::attribute("my:evaluation") == "cool");
   * // List all operations not in MyPlugin.
   * // Note that this query will only consider operations that have
   * // the pii:class attribute; the attribute() selector discards all
   * // statements that do not have the specified attribute name as
   * // the predicate.
   * lstResult = db->select(Pii::subject, Pii::attribute("pii:parent") != "MyPlugin") &&
   *             db->select(Pii::subject, Pii::attribute("pii:class") == "PiiOperation");
   * ~~~
   *
   * Filters support all comparison operators and the logical AND and
   * OR operators. In addition, there are two fuctions to convert the
   * QString output of a selector to a number:
   *
   * - `resourceStringTo`<T>() - converts a string argument to a
   * numeric type denoted by T. Returns 0 if the string cannot be
   * converted. If the conversion fails, any subsequent comparison
   * will fail. That is, `resourceStringTo<int>("abc") == 0`
   * will be `false` even though the function will return 0.
   *
   * - `resourceidToInt`() - converts a resource id of the form [123]
   * to an integer. If the input string is not an id of a statement,
   * -1 will be returned, and any subsequent comparison will return 
   * `false`.
   *
   * ~~~(c++)
   * using namespace Pii;
   * // Find the names of relations between MyClass and YourClass
   * db->select(predicate, subject == "MyClass" && object == "YourClass");
   * // Find all resources whose my:score is more than 5 or whose
   * // your:score is at least 0.5.
   * db->select(subject,
   *            resourceStringTo<int>(attribute("my:score")) > 5 ||
   *            resourceStringTo<double>(attribute("your:score")) >= 0.5);
   * // Find the objects of all statements whose object is a literal,
   * // but neither "Foo" nor "Bar" if the predicate is "my:name".
   * db->select(subject,
   *            !(attribute("my:name") == "Foo" ||
   *              attribute("my:name") == "Bar") &&
   *              resourceType == PiiResourceStatement::LiteralType);
   * // Find the ids of reified statements as integers.
   * QList<int> = db->select(resourceIdToInt(subject),
   *                         resourceIdToInt(subject) != -1);
   * // Find the ids of the reification statements.
   * QList<int> = db->select(statementId,
   *                         resourceIdToInt(subject) != -1);
   * ~~~
   */
  template <class Selector, class Filter>
  QList<typename Selector::ValueType> select(Selector selector, Filter filter) const;

  /**
   * Returns the id of the first statement matching `filter`. If no
   * statement matches, -1 will be returned.
   */
  template <class Filter> int findFirst(Filter filter) const;

  /**
   * Dumps all statements in the database to debug output.
   */
  void dump() const;

private:
  class Data    
  {
  public:
    QList<PiiResourceStatement> lstStatements;
  } *d;

  int generateId();
  
  PII_DISABLE_COPY(PiiResourceDatabase);
};

template <class Filter> QList<PiiResourceStatement> PiiResourceDatabase::select(Filter filter) const
{
  QList<PiiResourceStatement> lstResult;
  for (int i=0; i<d->lstStatements.size(); ++i)
    if (filter(d->lstStatements[i]))
      lstResult.append(d->lstStatements[i]);
  return lstResult;
}

template <class Selector, class Filter>
QList<typename Selector::ValueType> PiiResourceDatabase::select(Selector selector, Filter filter) const
{
  typedef typename Selector::ValueType ValueType;
  QList<ValueType> lstResult;
  for (int i=0; i<d->lstStatements.size(); ++i)
    if (filter(d->lstStatements[i]))
      {
        ValueType selected = selector(d->lstStatements[i]);
        if (!lstResult.contains(selected))
          lstResult.append(selected);
      }
  return lstResult;
}

template <class Filter> int PiiResourceDatabase::findFirst(Filter filter) const
{
  for (int i=0; i<d->lstStatements.size(); ++i)
    if (filter(d->lstStatements[i]))
      return d->lstStatements[i].id();
  return -1;
}

/// @hide
namespace Pii
{
  // Forward declaration
  template <class Filter> struct NotFilter;
  
  // A base class for resource filters. Used with the CRT pattern to
  // provide a common superclass for all filter types. Filters are
  // function objects (unary predicates) that map a statement to a
  // boolean value.
  template <class Filter> struct ResourceFilterBase
  {
    Filter* self() { return static_cast<Filter*>(this); }
    const Filter* self() const { return static_cast<const Filter*>(this); }

    inline NotFilter<Filter> operator! () const;
  };

  // A function object that composes the results of two other filters
  // using a logical operator (BinaryPredicate).
  template <class BinaryPredicate, class Filter1, class Filter2>
  struct ComposeFilter : ResourceFilterBase<ComposeFilter<BinaryPredicate, Filter1, Filter2> >
  {
    ComposeFilter(const BinaryPredicate& p, const Filter1& f1, const Filter2& f2) :
      predicate(p), filter1(f1), filter2(f2)
    {}

    bool operator() (const PiiResourceStatement& statement) const
    {
      return predicate(filter1(statement), filter2(statement));
    }

    const BinaryPredicate& predicate;
    const Filter1& filter1;
    const Filter2& filter2;
  };

  // Create a compose filter
  template <class BinaryPredicate, class Filter1, class Filter2>
  ComposeFilter<Filter1, Filter2, BinaryPredicate> composeFilter(const BinaryPredicate& op,
                                                                 const Filter1& f1,
                                                                 const Filter2& f2)
  {
    return ComposeFilter<BinaryPredicate, Filter1, Filter2>(op, f1, f2);
  }

  // Create a logical AND filter.
  template <class Filter1, class Filter2>
  ComposeFilter<std::logical_and<bool>, Filter1, Filter2> andFilter(const Filter1& f1, const Filter2& f2)
  {
    return ComposeFilter<std::logical_and<bool>, Filter1, Filter2>(std::logical_and<bool>(), f1, f2);
  }

  // Create a logical OR filter.
  template <class Filter1, class Filter2>
  ComposeFilter<std::logical_or<bool>, Filter1, Filter2> orFilter(const Filter1& f1, const Filter2& f2)
  {
    return ComposeFilter<std::logical_or<bool>, Filter1, Filter2>(std::logical_or<bool>(), f1, f2);
  }

  // A filter that negates the output of another filter.
  template <class Filter> struct NotFilter : ResourceFilterBase<NotFilter<Filter> >
  {
    NotFilter(const Filter& f) : filter(f) {}

    bool operator() (const PiiResourceStatement& statement) const { return !filter(statement); }

    const Filter& filter;
  };

  template <class Filter> NotFilter<Filter> ResourceFilterBase<Filter>::operator! () const { return NotFilter<Filter>(*self()); }

  // A function object that compares a selected field of a stament to
  // a given value. The comparison operator is given as the
  // BinaryPredicate template parameter.
  template <class Selector, class BinaryPredicate>
  struct ResourceFilter : ResourceFilterBase<ResourceFilter<Selector, BinaryPredicate> >
  {
    typedef typename Selector::ValueType ValueType;
    typedef typename Selector::ConstReferenceType ConstReferenceType;
    
    ResourceFilter(const Selector& s, const BinaryPredicate& c, ConstReferenceType v) :
      select(s), compare(c), value(v)
    {}

    bool operator() (const PiiResourceStatement& statement) const
    {
      // Returns true if the selector's return value matches value
      // with the given comparator AND if the selector is still valid
      // after the selection.
      return compare(select(statement), value) && select;
    }

    const Selector& select;
    const BinaryPredicate& compare;
    ValueType value;
  };

  // Create a resource filter
  template <class Selector, class BinaryPredicate>
  ResourceFilter<Selector, BinaryPredicate> resourceFilter(const Selector& select,
                                                           const BinaryPredicate& compare,
                                                           typename Selector::ConstReferenceType value)
  {
    return ResourceFilter<Selector, BinaryPredicate>(select, compare, value);
  }

  // A filter that matches a value against a list of values
  template <class Selector, class BinaryPredicate> struct MatchListFilter :
  ResourceFilterBase<MatchListFilter<Selector, BinaryPredicate> >
  {
    typedef typename Selector::ValueType ValueType;
    
    MatchListFilter(const Selector& s, const BinaryPredicate& c, const QList<ValueType>& lst) :
      select(s), compare(c), lstValues(lst)
    {}

    bool operator() (const PiiResourceStatement& statement) const
    {
      ValueType value = select(statement);
      if (select)
        for (int i=0; i<lstValues.size(); ++i)
          if (compare(value, lstValues[i]))
            return true;
      return false;
    }

    const Selector& select;
    const BinaryPredicate& compare;
    const QList<ValueType>& lstValues;
  };

  // Create a MatchListFilter
  template <class Selector, class BinaryPredicate>
  MatchListFilter<Selector, BinaryPredicate> matchListFilter(const Selector& selector,
                                                             const BinaryPredicate& compare,
                                                             const QList<typename Selector::ValueType>& lst)
  {
    return MatchListFilter<Selector, BinaryPredicate>(selector, compare, lst);
  }
  
  // Base class for selectors. Selectors are function objects that
  // take one member out of a statement triplet. Selector comparison
  // operators are defined in this structure, and they all return an
  // instance of a ResourceFilterBase subclass. Again, this struct is
  // used with the CRT pattern.
  template <class Selector, class Type> struct SelectorBase
  {
    typedef Type ValueType;
    // Primitive types are used as such in function arguments, complex
    // types as const references.
    typedef typename Pii::IfClass<Pii::IsPrimitive<Type>, Type, const Type&>::Type ConstReferenceType;
  
    Selector* self() { return static_cast<Selector*>(this); }
    const Selector* self() const { return static_cast<const Selector*>(this); }

#define PII_SELECTORBASE_OP(OP, FUNCTIONAL) \
    ResourceFilter<Selector, std::FUNCTIONAL<ValueType> > operator OP (ConstReferenceType value) const \
    { \
      return resourceFilter(*self(), std::FUNCTIONAL<ValueType>(), value); \
    }

    PII_SELECTORBASE_OP(==, equal_to);
    PII_SELECTORBASE_OP(!=, not_equal_to);
    PII_SELECTORBASE_OP(>, greater);
    PII_SELECTORBASE_OP(<, less);
    PII_SELECTORBASE_OP(>=, greater_equal);
    PII_SELECTORBASE_OP(<=, less_equal);

#undef PII_SELECTORBASE_OP

    MatchListFilter<Selector, std::equal_to<ValueType> > operator== (const QList<ValueType>& lst) const
    {
      return matchListFilter(*self(), std::equal_to<ValueType>(), lst);
    }

    // Check if the selector is valid after selecting a statement. 
    // Default selector is always valid.
    operator bool () const { return true; }
  };

  struct Subject : SelectorBase<Subject, QString>
  {
    QString operator() (const PiiResourceStatement& statement) const { return statement.subject(); }
  };

  struct Predicate : SelectorBase<Predicate, QString>
  {
    QString operator() (const PiiResourceStatement& statement) const { return statement.predicate(); }
  };

  struct Object : SelectorBase<Object, QString>
  {
    QString operator() (const PiiResourceStatement& statement) const { return statement.object(); }
  };

  template <class T> struct ObjectPtr : SelectorBase<ObjectPtr<T>, T*>
  {
    T* operator() (const PiiResourceStatement& statement) const { return reinterpret_cast<T*>(statement.objectPtr()); }
  };

  // Special selector for attributes. The operator(QString) returns
  // another Attribute structure with its strPredicate member set to
  // the given predicate name.
  struct Attribute : SelectorBase<Attribute, QString>
  {
    Attribute(const QString& predicate) :
      strPredicate(predicate), bOk(true)
    {}
    
    Attribute operator() (const QString& predicate) const
    {
      return Attribute(predicate);
    }
    
    QString operator() (const PiiResourceStatement& statement) const
    {
      bOk = statement.predicate() == strPredicate;
      return statement.object();
    }
    operator bool () const { return bOk; }
    
    const QString& strPredicate;
    mutable bool bOk;
  };

  struct ResourceType : SelectorBase<ResourceType, PiiResourceStatement::Type>
  {
    PiiResourceStatement::Type operator() (const PiiResourceStatement& statement) const { return statement.type(); }
  };

  struct StatementId : SelectorBase<StatementId, int>
  {
    int operator() (const PiiResourceStatement& statement) const { return statement.id(); }
  };

  // A selector that converts the QString-typed output of another
  // selector to any other type.
  template <class T, class Selector> struct ResourceStringTo : SelectorBase<ResourceStringTo<T,Selector>, T>
  {
    ResourceStringTo(const Selector& s) : select(s), bOk(true) {}
    T operator() (const PiiResourceStatement& statement) const
    {
      return Pii::stringTo<T>(select(statement), &bOk);
    }
    operator bool () const { return select && bOk; }
    const Selector& select;
    mutable bool bOk;
  };

  // Create a ResourceStringTo selector
  template <class T, class Selector> ResourceStringTo<T,Selector> resourceStringTo(const Selector& select)
  {
    return ResourceStringTo<T,Selector>(select);
  }
  
  // A selector that converts a numeric resource id (e.g. #123) to an
  // int.
  template <class Selector> struct ResourceIdToInt : SelectorBase<ResourceIdToInt<Selector>, int>
  {
    ResourceIdToInt(const Selector& s) : select(s), bOk(true) {}
    int operator() (const PiiResourceStatement& statement) const
    {
      QString str = select(statement);
      if (str.size() < 2 || str[0] != '#')
        {
          bOk = false;
          return -1;
        }
      return str.mid(1).toInt(&bOk);
    }
    operator bool () const { return select && bOk; }
    const Selector& select;
    mutable bool bOk;
  };

  template <class Selector> ResourceIdToInt<Selector> resourceIdToInt(const Selector& select)
  {
    return ResourceIdToInt<Selector>(select);
  }

  template <class Filter1, class Filter2>
  ComposeFilter<std::logical_and<bool>, Filter1, Filter2> operator&& (const ResourceFilterBase<Filter1>& f1,
                                                                      const ResourceFilterBase<Filter2>& f2)
  {
    return andFilter(*f1.self(), *f2.self());
  }

  template <class Filter1, class Filter2>
  ComposeFilter<std::logical_or<bool>, Filter1, Filter2> operator|| (const ResourceFilterBase<Filter1>& f1,
                                                                     const ResourceFilterBase<Filter2>& f2)
  {
    return orFilter(*f1.self(), *f2.self());
  }

  extern PII_CORE_EXPORT Subject subject;
  extern PII_CORE_EXPORT Predicate predicate;
  extern PII_CORE_EXPORT Object object;
  extern PII_CORE_EXPORT Attribute attribute;
  extern PII_CORE_EXPORT ResourceType resourceType;
  extern PII_CORE_EXPORT StatementId statementId;
}
/// @endhide

#endif //_PIIRESOURCEDATABASE_H
