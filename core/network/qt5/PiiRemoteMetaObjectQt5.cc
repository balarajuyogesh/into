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

class PiiRemoteMetaObject::Data : public PiiRemoteObject::Data
{
public:
  Data(QObject*);

  int typeIndex(int typeId)
  {
    if (typeId < int(QVariant::UserType))
      return typeId;
    return 0x80000000 | stringData.add(QMetaType::typeName(typeId));
  }

  template <class T> int addFunctionData(const QList<T>& functions, uint paramIndex, uint flags);
  template <class T> void addFunctionParams(const QList<T>& functions);
  void addPropertyData();

  QObject* pObject;

  struct StringData
  {
    StringData() : _iStringIndex(-1) {}

    int add(const QString& word)
    {
      QByteArray aWord(word.toLatin1());
      return add(aWord.constData(), aWord.size());
    }
    int add(const char* word, int len = -1);
    void build()
    {
      _aCombined = _aHeader + _aStringData;
      _aHeader.clear();
      _aStringData.clear();
      _iStringIndex = -1;
    }
    const QArrayData* data() const { return reinterpret_cast<const QArrayData*>(_aCombined.constData()); }

    QByteArray _aHeader, _aStringData, _aCombined;
    int _iStringIndex;
  } stringData;

  QVector<uint> vecMetaData;

  QMetaObject metaObject;
  bool bMetaCreated;
  QList<Function> lstFunctions;
  QList<Signal> lstSignals;
  QList<Property> lstProperties;
};

int PiiRemoteMetaObject::Data::StringData::add(const char* word, int len)
{
  ++_iStringIndex;
  _aHeader.resize((_iStringIndex + 1) * sizeof(QByteArrayData));

  QByteArrayData* pData = reinterpret_cast<QByteArrayData*>(_aHeader.data() + _iStringIndex * sizeof(QByteArrayData));
  pData->ref.atomic._q_value = -1;
  if (len == -1) len = std::strlen(word);
  pData->size = len;
  pData->alloc = 0;
  pData->capacityReserved = 0;
  pData->offset = sizeof(QByteArrayData) + _aStringData.size();
  _aStringData.append(word, len);
  _aStringData.append('\0');

  // Fix offsets
  for (int i=0; i<_iStringIndex; ++i)
    {
      pData = reinterpret_cast<QByteArrayData*>(_aHeader.data() + i * sizeof(QByteArrayData));
      pData->offset += sizeof(QByteArrayData);
    }

  return _iStringIndex;
}

PiiRemoteMetaObject::Data::Data(QObject* object) :
  pObject(object),
  bMetaCreated(false)
{
  vecMetaData.fill(0, iMetaHeaderSize+1);
  vecMetaData[0] = 7;

  /* Header structure:
    0,    revision
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

  stringData.add("PiiRemoteMetaObject");
  // Null string is always at index 1
  stringData.add("");
  stringData.build();

  metaObject.d.superdata = &QObject::staticMetaObject;
  metaObject.d.stringdata = stringData.data();
  metaObject.d.data = vecMetaData.constData();
  metaObject.d.static_metacall = 0;
  metaObject.d.relatedMetaObjects = 0;
  metaObject.d.extradata = 0;
}

template <class T> int PiiRemoteMetaObject::Data::addFunctionData(const QList<T>& functions, uint paramIndex, uint flags)
{
  for (int i=0; i<functions.size(); ++i)
    {
      int iNameIndex = stringData.add(functions[i].strName);
      int iParamCnt = functions[i].lstParamTypes.size();
      vecMetaData
        << iNameIndex
        << iParamCnt
        << paramIndex
        << 1 // null tag
        << flags;
      paramIndex += 1 + iParamCnt * 2;
    }
  return paramIndex;
}

template <class T> void PiiRemoteMetaObject::Data::addFunctionParams(const QList<T>& functions)
{
  for (int i=0; i<functions.size(); ++i)
    {
      vecMetaData << typeIndex(functions[i].returnType);
      int iParamCnt = functions[i].lstParamTypes.size();
      for (int j=0; j<iParamCnt; ++j)
        vecMetaData << typeIndex(functions[i].lstParamTypes[j]);
      // Parameter names all point to the null string.
      for (int j=0; j<iParamCnt; ++j)
        vecMetaData << 1;
    }
}

void PiiRemoteMetaObject::Data::addPropertyData()
{
  using namespace PiiNetworkPrivate;
  uint uiPropertyFlags =  Readable | Writable | StdCppSet | Designable | Scriptable | Stored;
  for (int i=0; i<lstProperties.size(); ++i)
    {
      // Add Name to string data and its index to meta data
      vecMetaData << stringData.add(lstProperties[i].strName);
      vecMetaData << typeIndex(lstProperties[i].type);
      // PENDING EnumOrFlag
      vecMetaData << uiPropertyFlags;
    }
}

void PiiRemoteMetaObject::createMetaObject()
{
  PII_D;
  if (d->bMetaCreated)
    return;
  d->bMetaCreated = true;

  d->stringData.add("PiiRemoteMetaObject");
  d->stringData.add("");

  collectFunctions(true); // signals
  collectFunctions(false); // other functions
  collectProperties();

  d->vecMetaData.resize(iMetaHeaderSize);

  // Store the number of functions ...
  d->vecMetaData[iMethodIndex] = d->lstFunctions.size() + d->lstSignals.size();
  // Functions start immediately after the header
  d->vecMetaData[iMethodIndex + 1] = iMetaHeaderSize;
  // The first N are signals
  d->vecMetaData[iSignalIndex] = d->lstSignals.size();

  // Parameters start after function data (5 numbers per function)
  uint iParamIndex = iMetaHeaderSize + (d->lstSignals.size() + d->lstFunctions.size()) * 5;
  // "Flags" seems to be 5 for signals and 2 for other functions.
  iParamIndex = d->addFunctionData(d->lstSignals, iParamIndex, 0x5);
  iParamIndex = d->addFunctionData(d->lstFunctions, iParamIndex, 0x2);
  d->addFunctionParams(d->lstSignals);
  d->addFunctionParams(d->lstFunctions);

  d->vecMetaData[iPropertyIndex] = d->lstProperties.size();
  d->vecMetaData[iPropertyIndex + 1] = d->vecMetaData.size();

  d->addPropertyData();

  d->vecMetaData << 0; // EOD

#if 0
  printf("  0: ");
  for (int i=0; i<d->vecMetaData.size(); ++i)
    {
      bool bHex = true, bLineFeed = false;
      if (i < iMetaHeaderSize)
        {
          bHex = false;
          bLineFeed = i member_of (0,1,3,5,7,9,11,12,13);
        }
      else if (i < iMetaHeaderSize + 5 * (d->lstSignals.size() + d->lstFunctions.size()))
        {
          bHex = bLineFeed = (i - iMetaHeaderSize) % 5 == 4;
        }
      else
        bLineFeed = i % 16 == 15;

      printf(bHex ? "0x%x" : "%u", d->vecMetaData[i]);
      if (bLineFeed)
        printf("\n%3d: ", i+1);
      else
        printf(", ");
    }
  printf("\n");
#endif

  d->stringData.build();
  d->metaObject.d.stringdata = d->stringData.data();
  d->metaObject.d.data = d->vecMetaData.constData();
}

void PiiRemoteMetaObject::collectProperties()
{
  PII_D;
  QList<QByteArray> lstProperties = readDirectoryList("properties/");
  // Properties are encoded as "type name", e.g. "int value"
  QRegExp propExp("[^ ]+ [^ ]+");

  d->lstProperties.clear();

  for (int i=0; i<lstProperties.size(); ++i)
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

      d->lstProperties << Property(type, lstProperties[i].constData() + iSpaceIndex + 1);
   }
}

void PiiRemoteMetaObject::collectFunctions(bool listSignals)
{
  PII_D;
  QList<QByteArray> lstSignatures = readDirectoryList(listSignals ? "signals/" : "functions/");
  QRegExp funcExp("([^ ]+ )?([^ (]+)\\(([^ )]*)\\)");

  if (listSignals)
    d->lstSignals.clear();
  else
    d->lstFunctions.clear();

  for (int i=0; i<lstSignatures.size(); ++i)
    {
      qDebug("%s", lstSignatures[i].constData());
      if (!funcExp.exactMatch(QString(lstSignatures[i])))
        {
          piiWarning(QString("Invalid function signature: %0").arg(QString(lstSignatures[i])));
          continue;
        }

      int iSpaceIndex = funcExp.cap(1).indexOf(' ');
      int returnType = QMetaType::Void;
      if (iSpaceIndex != -1)
        {
          lstSignatures[i][iSpaceIndex] = 0;
          returnType = QMetaType::type(lstSignatures[i].constData());
        }
      // Check that each parameter is a valid type name
      // PENDING split(',') fails with template types such as PiiVector<double,3>
      QList<QByteArray> lstParams;
      if (!funcExp.cap(3).isEmpty())
        lstParams = funcExp.cap(3).toLatin1().split(',');
      QList<int> lstParamTypes;
      const char* pSignature = lstSignatures[i].constData() + iSpaceIndex + 1;

      for (int j=0; j<lstParams.size(); ++j)
        {
          int type = QMetaType::type(lstParams[j].constData());
          if (type == 0)
            {
              piiWarning(QString("Unrecognized parameter type: %0").arg(QString(lstParams[j])));
              goto nextSignature;
            }
          lstParamTypes << type;
        }

      if (listSignals)
        d->lstSignals << Signal(pSignature, returnType, funcExp.cap(2), lstParamTypes);
      else
        d->lstFunctions << Function(pSignature, returnType, funcExp.cap(2), lstParamTypes);
    nextSignature:;
    }
}
