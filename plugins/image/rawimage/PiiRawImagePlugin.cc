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

#include "PiiRawImagePlugin.h"
#include "PiiRawImageHandler.h"

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(piiraw, PiiRawImagePlugin);
#endif

PiiRawImagePlugin::PiiRawImagePlugin(QObject* parent) :
  QImageIOPlugin(parent)
{
}

QImageIOPlugin::Capabilities PiiRawImagePlugin::capabilities(QIODevice* device, const QByteArray& format) const
{
  if (format == "praw")
    return Capabilities(CanRead | CanWrite);
  if (!format.isEmpty())
    return 0;
  if (!device->isOpen())
    return 0;

  Capabilities cap;
  if (device->isReadable() && PiiRawImageHandler::canRead(device))
    cap |= CanRead;
  if (device->isWritable())
    cap |= CanWrite;
  return cap;
}

QImageIOHandler* PiiRawImagePlugin::create(QIODevice* device, const QByteArray & format) const
{
  QImageIOHandler *pHandler = new PiiRawImageHandler;
  pHandler->setDevice(device);
  pHandler->setFormat(format);
  return pHandler;
}

QStringList PiiRawImagePlugin::keys() const
{
  return QStringList() << "praw";
}
