// -*- mode: c++ -*-

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

#ifndef __WMTS_PLUGIN_MANAGER_H__
#define __WMTS_PLUGIN_MANAGER_H__

/**************************************************************************************************/

#include <QObject>
#include <QHash>

#include "wmts/wmts_plugin.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcWmtsPluginManager : QObject
{
public:
  static QcWmtsPluginManager & instance() {
    // Thread-safe in C++11
    static QcWmtsPluginManager m_instance;
    return m_instance;
  }

  // Delete copy and move constructors and assign operators
  QcWmtsPluginManager(QcWmtsPluginManager const &) = delete;             // Copy construct
  QcWmtsPluginManager & operator=(QcWmtsPluginManager const &) = delete; // Copy assign
  QcWmtsPluginManager(QcWmtsPluginManager &&) = delete;                  // Move construct
  QcWmtsPluginManager & operator=(QcWmtsPluginManager &&) = delete;      // Move assign

  QList<QcWmtsPlugin *> plugins() const { return m_plugins.values(); }
  const QList<QString> & plugin_names() const { return m_plugin_names; }

  QcWmtsPlugin * operator[](const QString & name);

private:
  QcWmtsPluginManager();
  ~QcWmtsPluginManager();

  QcWmtsPlugin * create_plugin_geoportail();

private:
  QList<QString> m_plugin_names;
  QHash<QString, QcWmtsPlugin *> m_plugins;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __WMTS_PLUGIN_MANAGER_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
