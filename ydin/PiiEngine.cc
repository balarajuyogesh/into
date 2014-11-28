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

#include <PiiSerializationUtil.h>
#include "PiiEngine.h"
#include <PiiSerializableExport.h>
#include <PiiUtil.h>
#include <PiiFileUtil.h>
#include "PiiPlugin.h"
#include <PiiGenericTextOutputArchive.h>
#include <PiiGenericBinaryOutputArchive.h>
#include <PiiGenericTextInputArchive.h>
#include <PiiGenericBinaryInputArchive.h>

#include <QLibrary>
#include <QFile>
#include <QFileInfo>

PII_DEFINE_VIRTUAL_METAOBJECT_FUNCTION(PiiEngine)
PII_SERIALIZABLE_EXPORT(PiiEngine);

static int iEngineMetaType = qRegisterMetaType<PiiEngine*>("PiiEngine*");
static int iPluginMetaType = qRegisterMetaType<PiiEngine::Plugin>("PiiEngine::Plugin");

PiiEngine::PluginMap PiiEngine::_pluginMap;
QMutex PiiEngine::_pluginLock;

class PiiEngine::Plugin::Data
{
public:
  Data();
  Data(QLibrary* lib, const QString& name, const PiiVersionNumber& version, int iRefCount = 1);
  Data(const Data& other);

  QLibrary* pLibrary;
  QString strResourceName, strLibraryName;
  PiiVersionNumber version;
  int iRefCount;
} *d;


PiiEngine::PiiEngine()
{
  Q_UNUSED(iEngineMetaType); // suppresses compiler warning
  Q_UNUSED(iPluginMetaType);
#ifdef __MINGW32__
  /* HACK
   * Mingw exception handling is not thread safe. The first exception
   * thrown makes one-time initialization that occassionally crashes
   * the program, if many exceptions are thrown simultaneously from
   * different threads.
   */
  try { throw PiiException(); } catch (...) {}
#endif
}

PiiEngine::PiiEngine(Data* data) : PiiOperationCompound(data)
{}

PiiEngine::~PiiEngine()
{}

void PiiEngine::execute(ErrorHandling errorHandling)
{
  PII_D;
  QMutexLocker lock(&d->stateMutex);
  State s = state();
  if (s member_of (Stopped, Paused))
    {
      try
        {
          // Reset children if we were stopped
          check(s == Stopped);
        }
      catch (...)
        {
          if (errorHandling == ThrowOnError)
            throw;
        }
      start();
    }
}

void PiiEngine::loadPlugins(const QStringList& plugins)
{
  for (int i=0; i<plugins.size(); ++i)
    loadPlugin(plugins[i]);
}

static QString pluginNameToPath(const QString& name)
{
  if (PiiEngine::pluginPath().isEmpty() || QFileInfo(name).isAbsolute())
    return name;
  return Pii::fixPath(PiiEngine::pluginPath()) + name;
}

PiiEngine::Plugin PiiEngine::loadPlugin(const QString& name)
{
  QMutexLocker lock(&_pluginLock);

  if (_pluginMap.contains(name))
    {
      // The plug-in is already loaded. Just increase reference count.
      ++_pluginMap[name].d->iRefCount;
      return _pluginMap[name];
    }

  // Exception safety
  struct Unloader
  {
    Unloader(QLibrary* lib) : pLib(lib) {}
    ~Unloader()
    {
      if (pLib)
        {
          pLib->unload();
          delete pLib;
        }
    }
    QLibrary* release()
    {
      QLibrary* pTmp = pLib;
      pLib = 0;
      return pTmp;
    }

    QLibrary* pLib;
  };

  // Load library
  Unloader unloader(new QLibrary(pluginNameToPath(name)));

  unloader.pLib->setLoadHints(QLibrary::ExportExternalSymbolsHint);
  if (!unloader.pLib->load())
    PII_THROW(PiiLoadException, tr("Cannot load the shared library \"%1\".\n"
                                   "Error message: %2").arg(name).arg(unloader.pLib->errorString()));

  // Resolve plug-in information functions
  pii_plugin_function pNameFunc = (pii_plugin_function)(unloader.pLib->resolve(PII_PLUGIN_NAME_FUNCTION_STR));
  if (pNameFunc == 0)
    PII_THROW(PiiLoadException, tr("The shared library \"%1\" does not contain a valid plug-in. Missing plug-in name function.").arg(name));

  pii_plugin_function pVersionFunc = (pii_plugin_function)(unloader.pLib->resolve(PII_PLUGIN_VERSION_FUNCTION_STR));
  if (pVersionFunc == 0)
    PII_THROW(PiiLoadException, tr("The shared library \"%1\" does not contain a valid plug-in. Missing plug-in version function.").arg(name));

  pii_plugin_init_function pInitFunc = (pii_plugin_init_function)(unloader.pLib->resolve(PII_PLUGIN_INIT_FUNCTION_STR));
  if (pInitFunc && !(*pInitFunc)())
    PII_THROW(PiiLoadException, tr("Initialization of the shared library \"%1\" failed.").arg(name));

  PiiVersionNumber pluginVersion((*pVersionFunc)());
  PiiVersionNumber intoVersion(INTO_VERSION_STR);

  if (intoVersion < pluginVersion)
    PII_THROW(PiiLoadException, tr("The plug-in \"%1\" is compiled against a newer version of Into (%2, current is %3).")
              .arg(name)
              .arg(pluginVersion.toString())
              .arg(INTO_VERSION_STR));

  if (pluginVersion.part(0) < intoVersion.part(0))
    PII_THROW(PiiLoadException, tr("The plug-in is not binary compatible with your version of Into."));

  unloader.pLib->setObjectName(name);
  Plugin plugin(unloader.release(), (*pNameFunc)(), pluginVersion);
  _pluginMap.insert(name, plugin);

  return plugin;
}

int PiiEngine::unloadPlugin(const QString& name, bool force)
{
  QMutexLocker lock(&_pluginLock);

  // Cannot unload a non-loaded plug-in.
  if (!_pluginMap.contains(name))
    return 0;
  if (!force &&
      --_pluginMap[name].d->iRefCount > 0)
    // Still some references left...
    return _pluginMap[name].d->iRefCount;

  // Remove the plug-in from our map.
  Plugin plugin = _pluginMap.take(name);
  plugin.d->pLibrary->unload();
  delete plugin.d->pLibrary;
  return 0;
}

bool PiiEngine::isLoaded(const QString& name)
{
  QMutexLocker lock(&_pluginLock);
  return _pluginMap.contains(QFileInfo(name).baseName());
}

QList<PiiEngine::Plugin> PiiEngine::plugins()
{
  QMutexLocker lock(&_pluginLock);
  QList<Plugin> lstPlugins;

  for (PluginMap::const_iterator i = _pluginMap.constBegin(); i != _pluginMap.constEnd(); ++i)
    lstPlugins << i.value();

  return lstPlugins;
}

QStringList PiiEngine::pluginLibraryNames()
{
  QMutexLocker lock(&_pluginLock);
  return _pluginMap.keys();
}

QStringList PiiEngine::pluginResourceNames()
{
  QMutexLocker lock(&_pluginLock);
  QStringList lstResult;
  for (PluginMap::const_iterator i = _pluginMap.constBegin(); i != _pluginMap.constEnd(); ++i)
    lstResult << i.value().resourceName();
  return lstResult;
}

QStringList PiiEngine::usedPluginLibraryNames()
{
  return usedPluginLibraryNames(this);
}

QStringList PiiEngine::usedPluginLibraryNames(PiiOperation* operation)
{
  QMutexLocker lock(&_pluginLock);

  QStringList lstPlugins;
  foreach (QString strResourceName, usedPluginResourceNames(operation))
    {
      for (PluginMap::const_iterator i = _pluginMap.constBegin(); i != _pluginMap.constEnd(); ++i)
        {
          if (i.value().resourceName() == strResourceName)
            {
              lstPlugins << i.key();
              break;
            }
        }
    }
  return lstPlugins;
}

QStringList PiiEngine::usedPluginResourceNames()
{
  return usedPluginResourceNames(this);
}

QString PiiEngine::operationsUsedPlugin(PiiOperation* operation)
{
  // In the resource database, the parent resource of an operation
  // is the plugin it came from.
  // Find all objects that satisfy <"OperationName", "pii::parent", object>
  QList<QString> lstPlugins = PiiYdin::resourceDatabase()->
    select(Pii::object,
           Pii::subject == PiiYdin::resourceName(operation) &&
           Pii::predicate == PiiYdin::parentPredicate);
  if (!lstPlugins.isEmpty())
    return lstPlugins[0];
  return QString();
}


QStringList PiiEngine::compoundsUsedPlugins(PiiOperationCompound* compound)
{
  // Scan all child operations...
  QList<PiiOperation*> lstOperations(compound->findChildren<PiiOperation*>());
  // ... and the compound itself.
  lstOperations.append(compound);
  QStringList lstPlugins;
  for (int i = 0; i < lstOperations.size(); ++i)
    {
      QString strPlugin(operationsUsedPlugin(lstOperations[i]));
      if (!strPlugin.isEmpty() && !lstPlugins.contains(strPlugin))
        lstPlugins.append(strPlugin);
    }
  return lstPlugins;
}

QStringList PiiEngine::usedPluginResourceNames(PiiOperation* operation)
{
  if (operation->isCompound())
    return compoundsUsedPlugins(static_cast<PiiOperationCompound*>(operation));
  QString strPlugin(operationsUsedPlugin(operation));
  QStringList lstPlugins;
  if (!strPlugin.isEmpty())
    lstPlugins.append(strPlugin);
  return lstPlugins;
}

PiiEngine* PiiEngine::clone() const
{
  PiiEngine *pResult = static_cast<PiiEngine*>(PiiOperationCompound::clone());

  // Set properties if not derived
  if (pResult != 0 && PiiEngine::metaObject() == metaObject())
    Pii::setProperties(pResult, Pii::propertyList(this, 0, Pii::WritableProperties | Pii::DynamicProperties));

  return pResult;
}

void PiiEngine::save(const QString& fileName,
                     const QVariantMap& config,
                     FileFormat format) const
{
  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly))
    PII_THROW(PiiException, tr("Cannot open %1 for writing.").arg(fileName));

  QVariantMap mapConfig(config);

  if (!mapConfig.contains("application"))
    {
      mapConfig["application"] = "Into";
      if (!mapConfig.contains("version"))
        mapConfig["version"] = INTO_VERSION_STR;
    }
  if (!mapConfig.contains("plugins"))
    mapConfig["plugins"] = pluginLibraryNames();

  if (format == TextFormat)
    {
      PiiGenericTextOutputArchive oa(&file);
      oa << PII_NVP("config", mapConfig);
      oa << PII_NVP("engine", this);
    }
  else
    {
      PiiGenericBinaryOutputArchive oa(&file);
      oa << PII_NVP("config", mapConfig);
      oa << PII_NVP("engine", this);
    }
}

void PiiEngine::ensurePlugin(const QString& plugin)
{
  if (!isLoaded(plugin))
    PiiEngine::loadPlugin(plugin);
}

void PiiEngine::ensurePlugins(const QStringList& plugins)
{
  foreach (QString strPlugin, plugins)
    if (!isLoaded(strPlugin))
      PiiEngine::loadPlugin(strPlugin);
}

PiiEngine* PiiEngine::load(const QString& fileName,
                           QVariantMap* config)
{
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly))
    PII_THROW(PiiException, tr("Cannot open %1 for reading.").arg(fileName));
  PiiEngine* pEngine = 0;
  QVariantMap mapConfig;
  if (file.peek(PII_TEXT_ARCHIVE_ID_LEN) == PII_TEXT_ARCHIVE_ID)
    {
      PiiGenericTextInputArchive ia(&file);
      ia >> PII_NVP("config", mapConfig);
      ensurePlugins(mapConfig["plugins"].toStringList());
      ia >> PII_NVP("engine", pEngine);
    }
  else if (file.peek(PII_BINARY_ARCHIVE_ID_LEN) == PII_BINARY_ARCHIVE_ID)
    {
      PiiGenericBinaryInputArchive ia(&file);
      ia >> PII_NVP("config", mapConfig);
      ensurePlugins(mapConfig["plugins"].toStringList());
      ia >> PII_NVP("engine", pEngine);
    }
  else
    PII_SERIALIZATION_ERROR(UnrecognizedArchiveFormat);

  if (config != 0)
    *config = mapConfig;

  return pEngine;
}

static QString* pluginPathPtr()
{
  static QString strPluginPath;
  return &strPluginPath;
}

void PiiEngine::setPluginPath(const QString& path)
{
  *pluginPathPtr() = path;
}

QString PiiEngine::pluginPath()
{
  return *pluginPathPtr();
}

PiiEngine::Plugin::Data::Data() : pLibrary(0), iRefCount(0)
{
}

PiiEngine::Plugin::Data::Data(QLibrary* lib, const QString& name, const PiiVersionNumber& v, int refCount) :
  pLibrary(lib), strResourceName(name), strLibraryName(lib->objectName()), version(v), iRefCount(refCount)
{
}

PiiEngine::Plugin::Data::Data(const Data& other) :
  pLibrary(other.pLibrary),
  strResourceName(other.strResourceName),
  strLibraryName(other.strLibraryName),
  version(other.version),
  iRefCount(other.iRefCount)
{
}

PiiEngine::Plugin::Plugin() : d(new Data)
{
}

PiiEngine::Plugin::Plugin(QLibrary* lib, const QString& name, const PiiVersionNumber& version) :
  d(new Data(lib, name, version))
{
}

PiiEngine::Plugin::Plugin(const Plugin& other) :
  d(new Data(*other.d))
{
}

PiiEngine::Plugin& PiiEngine::Plugin::operator= (const PiiEngine::Plugin& other)
{
  if (d != other.d)
    {
      d->pLibrary = other.d->pLibrary;
      d->strResourceName = other.d->strResourceName;
      d->strLibraryName = other.d->strLibraryName;
      d->version = other.d->version;
      d->iRefCount = other.d->iRefCount;
    }
  return *this;
}

bool PiiEngine::Plugin::operator== (const Plugin& other) const
{
  return d->strResourceName == other.d->strResourceName &&
    d->version == other.d->version;
}

PiiEngine::Plugin::~Plugin()
{
  delete d;
}

QString PiiEngine::Plugin::resourceName() const
{
  return d->strResourceName;
}

QString PiiEngine::Plugin::libraryName() const
{
  return d->strLibraryName;
}

PiiVersionNumber PiiEngine::Plugin::version() const
{
  return d->version;
}
