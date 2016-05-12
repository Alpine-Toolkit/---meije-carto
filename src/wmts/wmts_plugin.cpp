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

QcWmtsPluginLayer::QcWmtsPluginLayer(QcWmtsPlugin * plugin,
                                     int map_id,
                                     int position,
                                     const QString & title,
                                     const QString & name,
                                     const QString & image_format)
  : m_plugin(plugin),
    m_map_id(map_id),
    m_position(position),
    m_title(title),
    m_name(name),
    m_image_format(image_format)
{}

QcWmtsPluginLayer::QcWmtsPluginLayer(const QcWmtsPluginLayer & other)
  : m_plugin(other.m_plugin),
    m_map_id(other.m_map_id),
    m_position(other.m_position),
    m_title(other.m_title),
    m_name(other.m_name),
    m_image_format(other.m_image_format)
{}

QcWmtsPluginLayer::~QcWmtsPluginLayer()
{}

QcWmtsPluginLayer &
QcWmtsPluginLayer::operator=(const QcWmtsPluginLayer & other)
{
  if (this != &other) {
    m_plugin = other.m_plugin;
    m_map_id = other.m_map_id;
    m_position = other.m_position;
    m_title = other.m_title;
    m_name = other.m_name;
    m_image_format = other.m_image_format;
  }

  return *this;
}

QString
QcWmtsPluginLayer::hash_name() const
{
  return m_plugin->name() + '/' + QString::number(m_map_id);
}

QcTileSpec
QcWmtsPluginLayer::create_tile_spec(int level, int x, int y) const
{
  return m_plugin->create_tile_spec(m_map_id, level, x, y);
}

/**************************************************************************************************/

QcWmtsPlugin::QcWmtsPlugin(const QString & name, size_t number_of_levels, size_t tile_size)
  : m_name(name),
    m_tile_matrix_set(name, number_of_levels, tile_size),
    m_wmts_manager(name)
{}

QcWmtsPlugin::~QcWmtsPlugin()
{}

void
QcWmtsPlugin::add_layer(const QcWmtsPluginLayer * layer)
{
  // Fixme: check for errors
  m_layers << layer;
  m_layer_map.insert(layer->map_id(), layer);
}

const QcWmtsPluginLayer *
QcWmtsPlugin::layer(int map_id) const
{
  return m_layer_map.value(map_id, nullptr);
}

const QcWmtsPluginLayer *
QcWmtsPlugin::layer(const QcTileSpec & tile_spec) const
{
  return m_layer_map.value(tile_spec.map_id(), nullptr);
}

const QcWmtsPluginLayer *
QcWmtsPlugin::layer(const QString & title) const
{
  for (const auto * _layer : m_layers)
    if (_layer->title() == title)
      return _layer;
  return nullptr;
}

QUrl
QcWmtsPlugin::make_layer_url(const QcTileSpec & tile_spec) const
{
  // Fixme: error
  return layer(tile_spec)->url(tile_spec);
}

/**************************************************************************************************/

// #include "wmts_plugin.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
