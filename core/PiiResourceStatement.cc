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



PiiResourceStatement::Data::Data() :
  bStringData(false),
  pSubject(0),
  pPredicate(0),
  pObject(0),
  type(InvalidType),
  id(-1)
{
}

PiiResourceStatement::Data::Data(const char* s,
                                 const char* p,
                                 const char* o,
                                 Type t,
                                 int i) :
  bStringData(false),
  pConstSubject(s),
  pConstPredicate(p),
  pConstObject(o),
  type(t),
  id(i)
{
}

PiiResourceStatement::Data::Data(const QString& s,
                                 const QString& p,
                                 const QString& o,
                                 Type t,
                                 int i) :
  bStringData(true),
  pSubject(new QString(s)),
  pPredicate(new QString(p)),
  pObject(new QString(o)),
  type(t),
  id(i)
{
}

PiiResourceStatement::Data::~Data()
{
  if (bStringData)
    deleteStrings();
}

void PiiResourceStatement::Data::deleteStrings()
{
  delete reinterpret_cast<QString*>(pSubject);
  delete reinterpret_cast<QString*>(pPredicate);
  delete reinterpret_cast<QString*>(pObject);
}

PiiResourceStatement::PiiResourceStatement(const char* s,
                                           const char* p,
                                           const char* o,
                                           Type t,
                                           int i) :
  d(new Data(s, p, o, t, i))
{
}

PiiResourceStatement::PiiResourceStatement(const QString& s,
                                           const QString& p,
                                           const QString& o,
                                           Type t,
                                           int i) :
  d(new Data(s, p, o, t, i))
{
}

PiiResourceStatement::PiiResourceStatement(int s,
                                           const QString& p,
                                           const QString& o,
                                           Type t,
                                           int i) :
  d(new Data(QString("#%1").arg(s), p, o, t, i))
{
}

PiiResourceStatement::~PiiResourceStatement()
{
  delete d;
}

PiiResourceStatement::PiiResourceStatement(const PiiResourceStatement& other) :
  d(other.d->bStringData ?
    new Data(other.subject(), other.predicate(), other.object(), other.type(), other.id()) :
    new Data(reinterpret_cast<char*>(other.d->pSubject),
             reinterpret_cast<char*>(other.d->pPredicate),
             reinterpret_cast<char*>(other.d->pObject),
             other.type(), other.id()))
{
}

PiiResourceStatement& PiiResourceStatement::operator= (const PiiResourceStatement& other)
{
  if (&other != this)
    {
      if (other.d->bStringData)
        {
          d->deleteStrings();
          d->pSubject = new QString(other.subject());
          d->pPredicate = new QString(other.predicate());
          d->pObject = new QString(other.object());
          d->bStringData = true;
        }
      else
        {
          d->pSubject = other.d->pSubject;
          d->pPredicate = other.d->pPredicate;
          d->pObject = other.d->pObject;
          d->bStringData = false;
        }
      d->type = other.d->type;
      d->id = other.d->id;
    }
  return *this;
}

bool PiiResourceStatement::isValid() const
{
  return !subject().isEmpty() && !object().isEmpty() && d->type != InvalidType;
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
  return d->bStringData ? *reinterpret_cast<QString*>(d->pObject) : reinterpret_cast<char*>(d->pObject);
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
  d->id = id;
}
