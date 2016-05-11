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

#include "wmts_plugin.h"

/**************************************************************************************************/

QcWmtsPlugin::QcWmtsPlugin(const QString & name, size_t number_of_levels, size_t tile_size)
  : m_name(name),
    m_tile_matrix_set(name, number_of_levels, tile_size),
    m_wmts_manager(name)
{}

QcWmtsPlugin::~QcWmtsPlugin()
{}

QcWmtsPluginMap
QcWmtsPlugin::plugin_map(int map_id) // const
{
  if (is_valid_map_id(map_id))
    return QcWmtsPluginMap(this, map_id);
  else
    throw std::invalid_argument("invalid map id");
}

/**************************************************************************************************/

QcWmtsPluginMap::QcWmtsPluginMap(QcWmtsPlugin * plugin, int map_id)
  : m_plugin(plugin),
    m_map_id(map_id)
{}

QcWmtsPluginMap::QcWmtsPluginMap(const QcWmtsPluginMap & other)
  : m_plugin(other.m_plugin),
    m_map_id(other.m_map_id)
{}

QcWmtsPluginMap::~QcWmtsPluginMap()
{}

QcWmtsPluginMap &
QcWmtsPluginMap::operator=(const QcWmtsPluginMap & other)
{
  if (this != &other) {
    m_plugin = other.m_plugin;
    m_map_id = other.m_map_id;
  }

  return *this;
}

QString
QcWmtsPluginMap::name() const
{
  return m_plugin->name() + '/' + QString::number(m_map_id);
}

/**************************************************************************************************/

// #include "wmts_plugin.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
