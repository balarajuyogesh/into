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

#include "PiiLog.h"
#include <QDateTime>
#include <QFile>
#include <QRegExp>
#include <QStringList>
#include <QMutex>
#include <cstdlib>

namespace PiiLog
{
  static MessageFilter pLogMessageFilter = defaultMessageFilter;
  static QString strMessageFormat;
  static QString strLogFile;
  static qint64 iMaxFileSize = 1024*1024;
  static int iMaxArchivedFiles = 5;

  static void rotateLog()
  {
    for (int i=iMaxArchivedFiles; i>=0; --i)
      {
        // Add numeric suffix to file name as needed.
        QFile file(i == 0 ? strLogFile : QString("%1.%2").arg(strLogFile).arg(i));
        if (file.exists())
          {
            // This is the last one -> get rid of it
            if (i == iMaxArchivedFiles)
              file.remove();
            // Move log.N to log.N+1
            else
              file.rename(QString("%1.%2").arg(strLogFile).arg(i+1));
          }
      }
  }

  bool defaultMessageFilter(const char* /*module*/, QtMsgType level)
  {
    static struct LogEnv
    {
      LogEnv()
      {
        char* pLogLevel = getenv("PII_LOG_LEVEL");
        level = pLogLevel == 0  ? 0 : *pLogLevel - '0';
      }
      int level;
    } logEnv;
    return int(level) >= logEnv.level;
  }

  void ignoreMessage(QtMsgType, const char*) {}
  
  void writeToFile(QtMsgType, const char* msg)
  {
    static QMutex fileLock;
    QMutexLocker lock(&fileLock);
    if (strLogFile.isEmpty())
      return;
    QFile file(strLogFile);
    if (file.exists() && file.size() >= iMaxFileSize)
      rotateLog();
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
      return;
    file.write(msg);
    file.putChar('\n');
    file.close();
  }

  MessageFilter setLogMessageFilter(MessageFilter filter)
  {
    MessageFilter pOldFilter = pLogMessageFilter;
    pLogMessageFilter = filter;
    return pOldFilter;
  }

  void setLogFormat(const QString& format)
  {
    strMessageFormat = format;
  }

  QString logFormat()
  {
    return strMessageFormat;
  }

  void setLogFile(const QString& file)
  {
    strLogFile = file;
  }

  QString logFile()
  {
    return strLogFile;
  }

  void setMaxFileSize(qint64 maxSize)
  {
    iMaxFileSize = maxSize;
  }

  qint64 getMaxFileSize()
  {
    return iMaxFileSize;
  }

  void setMaxArchivedFiles(int maxCount)
  {
    iMaxArchivedFiles = maxCount;
  }

  int getMaxArchivedFiles()
  {
    return iMaxArchivedFiles;
  }
}

static void outputMessage(QtMsgType level, const QString& message)
{
#if QT_VERSION < 0x050000
  qt_message_output(level, message.toLocal8Bit().constData());
#else
  qt_message_output(level, QMessageLogContext(), message);
#endif
}

void piiLogv(const char* module, QtMsgType level, const char* msg, va_list argp)
{
  // PENDING should use Pii::replaceVariables() here (DRY)
  static const QString aTypes[] = { "Debug", "Warning", "Critical", "Fatal" };
  static const QString strDefaultDateFormat("yyyy-MM-dd hh:mm");

  if (PiiLog::pLogMessageFilter != 0 && !(*PiiLog::pLogMessageFilter)(module, level))
    return;
  
  QString strMessage;
  if (msg != 0)
    strMessage.vsprintf(msg, argp);

  if (PiiLog::strMessageFormat.isEmpty())
    outputMessage(level, strMessage);
  else
    {
      QString strLogLine(PiiLog::strMessageFormat);
      QRegExp reVariable("\\$((\\w+)|\\{(\\w+)\\}|\\{(\\w+) ([^}]+)\\})");
      QStringList lstVariables = QStringList() << "time" << "type" << "module" << "message";
      int index = 0;
      while ((index = reVariable.indexIn(strLogLine, index)) != -1)
        {
          QString strVarName, strParams, strReplacement;

          if (!reVariable.cap(2).isEmpty()) // no curly braces
            strVarName = reVariable.cap(2);
          else if (!reVariable.cap(3).isEmpty()) // curly braces
            strVarName = reVariable.cap(3);
          else // name with parameters
            {
              strVarName = reVariable.cap(4);
              strParams = reVariable.cap(5);
            }
          switch (lstVariables.indexOf(strVarName))
            {
            case 0: // time
              strReplacement = QDateTime::currentDateTime().toString(strParams.isEmpty() ? strDefaultDateFormat : strParams);
              break;
            case 1: // type
              strReplacement = aTypes[qBound(0, int(level), 3)].left(strParams.isEmpty() ? -1 : strParams.toInt());
              break;
            case 2: // module
              strReplacement = module;
              break;
            case 3: // message
              strReplacement = strMessage;
              break;
            }
          strLogLine.replace(index, reVariable.matchedLength(), strReplacement);
          index += strReplacement.size();
        }
      outputMessage(level, strLogLine);
    }
}
