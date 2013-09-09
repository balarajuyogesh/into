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

#include "PiiVersionNumber.h"
#include <QStringList>

const QStringList PiiVersionNumber::_lstGreekLetters = QStringList() <<
  "alpha" << "beta" << "gamma" << "delta" << "epsilon" <<
  "zeta" << "eta" << "theta" << "iota" << "kappa" <<
  "lambda" << "mu" << "nu" << "xi" << "omicron" <<
  "pi" << "rho" << "sigma" << "tau" << "upsilon" <<
  "phi" << "chi" << "psi" << "omega";

PiiVersionNumber::Data::Data()
{
}

PiiVersionNumber::Data::Data(const Data& other) :
  vecParts(other.vecParts),
  strRevision(other.strRevision),
  lstRevisionParts(other.lstRevisionParts)
{
}

PiiVersionNumber::PiiVersionNumber(int major, int minor, int patch, const QString& revision) :
  d(new Data)
{
  d->strRevision = revision;
  if (major >= 0)
    {
      d->vecParts << major;
      if (minor >= 0)
        {
          d->vecParts << minor;
          if (patch >= 0)
            d->vecParts << patch;
        }
    }
  d->lstRevisionParts = revision.split('-', QString::SkipEmptyParts);
}

PiiVersionNumber::PiiVersionNumber(const QString& versionString) :
  d(new Data)
{
  setVersionString(versionString);
}

PiiVersionNumber::PiiVersionNumber(const PiiVersionNumber& other) :
  d(new Data(*other.d))
{
}

PiiVersionNumber::~PiiVersionNumber()
{
  delete d;
}

PiiVersionNumber& PiiVersionNumber::operator= (const PiiVersionNumber& other)
{
  if (this != &other)
    {
      d->vecParts = other.d->vecParts;
      d->strRevision = other.d->strRevision;
      d->lstRevisionParts = other.d->lstRevisionParts;
    }
  return *this;
}

bool PiiVersionNumber::hasRevision() const
{
  return !d->strRevision.isEmpty();
}

QString PiiVersionNumber::revision() const
{
  return d->strRevision;
}

QString PiiVersionNumber::toString() const
{
  QString strResult;
  for (int i=0; i<d->vecParts.size(); ++i)
    {
      if (i > 0)
        strResult += '.';
      strResult += QString::number(d->vecParts[i]);
    }
  if (!d->strRevision.isEmpty())
    strResult.append('-').append(d->strRevision);
  return strResult;
}

bool PiiVersionNumber::setVersionString(const QString& versionString)
{
  int iDashIndex = versionString.indexOf('-');
  d->vecParts.clear();
  d->lstRevisionParts.clear();
  if (iDashIndex >= 0)
    {
      d->strRevision = versionString.mid(iDashIndex+1);
      d->lstRevisionParts = d->strRevision.toLower().split('-', QString::SkipEmptyParts);
    }
  if (iDashIndex != 0)
    {
      // Split version number into pieces
      QStringList lstVersionParts = versionString.mid(0, iDashIndex).split('.');
      // Check that each piece is a valid number and build a vector of
      // ints.
      for (int i=0; i<lstVersionParts.size(); ++i)
        {
          bool bSuccess = false;
          int iPart = lstVersionParts[i].toInt(&bSuccess);
          if (!bSuccess)
            {
              d->vecParts.clear();
              return false;
            }
          d->vecParts << iPart;
        }
    }
  return true;
}

int PiiVersionNumber::part(int index) const
{
  if (index >= 0 && index < d->vecParts.size())
    return d->vecParts[index];
  return 0;
}

int PiiVersionNumber::partCount() const
{
  return d->vecParts.size();
}

int PiiVersionNumber::compare(const PiiVersionNumber& other) const
{
  // First compare version numbers
  int iMaxLen = qMax(d->vecParts.size(), other.d->vecParts.size());
  for (int i=0; i<iMaxLen; ++i)
    {
      int iDiff = (i < d->vecParts.size() ? d->vecParts[i] : 0) -
        (i < other.d->vecParts.size() ? other.d->vecParts[i] : 0);
      if (iDiff < 0)
        return -1;
      else if (iDiff > 0)
        return 1;
    }

  // Compare revision parts
  iMaxLen = qMin(d->lstRevisionParts.size(), other.d->lstRevisionParts.size());
  for (int i=0; i<iMaxLen; ++i)
    {
      bool bSuccess1 = false, bSuccess2 = false;
      int iPart1 =  d->lstRevisionParts[i].toInt(&bSuccess1),
        iPart2 = other.d->lstRevisionParts[i].toInt(&bSuccess2);

      // Both are numeric
      if (bSuccess1 && bSuccess2)
        {
          if (iPart1 < iPart2)
            return -1;
          else if (iPart1 > iPart2)
            return 1;
          continue;
        }

      iPart1 = _lstGreekLetters.indexOf(d->lstRevisionParts[i]);
      iPart2 = _lstGreekLetters.indexOf(other.d->lstRevisionParts[i]);
      // Try to convert from Greek alphabet
      if (iPart1 != -1 && iPart2 != -1)
        {
          if (iPart1 < iPart2)
            return -1;
          else if (iPart1 > iPart2)
            return 1;
          continue;
        }
      
      // Finally, try alphabetical ordering
      iPart1 = d->lstRevisionParts[i].compare(other.d->lstRevisionParts[i]);
      if (iPart1 != 0)
        return iPart1;
    }

  // The version numbers are equal up to the last common part. Now, we
  // need to check the remaining parts.
  QString strTailPart;
  int iMultiplier = 0;
  if (d->lstRevisionParts.size() > other.d->lstRevisionParts.size())
    {
      strTailPart = d->lstRevisionParts[iMaxLen];
      iMultiplier = 1;
    }
  else if (d->lstRevisionParts.size() < other.d->lstRevisionParts.size())
    {
      strTailPart = other.d->lstRevisionParts[iMaxLen];
      iMultiplier = -1;
    }

  if (strTailPart.size() != 0)
    {
      // The version number with a numeric suffix is larger. If the
      // suffix is not a number, then it is smaller.
      bool bSuccess;
      strTailPart.toInt(&bSuccess);
      return bSuccess ? iMultiplier : -iMultiplier;
    }
  
  return 0;
}

bool PiiVersionNumber::operator== (const PiiVersionNumber& other) const
{
  return d->vecParts == other.d->vecParts && d->strRevision == other.d->strRevision;
}

bool PiiVersionNumber::operator!= (const PiiVersionNumber& other) const
{
  return d->vecParts != other.d->vecParts || d->strRevision != other.d->strRevision;
}
