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

#ifndef _PIIREGEXPMATCHER_H
#define _PIIREGEXPMATCHER_H

#include <PiiDefaultOperation.h>
#include <QRegExp>

/**
 * An operation that splits strings into pieces based on a regular
 * expression.
 *
 * @inputs
 *
 * @in input - input text (QString)
 *
 * @outputs
 *
 * @out outputX - matched texts. X is a zero-based index. The first
 * output, @p output0 will emit the full matched text. The other
 * outputs will emit the matched subexpressions, if any. The number of
 * outputs matches the number of subexpressions in #pattern, plus one.
 *
 * @code
 * PiiOperation* pRe = engine.createOperation("PiiRegExpMatcher");
 * // Separate path from file name. Output1 will emit the path
 * // without a trailing slash, and output2 the file name part.
 * pRe->setProperty("pattern", "(^.*)/([^/]+$)
 * @endcode
 *
 * @ingroup PiiBasePlugin
 */
class PiiRegExpMatcher : public PiiDefaultOperation
{
  Q_OBJECT
  /**
   * The regular expression to be matched against @p input. See
   * QRegExp for syntax of valid patterns. Setting this property
   * changes the number of outputs to match the number of
   * subexpressions (plus one).
   */
  Q_PROPERTY(QString pattern READ pattern WRITE setPattern);
   
  PII_OPERATION_SERIALIZATION_FUNCTION
public:
  PiiRegExpMatcher();

  void setPattern(const QString& pattern);
  QString pattern() const;

protected:
  void process();

private:
  /// @internal
  class Data : public PiiDefaultOperation::Data
  {
  public:
    QRegExp re;
  };
  PII_D_FUNC;
};

#endif //_PIIREGEXPMATCHER_H
