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

#ifndef _PIISCRIPTEXTENSIONPLUGIN_H
#define _PIISCRIPTEXTENSIONPLUGIN_H

#include <QScriptExtensionPlugin>

class PiiScriptExtensionPlugin : public QScriptExtensionPlugin
{
  Q_OBJECT
#if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "com.intopii.Into.PiiScriptExtensionPlugin" FILE "piiscriptextensionplugin.json")
#endif
  
public:
  void initialize(const QString& key, QScriptEngine* engine);
  QStringList	keys() const;
};

#endif //_PIISCRIPTEXTENSIONPLUGIN_H
