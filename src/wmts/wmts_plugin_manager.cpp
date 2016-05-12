/***************************************************************************************************
**
** $QTCARTO_BEGIN_LICENSE:GPL3$
**
** Copyright (C) 2016 Fabrice Salvaire
** Contact: http://www.fabrice-salvaire.fr
**
** This file is part of the QtCarto library.
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
** $QTCARTO_END_LICENSE$
**
***************************************************************************************************/

/**************************************************************************************************/

#include "wmts_plugin_manager.h"

#include "config.h"
#include "geoportail/geoportail_plugin.h"
#include "osm/osm_plugin.h"

#include <QtDebug>

/**************************************************************************************************/

QcWmtsPluginManager::QcWmtsPluginManager()
{
  // Fixme: plugin name versus title ?
  m_plugin_names << QcGeoportailPlugin::PLUGIN_NAME
                 << QcOsmPlugin::PLUGIN_NAME;
}

QcWmtsPluginManager::~QcWmtsPluginManager()
{}

QcWmtsPlugin *
QcWmtsPluginManager::operator[](const QString & name)
{
  if (m_plugins.contains(name))
    return m_plugins[name];
  else {
    // Fixme:: use Qt Meta
    // int QMetaObject::indexOfMethod(const char *method) const
    // bool QMetaObject::invokeMethod(QObject *obj, const char *member
    QcWmtsPlugin * plugin = nullptr;
    if (name == QcGeoportailPlugin::PLUGIN_NAME)
      plugin = create_plugin_geoportail();
    else if (name == QcOsmPlugin::PLUGIN_NAME)
      plugin = create_plugin_osm();

    if (plugin)
      m_plugins.insert(plugin->name(), plugin);

    return plugin;
  }
}

QcWmtsPlugin *
QcWmtsPluginManager::create_plugin_geoportail()
{
  QcConfig & config = QcConfig::instance();

  // Fixme: Hide license
  QString json_path = QDir(config.application_user_directory()).filePath(QLatin1Literal("geoportail-license.json"));
  qInfo() << "Geoportail license json" << json_path;
  QcGeoportailWmtsLicense geoportail_license(json_path);

  return new QcGeoportailPlugin(geoportail_license);
}

QcWmtsPlugin *
QcWmtsPluginManager::create_plugin_osm()
{
  return new QcOsmPlugin();
}

/**************************************************************************************************/

// #include "wmts_plugin_manager.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
