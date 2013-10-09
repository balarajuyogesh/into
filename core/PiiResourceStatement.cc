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

#include "PiiResourceStatement.h"


class PiiResourceStatement::Data : public PiiSharedD<Data>
{
public:
  Data() :
    bStringData(false),
    pSubject(0),
    pPredicate(0),
    pObject(0),
    type(Invalid),
    id(-1)
  {}

  Data(const Data& other) :
    bStringData(other.bStringData),
    type(other.type),
    id(other.id)
  {
    if (bStringData)
      {
        pSubject = new QString(*reinterpret_cast<QString*>(other.pSubject));
        pPredicate = new QString(*reinterpret_cast<QString*>(other.pPredicate));
        if (type == Pointer)
          pObject = other.pObject;
        else if (bStringData)
          pObject = new QString(*reinterpret_cast<QString*>(other.pObject));
      }
    else
      {
        pSubject = other.pSubject;
        pPredicate = other.pPredicate;
        pObject = other.pObject;
      }
  }
  
  Data(const char* s,
       const char* p,
       const char* o,
       Type t) :
    bStringData(false),
    pConstSubject(s),
    pConstPredicate(p),
    pConstObject(o),
    type(t),
    id(-1)
  {}
  
  Data(const char* s,
       const char* p,
       void* o) :
    bStringData(false),
    pConstSubject(s),
    pConstPredicate(p),
    pObject(o),
    type(Pointer),
    id(-1)
  {}
  
  Data(const QString& s,
       const QString& p,
       const QString& o,
       Type t) :
    bStringData(true),
    pSubject(new QString(s)),
    pPredicate(new QString(p)),
    pObject(new QString(o)),
    type(t),
    id(-1)
  {}
  
  Data(const QString& s,
       const QString& p,
       void* o) :
    bStringData(true),
    pSubject(new QString(s)),
    pPredicate(new QString(p)),
    pObject(o),
    type(Pointer),
    id(-1)
  {}
  
  ~Data()
  {
    if (bStringData)
      deleteStrings();
  }
  
  void deleteStrings()
  {
    delete reinterpret_cast<QString*>(pSubject);
    delete reinterpret_cast<QString*>(pPredicate);
    if (type != Pointer)
      delete reinterpret_cast<QString*>(pObject);
  }
  
  bool bStringData;
  union { void* pSubject; const void* pConstSubject; };
  union { void* pPredicate; const void* pConstPredicate; };
  union { void* pObject; const void* pConstObject; };
  Type type;
  int id;
};

PII_SHARED_D_FUNC_DEF(PiiResourceStatement)

PiiResourceStatement::PiiResourceStatement() : d(new Data) {}

PiiResourceStatement::PiiResourceStatement(const char* s,
                                           const char* p,
                                           const char* o,
                                           Type t) :
  d(new Data(s, p, o, t))
{}

PiiResourceStatement::PiiResourceStatement(const char* s,
                                           const char* p,
                                           void* o) :
  d(new Data(s, p, o))
{}

PiiResourceStatement::PiiResourceStatement(const QString& s,
                                           const QString& p,
                                           const QString& o,
                                           Type t) :
  d(new Data(s, p, o, t))
{}

PiiResourceStatement::PiiResourceStatement(const QString& s,
                                           const QString& p,
                                           void* o) :
  d(new Data(s, p, o))
{}

PiiResourceStatement::PiiResourceStatement(int s,
                                           const QString& p,
                                           const QString& o,
                                           Type t) :
  d(new Data(QString("#%1").arg(s), p, o, t))
{}

PiiResourceStatement::~PiiResourceStatement()
{
  d->release();
}

PiiResourceStatement::PiiResourceStatement(const PiiResourceStatement& other) :
  d(other.d->reserved())
{}

PiiResourceStatement& PiiResourceStatement::operator= (const PiiResourceStatement& other)
{
  other.d->assignTo(this->d);
  return *this;
}

bool PiiResourceStatement::isValid() const
{
  return !subject().isEmpty() && !object().isEmpty() && d->type != Invalid;
}

QString PiiResourceStatement::subject() const
{
  return d->bStringData ? *reinterpret_cast<QString*>(d->pSubject) : reinterpret_cast<char*>(d->pSubject);
}

QString PiiResourceStatement::predicate() const
{
  return d->bStringData ? *reinterpret_cast<QString*>(d->pPredicate) : reinterpret_cast<char*>(d->pPredicate);
}

QString PiiResourceStatement::object() const
{
  if (d->type == Pointer)
    return QString();
  return d->bStringData ? *reinterpret_cast<QString*>(d->pObject) : reinterpret_cast<char*>(d->pObject);
}

void* PiiResourceStatement::objectPtr() const
{
  return d->type == Pointer ? d->pObject : 0;
}

PiiResourceStatement::Type PiiResourceStatement::type() const
{
  return d->type;
}

int PiiResourceStatement::id() const
{
  return d->id;
}

void PiiResourceStatement::setId(int id)
{
  _d()->id = id;
}
