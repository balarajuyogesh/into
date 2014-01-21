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

#include "PiiFileInfo.h"
#include "PiiHttpProtocol.h"
#include "PiiHttpResponseHeader.h"

#include <QFileInfo>
#include <PiiException.h>

PiiFileInfo::Data::Data() :
  bExists(false)
{
}

PiiFileInfo::Data::Data(const QString& uri) :
  strUri(uri),
  bExists(false)
{
}

PiiFileInfo::PiiFileInfo() :
  d(new Data())
{}

PiiFileInfo::PiiFileInfo(const QString& uri) :
  d(new Data(uri))
{
  refresh();
}

PiiFileInfo::PiiFileInfo(const PiiFileInfo& other) :
  d(other.d)
{
  d->reserve();
}

PiiFileInfo::~PiiFileInfo()
{
  d->release();
}

PiiFileInfo& PiiFileInfo::operator= (const PiiFileInfo& other)
{
  other.d->assignTo(d);
  return *this;
}

bool PiiFileInfo::exists() const
{
  return d->bExists;
}

QDateTime PiiFileInfo::lastModified() const
{
  return d->lastModifiedTime;
}

void PiiFileInfo::setUri(const QString& uri)
{
  d->strUri = uri;
  refresh();
}

void PiiFileInfo::refresh()
{
  if (d->strUri.startsWith("file://"))
    {
      QFileInfo info(d->strUri.mid(7));
      d->bExists = info.exists();
      d->lastModifiedTime = info.lastModified();
      return;
    }

  d->bExists = false;
  d->lastModifiedTime = QDateTime();

  try
    {
      PiiHttpResponseHeader header = PiiNetwork::readHeader(d->strUri);
      if (header.statusCode() == PiiHttpProtocol::OkStatus)
        {
          d->bExists = true;
          QString strLastModified = header.value("Last-Modified");
          if (!strLastModified.isEmpty())
            d->lastModifiedTime = PiiHttpProtocol::stringToTime(strLastModified);
        }
    }
  catch (PiiException&)
    {}
}
