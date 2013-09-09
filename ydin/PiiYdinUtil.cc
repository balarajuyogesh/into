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

#include "PiiYdinUtil.h"
#include "PiiOperation.h"
#include "PiiYdinTypes.h"

namespace PiiYdin
{
  static QString dumpInputs(PiiAbstractOutputSocket* socket)
  {
    QString strResult;
    QList<PiiAbstractInputSocket*> lstInputs = socket->connectedInputs();
    for (int i=0; i<lstInputs.size(); ++i)
      {
        if (i > 0)
          strResult += " ";
        PiiOperation* pParent = Pii::findFirstParent<PiiOperation*>(lstInputs[i]->socket());
        if (pParent != 0)
          strResult += QString("%1.%2").arg(pParent->objectName(),
                                            pParent->socketName(lstInputs[i]));
        else
          strResult += "<unknown>." + lstInputs[i]->socket()->objectName();
      }
    return strResult;
  }

  static char symbolFor(const PiiVariant& obj)
  {
    switch (obj.type())
      {
      case PiiVariant::InvalidType: return '0';
      case PiiYdin::SynchronizationTagType: return obj.valueAs<int>() > 0 ? '>' : '<';
      case PiiYdin::StopTagType: return 'S';
      case PiiYdin::PauseTagType: return 'P';
      case PiiYdin::ResumeTagType: return 'R';
      default: return '.';
      }
  }

  QString illustrateOperation(PiiOperation* op, IllustrationFlags flags)
  {
    QList<PiiAbstractInputSocket*> inputs = op->inputs();
    QList<PiiAbstractOutputSocket*> outputs = op->outputs();

    QString strTitle(op->metaObject()->className());
    QString strName;
    if (!op->objectName().isEmpty() && strTitle != op->objectName())
      strName = QString("(%1)").arg(op->objectName());

    int iMaxSize = qMax(strTitle.size(), strName.size());
    int iMaxQueueLength = 0;
    for (int i=0; i<inputs.size(); i++)
      {
        PiiInputSocket* pSocket = static_cast<PiiInputSocket*>(inputs[i]->socket());
        QString name = op->socketName(inputs[i]);
        if (name.size() > iMaxSize)
          iMaxSize = name.size();
        if (pSocket != 0 && pSocket->queueLength() > iMaxQueueLength)
          iMaxQueueLength = pSocket->queueLength();
      }
    ++iMaxQueueLength;
    
    for (int i=0; i<outputs.size(); i++)
      {
        QString name = op->socketName(outputs[i]);
        if (name.size() > iMaxSize)
          iMaxSize = name.size();
      }

    QString result;
    if (flags & ShowState)
      {
        QString strState = PiiOperation::stateName(op->state());
        if (strState.size() > iMaxSize)
          iMaxSize = strState.size();
        result = QString("%0%1\n").arg("", iMaxQueueLength+2).arg(strState);
      }

    result += QString("%0+%1+\n%0| %2 |\n").
      arg("", iMaxQueueLength).
      arg('-', iMaxSize+3, QChar('-')).
      arg(strTitle, -(iMaxSize+1));

    if (strName.size() > 0)
      result += QString("%0| %1 |\n").arg("", iMaxQueueLength).arg(strName, -(iMaxSize+1));

    result += QString("%0+%1+\n").arg("", iMaxQueueLength).arg('-', iMaxSize+3, QChar('-'));

    for (int i=0; i<inputs.size(); i++)
      {
        PiiInputSocket* pSocket = static_cast<PiiInputSocket*>(inputs[i]->socket());
        QString name = op->socketName(inputs[i]);
        if ((flags & ShowInputQueues) &&
            pSocket != 0 &&
            pSocket->isConnected())
          {
            for (int j=pSocket->queueLength(); j<iMaxQueueLength-1; ++j)
              result += ' ';
            for (int j=pSocket->queueLength(); j--; )
              result += symbolFor(pSocket->queuedObject(j));
            result += symbolFor(pSocket->firstObject());
          }
        else
          result += QString("%1").arg("", iMaxQueueLength);
        result += QString("|> %1 |\n").arg(name, -iMaxSize);
      }
    for (int i=0; i<outputs.size(); i++)
      {
        QString name = op->socketName(outputs[i]);
        result += QString("%0| %1 >|").arg("", iMaxQueueLength).arg(name, iMaxSize);
        if (flags & ShowOutputStates)
          result += " " + dumpInputs(outputs[i]);
        result += '\n';
      }
    result += QString("%0+%1+\n").arg("", iMaxQueueLength).arg('-', iMaxSize+3, QChar('-'));
    return result;
  }
  
  void dumpOperation(PiiOperation *op, IllustrationFlags flags, int level)
  {
    PiiOperationCompound* compound = qobject_cast<PiiOperationCompound*>(op);
    QString padding;
    padding.fill(' ', level*2);
    if (compound != 0)
      {
        QString strHeader = QString("%0 (%1)").arg(compound->metaObject()->className(), compound->objectName());
        if (flags & ShowState)
          strHeader += QString(" ") + PiiOperation::stateName(op->state());

        qDebug("%s>>>>>>>> %s\n", qPrintable(padding), qPrintable(strHeader));
        
        QList<PiiOperation*> kids = compound->childOperations();
        for (int i=0; i<kids.size(); ++i)
          dumpOperation(kids[i], flags, level + 1);
        
        qDebug("%s<<<<<<<< %s\n", qPrintable(padding), qPrintable(strHeader));
      }
    else
      {
        QString strIllustration = illustrateOperation(op, flags);
        strIllustration = padding + strIllustration.replace("\n", "\n" + padding);
        qDebug("%s", strIllustration.toUtf8().constData());
      }
  }
}
