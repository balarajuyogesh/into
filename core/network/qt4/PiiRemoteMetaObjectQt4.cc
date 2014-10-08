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

static int iStringHeaderSize = 41;
static int iEmptyStringIndex = 19;
static int iParameterIndex = 20;

class PiiRemoteMetaObject::Data : public PiiRemoteObject::Data
{
public:
  Data(QObject*);

  ~Data()
  {
    qDeleteAll(lstProperties);
  }

  QObject* pObject;

  QByteArray stringData;
  QVector<uint> vecMetaData;

  QMetaObject metaObject;
  bool bMetaCreated;
  QList<Function> lstFunctions;
  QList<Signal> lstSignals;
  QList<Property*> lstProperties;
};

PiiRemoteMetaObject::Data::Data(QObject* object) :
  pObject(object),
  stringData("PiiRemoteMetaObject\0a,b,c,d,e,f,g,h,i,j\0\0\0", iStringHeaderSize+1),
  bMetaCreated(false)
{
  vecMetaData.fill(0, iMetaHeaderSize+1);
  vecMetaData[0] = 5;
  /* Header structure:
    5,    revision
    0,    classname
    0, 0, classinfo
    0, 0, methods
    0, 0, properties
    0, 0, enums/sets
    0, 0, constructors
    0,    flags
    0,    signalCount
    0     end of data
  */

  metaObject.d.superdata = &QObject::staticMetaObject;
  metaObject.d.stringdata = stringData.constData();
  metaObject.d.data = vecMetaData.constData();
  metaObject.d.extradata = 0;
}

void PiiRemoteMetaObject::createMetaObject()
{
  PII_D;
  if (d->bMetaCreated)
    return;
  d->bMetaCreated = true;
  d->stringData.resize(iStringHeaderSize);
  d->vecMetaData.resize(iMetaHeaderSize);
  d->vecMetaData << 0; // EOD

  collectFunctions(true); // signals
  collectFunctions(false); // other functions

  // Store the number of functions ...
  d->vecMetaData[iMethodIndex] = d->lstFunctions.size() + d->lstSignals.size();
  // Functions start immediately after the header
  d->vecMetaData[iMethodIndex + 1] = iMetaHeaderSize;
  // The first N are signals
  d->vecMetaData[iSignalIndex] = d->lstSignals.size();

  collectProperties();

  d->metaObject.d.stringdata = d->stringData.constData();
  d->metaObject.d.data = d->vecMetaData.constData();
}

void PiiRemoteMetaObject::collectProperties()
{
  PII_D;
  QList<QByteArray> lstProperties = readDirectoryList("properties/");
  // Properties are encoded as "type name", e.g. "int value"
  QRegExp propExp("[^ ]+ [^ ]+");

  // Remove EOD marker from vecMetaData
  d->vecMetaData.resize(d->vecMetaData.size()-1);
  int iFirstPropertyIndex = d->vecMetaData.size();
  d->lstProperties.clear();

  for (int i = 0; i < lstProperties.size(); ++i)
    {
      // This also catches the special case of no properties (one empty entry in the list).
      if (!propExp.exactMatch(QString(lstProperties[i])))
        continue;

      int iSpaceIndex = lstProperties[i].indexOf(' ');
      lstProperties[i][iSpaceIndex] = 0;

      // Check that the variant type is correctly specified
      int type = QMetaType::type(lstProperties[i].constData());
      if (type == 0)
        {
          piiWarning(QString("Unsupported remote property type: ") + QString(lstProperties[i]));
          continue;
        }

      // Add property name to string data and its index to metadata
      d->vecMetaData << d->stringData.size();
      d->stringData.append(lstProperties[i].constData() + iSpaceIndex + 1);
      d->stringData.append('\0');
      // Add property type
      d->vecMetaData << d->stringData.size();
      d->stringData.append(lstProperties[i].constData());
      d->stringData.append('\0');

      using namespace PiiNetworkPrivate;
      // Some magic again. These are the property "flags".
      uint uiFlags =  Readable | Writable | StdCppSet | Designable | Scriptable | Stored;
      if (type < int(QVariant::UserType))
        uiFlags |= type << 24;
      d->vecMetaData << uiFlags;

      d->lstProperties << new Property(type, lstProperties[i].constData() + iSpaceIndex + 1);
   }

  // Add EOD marker back
  d->vecMetaData << 0;

  // Store the number of properties and the start index
  d->vecMetaData[iPropertyIndex] = d->lstProperties.size();
  d->vecMetaData[iPropertyIndex + 1] = d->lstProperties.isEmpty() ? 0 : iFirstPropertyIndex;
}

void PiiRemoteMetaObject::collectFunctions(bool listSignals)
{
  PII_D;
  QList<QByteArray> lstSignatures = readDirectoryList(listSignals ? "signals/" : "functions/");
  QRegExp funcExp("([^ ]+ )?([^ (]+)\\(([^ )]*)\\)");

  // Remove EOD marker from vecMetaData
  d->vecMetaData.resize(d->vecMetaData.size()-1);

  if (listSignals)
    d->lstSignals.clear();
  else
    d->lstFunctions.clear();

  for (int i=0; i<lstSignatures.size(); ++i)
    {
      if (!funcExp.exactMatch(QString(lstSignatures[i])))
        continue;

      // Check that each parameter is a valid type name
      QStringList lstParams = funcExp.cap(3).split(',', QString::SkipEmptyParts);
      int iSpaceIndex = lstSignatures[i].indexOf(' ');
      int returnType = 0;
      if (iSpaceIndex != -1)
        {
          lstSignatures[i][iSpaceIndex] = 0;
          returnType = QMetaType::type(lstSignatures[i].constData());
        }
      const char* pSignature;
      QList<int> lstParamTypes;
      for (int j=0; j<lstParams.size(); ++j)
        {
          int type = QMetaType::type(qPrintable(lstParams[j]));
          if (type == 0)
            goto nextSignature;
          lstParamTypes << type;
        }
      // Add index of signature to metadata
      d->vecMetaData << d->stringData.size();
      lstSignatures[i].append('\0');
      pSignature = lstSignatures[i].constData() + iSpaceIndex + 1;
      d->stringData.append(pSignature);
      d->stringData.append('\0');

      // Add index of (dummy) parameter names. Qt allows at most 10 parameters.
      d->vecMetaData << iParameterIndex + (10 - lstParamTypes.size()) * 2;

      // Not void
      if (iSpaceIndex != -1)
        {
          // Add index to return type to metadata
          d->vecMetaData << d->stringData.size();
          // Add return value to string data
          d->stringData.append(lstSignatures[i].constData());
          d->stringData.append('\0');
        }
      else
        // Void function has null return type
        d->vecMetaData << iEmptyStringIndex;

      // Add "tag" and "flags". No idea what these are for. "5" seems to identify a signal.
      d->vecMetaData << iEmptyStringIndex << (listSignals ? 5 : 2);

      if (listSignals)
        d->lstSignals << Signal(pSignature, returnType, funcExp.cap(2), lstParamTypes);
      else
        d->lstFunctions << Function(pSignature, returnType, funcExp.cap(2), lstParamTypes);
    nextSignature:;
    }

  // Add EOD marker back
  d->vecMetaData << 0;
}
