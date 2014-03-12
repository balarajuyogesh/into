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

#include "PiiUtil.h"

#include "PiiFunctional.h"

#include <QtDebug>
#include <QLinkedList>
#include <QCoreApplication>
#include <QHash>
#include <QSet>
#include <cmath>

namespace Pii
{
  QStringList argsToList(int argc, char* argv[])
  {
    QStringList lstResult;
    lstResult.reserve(argc);
    for (int i=0; i<argc; ++i)
      lstResult << argv[i];
    return lstResult;
  }

  bool isParent(const QObject* parent, const QObject* child)
  {
    QObject* parentObj = child->parent();
    while (parentObj != 0)
      {
        if (parentObj == parent)
          return true;
        parentObj = parentObj->parent();
      }
    return false;
  }

  QList<QObject*> findAllParents(const QObject* obj, unsigned int maxParents)
  {
    QList<QObject*> result;
    QObject* parentObj = obj->parent();
    while (parentObj != 0 && maxParents--)
      {
        result << parentObj;
        parentObj = parentObj->parent();
      }
    return result;
  }

  QObject* findCommonParent(const QObject* obj1, const QObject* obj2, int* parentIndex)
  {
    QList<QObject*> parents2 = findAllParents(obj2);
    QList<QObject*> parents1 = findAllParents(obj1);
    for (int i=0; i<parents1.size(); i++)
      {
        // this is a common parent
        if (parents2.contains(parents1[i]))
          {
            if (parentIndex)
              *parentIndex = i;
            return parents1[i];
          }
      }
    return 0;
  }

  bool isA(const char* className, const QObject* obj)
  {
    if (obj == 0) return false;
    const QMetaObject* pMeta = obj->metaObject();
    while (pMeta != 0)
      {
        if (!strcmp(pMeta->className(), className))
          return true;
        pMeta = pMeta->superClass();
      }
    return false;
  }

  uint qHash(const char* key)
  {
    if (key == 0)
      return 0;

    uint h = 0;
    uint g;

    while (*key) {
        h = (h << 4) + *key++;
        if ((g = (h & 0xf0000000)) != 0)
            h ^= g >> 23;
        h &= ~g;
    }
    return h;
  }

  static bool checkList(int temp, const QString& string )
  {
    QList<int> list;
    if ( string == "*" )
      {
        return true;
      }
    else
      {
        QStringList list2 = string.split(",");
        for ( int k=0; k<list2.size(); k++ )
          {
            QStringList list3 = list2[k].split("-");
            if ( list3.size() == 1 )
              list << list3[0].toInt();
            else
              {
                for ( int g=list3[0].toInt(); g<=list3[1].toInt(); g++ )
                  list << g;
              }
          }

      }
    if ( list.contains(temp) )
      return true;
    else
      return false;
  }

  bool matchCrontab(QStringList list, QDateTime timeStamp)
  {
    int table[6] = {timeStamp.time().minute(),
                    timeStamp.time().hour(),
                    timeStamp.date().day(),
                    timeStamp.date().month(),
                    timeStamp.date().dayOfWeek(),
                    timeStamp.date().weekNumber()};

    //qDebug("timeStamp : %i %i %i %i %i %i", table[0], table[1], table[2], table[3], table[4], table[5]);

    bool ret = false;
    for ( int i=0; i<list.size(); i++ )
      {
        QStringList list1 = list[i].split(" ");
        if ( list1.size() != 6 )
          {
            ret = false;
            break;
          }
        else
          {
            bool line = true;
            for ( int k=0; k<list1.size(); k++ )
              {
                if ( !checkList(table[k],list1[k]) )
                  {
                    line = false;
                    break;
                  }

              }
            ret = line;

          }

        if ( ret )
          return ret;
      }

    return ret;
  }

  int findSeparator(const QString& str, QChar separator, int startIndex, QChar escape)
  {
    int index;
    // Loop until a real separator has been found
    for (;;startIndex = index+1)
      {
        index = str.indexOf(separator, startIndex);
        if (index == -1)
          break;
        int escapeIndex = index-1;
        while (escapeIndex > 0 && str[escapeIndex] == escape) --escapeIndex;
        if (((index - escapeIndex) & 1) != 0) // even number of escapes -> this is it
          break;
      }
    return index;
  }

  QStringList splitQuoted(const QString& str, QChar separator, QChar quote,
                          QString::SplitBehavior behavior)
  {
    QStringList lstResult;
    int iPos = 0;
    while (iPos < str.size())
      {
        // Part starts with quote -> find its pair
        if (str[iPos] == quote)
          {
            int iNextQuotePos = findSeparator(str, quote, iPos+1);
            // Pair was found -> strip quotes
            if (iNextQuotePos != -1)
              {
                lstResult << str.mid(iPos+1, iNextQuotePos-iPos-1);
                iPos = iNextQuotePos + 1;
                // Find the next separator
                while (iPos < str.size() && str[iPos++] != separator) ;
              }
            // No matching quote found -> use the rest of the string
            else
              {
                lstResult << str.mid(iPos);
                break;
              }
          }
        // Empty part
        else if (str[iPos] == separator)
          {
            ++iPos;
            if (behavior == QString::KeepEmptyParts)
              lstResult << "";
          }
        // Unquoted part?
        else
          {
            // Strip white space at the beginning of a quoted string.
            int iTmpPos = iPos;
            while (iTmpPos < str.size() - 1 && str[iTmpPos] == ' ') ++iTmpPos;
            if (str[iTmpPos] == quote)
              iPos = iTmpPos;
            // The string didn't start with a white space - quote sequence.
            else
              {
                iTmpPos = iPos;
                while (iTmpPos < str.size() && str[iTmpPos] != separator) ++iTmpPos;
                if (iTmpPos == str.size())
                  {
                    lstResult << str.mid(iPos);
                    break;
                  }
                else
                  {
                    lstResult << str.mid(iPos, iTmpPos-iPos);
                    iPos = iTmpPos + 1;
                  }
              }
          }
      }
    return lstResult;
  }

  QVariantMap decodeProperties(const QString& encodedProperties,
                               QChar propertySeparator,
                               QChar valueSeparator,
                               QChar escape,
                               PropertyDecodingFlags flags)
  {
    QVariantMap result;
    int previousIndex = 0, propertySeparatorIndex = 0;

    // Loop until all property separators have been handled
    for (; propertySeparatorIndex < encodedProperties.size(); previousIndex = propertySeparatorIndex + 1)
      {
        propertySeparatorIndex = findSeparator(encodedProperties, propertySeparator, previousIndex, escape);
        if (propertySeparatorIndex == -1)
          propertySeparatorIndex = encodedProperties.size();

        // Now separate property name and value
        QString pair = encodedProperties.mid(previousIndex, propertySeparatorIndex - previousIndex); // may be empty
        int valueSeparatorIndex = findSeparator(pair, valueSeparator, 0, escape);
        // No value separator -> ignore this value
        if (valueSeparatorIndex == -1)
          continue;

        // Take property name and unescape
        QRegExp unescape(escape == '\\' ? QString("\\\\(.)") : QString("\\%1(.)").arg(escape));
        QString name = pair.left(valueSeparatorIndex).replace(unescape, "\\1");
        if (flags & TrimPropertyName || flags & RemoveQuotes)
          name = name.trimmed();
        if (name.isEmpty())
          continue;
        if (flags & DowncasePropertyName)
          name = name.toLower();

        if (flags & RemoveQuotes && name.size() > 1 && name[0] == '"' && name[name.size()-1] == '"')
          name = name.mid(1, name.size()-2);
        if (name.isEmpty())
          continue;

        // Take property value and unescape
        QString value = pair.mid(valueSeparatorIndex+1).replace(unescape, "\\1");;
        if (flags & TrimPropertyValue || flags & RemoveQuotes)
          value = value.trimmed();
        if (flags & RemoveQuotes && value.size() > 1 && value[0] == '"' && value[value.size()-1] == '"')
          value = value.mid(1, value.size()-2);

        result[name] = value;
      }

    return result;
  }

  QList<QPair<QString,QVariant> > propertyList(const QObject* obj,
                                               int propertyOffset,
                                               PropertyFlags flags)
  {
    return properties<QList<QPair<QString,QVariant> > >(obj, propertyOffset, flags);
  }

  const char* propertyName(const QObject* obj, const QString& name)
  {
    QString lowerCaseProp = name.toLower();
    const QMetaObject* metaObj = obj->metaObject();
    for (int i=metaObj->propertyCount(); i--;)
      {
        QMetaProperty prop = metaObj->property(i);
        if (QString(prop.name()).toLower() == lowerCaseProp)
          return prop.name();
      }
    return 0;
  }

  template <class Map> bool setProperties(QObject* obj, const QStringList& properties, const Map& valueMap,
                                          Qt::CaseSensitivity sensitivity,
                                          const QString& commentMark)
  {
    bool bResult = true;
    for (int i=0; i<properties.size(); ++i)
      {
        QString key = properties[i];
        if (key.startsWith(commentMark)) // ignore comment lines
          continue;
        if (sensitivity == Qt::CaseInsensitive)
          {
            const char* name = propertyName(obj, key);
            bResult &= obj->setProperty(name, valueMap.value(key));
          }
        else
          bResult &= obj->setProperty(key.toLatin1(), valueMap.value(key));
      }
    return bResult;
  }

  // Explicit instantiations.
  PII_DEFINE_EXPORTED_FUNCTION_TEMPLATE(bool, setProperties<QSettings>,
                                        (QObject* obj,
                                         const QStringList& properties,
                                         const QSettings& valueMap,
                                         Qt::CaseSensitivity sensitivity,
                                         const QString& commentMark));

  PII_DEFINE_EXPORTED_FUNCTION_TEMPLATE(bool, setProperties<QVariantMap>,
                                        (QObject* obj,
                                         const QStringList& properties,
                                         const QVariantMap& valueMap,
                                         Qt::CaseSensitivity sensitivity,
                                         const QString& commentMark));

  struct Magnitude
  {
    char symbol;
    int exponent;
  };

  double toDouble(const QString& number, bool *ok)
  {
    static const Magnitude magnitudes[] =
      { { 'Y', 24 },
        { 'Z', 21 },
        { 'E', 18 },
        { 'P', 15 },
        { 'T', 12 },
        { 'G', 9 },
        { 'M', 6 },
        { 'k', 3 },
        { 'h', 2 },
        { 'e', 1 },
        { 'd', -1 },
        { 'c', -2 },
        { 'm', -3 },
        { 'u', -6 },
        { 'n', -9 },
        { 'p', -12 },
        { 'f', -15 },
        { 'a', -18 },
        { 'z', -21 },
        { 'y', -24 }
      };

    if (number.size() > 0)
      {
        QChar cLast = number[number.size()-1];
        for (unsigned int i=0; i<sizeof(magnitudes)/sizeof(Magnitude); ++i)
          if (cLast == magnitudes[i].symbol)
            return number.left(number.size()-1).toDouble(ok) * ::pow(10.0, double(magnitudes[i].exponent));
      }

    return number.toDouble(ok);
  }
  
  typedef QLinkedList<QPair<int,int> > RelationList;

  void joinNeighbors(int index, RelationList& pairs, QList<int>& indices)
  {
    QList<int> newIndices;
    RelationList::iterator i = pairs.begin();
    while (i != pairs.end())
      {
        // If a match was found in the pair, add the other index in the
        // pair to the index list. Then remove the found match from the
        // list.
        if (i->first == index)
          {
            newIndices << i->second;
            i = pairs.erase(i);
          }
        else if (i->second == index)
          {
            newIndices << i->first;
            i = pairs.erase(i);
          }
        else
          ++i;
      }
    // Now we have found all direct neighbors of <index>. Let's find
    // (recursively) the neighbors of its neighbors.
    for (int i=0; i<newIndices.size(); ++i)
      if (newIndices[i] != index) //no need to check myself again...
        joinNeighbors(newIndices[i], pairs, indices);

    // Store all indices to a single list
    indices << newIndices;
  }

  QList<QList<int> > findNeighbors(RelationList& pairs)
  {
    QList<QList<int> > lstResult;

    while (!pairs.isEmpty())
      {
        QList<int> lstIndices;
        int iFirst = pairs.first().first;
        lstIndices << iFirst;

        // This removes all neighbors of the first item from <pairs>.
        joinNeighbors(iFirst, pairs, lstIndices);
        qSort(lstIndices);
        for (int i=lstIndices.size()-2; i>=0; --i)
          if (lstIndices[i+1] == lstIndices[i])
            lstIndices.removeAt(i);

        lstResult << lstIndices;
      }

    return lstResult;
  }

  /* The following two structures encapsulate differences between
     single- and multilayered dependency resolution so that the
     algorithm implementation is the same in both cases.
   */
  struct SingleLayerOrder
  {
    typedef QHash<int, int> DepCache;
    typedef QList<int> Level;
    typedef QList<Level> LevelList;

    static inline int& primary(int& count) { return count; }
    static inline int& secondary(int& count) { return count; }
    static inline void update(DepCache&) {}
    static inline void addLevel(LevelList&) {}
    static inline int nextLevel(int i) { return i; }
    static inline void insert(Level& level, int i)
    {
      if (!level.contains(i))
        level.append(i);
    }
    static inline const LevelList& toLists(const LevelList& levels) { return levels; }
  };

  struct MultiLayerOrder
  {
    typedef QHash<int, QPair<int,int> > DepCache;
    typedef QSet<int> Level;
    typedef QList<Level> LevelList;

    static inline int& primary(QPair<int,int>& pair) { return pair.first; }
    static inline int& secondary(QPair<int,int>& pair) { return pair.second; }

    static inline void update(DepCache& counts)
    {
      for (DepCache::iterator it = counts.begin(); it != counts.end(); ++it)
        it->first = it->second;
    }

    static inline void addLevel(LevelList& levels) { levels << Level(); }
    static inline int nextLevel(int i) { return i+1; }
    static inline void insert(Level& level, int i) { level.insert(i); }
    static inline QList<QList<int> > toLists(const LevelList& levels)
    {
      QList<QList<int> > lstResult;
      for (int i=0; i<levels.size(); ++i)
        {
          // May happen if the input has dependency loops.
          if (levels[i].isEmpty())
            break;
          lstResult.append(levels[i].toList());
        }
      return lstResult;
    }
  };

  template <class Order>
  QList<QList<int> > findDependencies(RelationList& pairs)
  {
    // Initialize a cache that counts dependencies for all elements.
    typedef typename Order::DepCache DepCache;
    DepCache hshDepCounts;

    // The cache holds two dependency counters. The secondary one is
    // updated on each iteration step. The primary counters are
    // updated in batch once a round is done.
    for (RelationList::iterator it = pairs.begin(); it != pairs.end(); ++it)
      ++Order::secondary(hshDepCounts[it->second]);

    typename Order::LevelList lstLevels;
    lstLevels << typename Order::Level();
    int iLevel = 0;
    bool bPairRemoved = true;

    while (!pairs.isEmpty() && bPairRemoved)
      {
        // Make secondary counters primary.
        Order::update(hshDepCounts);
        Order::addLevel(lstLevels);

        bPairRemoved = false;
        for (RelationList::iterator it = pairs.begin(); it != pairs.end();)
          {
            // This element has no dependencies.
            if (Order::primary(hshDepCounts[it->first]) == 0)
              {
                // Put it on the current dependency level.
                Order::insert(lstLevels[iLevel], it->first);
                // If its pair only had this one dependency, put it on
                // the next level.
                if (--Order::secondary(hshDepCounts[it->second]) == 0)
                  Order::insert(lstLevels[Order::nextLevel(iLevel)], it->second);
                it = pairs.erase(it);
                bPairRemoved = true;
              }
            else
              ++it;
          }
        iLevel = Order::nextLevel(iLevel);
      }

    return Order::toLists(lstLevels);
  }

  QList<QList<int> > findDependencies(RelationList& pairs, DependencyOrder order)
  {
    if (order == AnyValidOrder)
      return findDependencies<SingleLayerOrder>(pairs);
    QList<QList<int> > lstResult(findDependencies<MultiLayerOrder>(pairs));
    if (order == SortedLayeredOrder)
      {
        for (int i=0; i<lstResult.size(); ++i)
          qSort(lstResult[i]);
      }
    return lstResult;
  }

  QList<int> parseTypes(const QByteArray& types)
  {
    QList<int> lstResult;
    if (types.isEmpty())
      return lstResult;
    QList<QByteArray> lstTypes(types.split(','));
    for (int i=0; i<lstTypes.size(); ++i)
      {
        int type = QMetaType::type(lstTypes[i]);
        if (type == 0)
          return QList<int>();
        lstResult << type;
      }
    return lstResult;
  }

  QString escape(const QString& source)
  {
    QString strResult;
    strResult.reserve(source.size() + source.size()/2);
    for (int i=0; i<source.size(); ++i)
      {
        int iChar = source[i].toLatin1();
        if (iChar == '\\')
          {
            strResult += '\\';
            strResult += '\\';
          }
        else if (iChar == '\n')
          {
            strResult += '\\';
            strResult += 'n';
          }
        else if (iChar == '\r')
          {
            strResult += '\\';
            strResult += 'r';
          }
        else if (iChar == '"')
          {
            strResult += '\\';
            strResult += '"';
          }
        else if (iChar >= ' ' && iChar <= '~')
          strResult += source[i];
        else
          // four hex numbers, fill with zeros
          strResult += QString("\\u%1").arg(source[i].unicode(), 4, 16, QLatin1Char('0'));
      }
    return strResult;
  }

  QString escape(const QVariant& value)
  {
    switch (value.type())
      {
      case QVariant::String: return '"' + escape(value.toString()) + '"';
      case QVariant::Int:
      case QVariant::Double:
      case QVariant::LongLong:
      case QVariant::UInt:
      case QVariant::ULongLong:
      case QVariant::Bool: return value.toString();
      default:
        return QString();
      }
  }

  QString unescapeString(const QString& source)
  {
    QString strResult;
    strResult.reserve(source.size());

    for (int i=0; i<source.size(); ++i)
      {
        int iChar = source[i].toLatin1();
        if (iChar == '\\' && i<source.size()-1)
          {
            iChar = source[++i].toLatin1();
            switch (iChar)
              {
              case 'n': strResult += '\n'; break;
              case 'r': strResult += '\r'; break;
              case 't': strResult += '\t'; break;
              case 'u':
                if (i < source.size() - 3)
                  {
                    bool bRowOk = false, bCellOk = false;
                    uchar ucRow = (uchar)source.mid(i,2).toUInt(&bRowOk, 16);
                    uchar ucCell = (uchar)source.mid(i+2,2).toUInt(&bCellOk, 16);
                    if (bRowOk && bCellOk)
                      {
                        strResult += QChar(ucCell, ucRow);
                        i += 3;
                        break;
                      }
                  }
              default: strResult += QChar(iChar); break;
              }
          }
        else
          strResult += QChar(iChar);
      }
    return strResult;
  }

  QVariant unescapeVariant(const QString& value)
  {
    QVariant result;
    if (value.size() == 0)
      return result;
    // Integers
    else if (value.contains(QRegExp("^[0-9]+$")))
      {
        bool bOk = false;
        int iValue = value.toInt(&bOk);
        if (bOk)
          return iValue;
        qlonglong llValue = value.toLongLong(&bOk);
        if (bOk)
          return llValue;
        qulonglong ullValue = value.toULongLong(&bOk);
        if (bOk)
          return ullValue;
      }
    // Double
    else if (strchr("0123456789.-", value[0].toLatin1()))
      {
        bool bOk = false;
        double dValue = value.toDouble(&bOk);
        if (bOk)
          return dValue;
      }
    // Boolean values
    else if (value == "true")
      return true;
    else if (value == "false")
      return false;

    // The default is a string
    int iSize = value.size();
    if (value.size() >= 2 && value[0] == '"' && value[iSize-1] == '"')
      return unescapeString(value.mid(1, iSize-2));
    return unescapeString(value);
  }
}
