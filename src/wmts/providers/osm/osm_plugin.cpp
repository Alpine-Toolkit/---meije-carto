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

#include "osm_plugin.h"

/**************************************************************************************************/

QcOsmLayer::QcOsmLayer(QcOsmPlugin * plugin,
                       int map_id,
                       int position,
                       const QString & title,
                       const QString & name,
                       const QString & image_format,
                       const QString & base)
  : QcWmtsPluginLayer(plugin, map_id, position, title, name, image_format),
    m_base(base)
{}

QcOsmLayer::QcOsmLayer(const QcOsmLayer & other)
  : QcWmtsPluginLayer(other),
    m_base(other.m_base)
{}

QcOsmLayer::~QcOsmLayer()
{}

QcOsmLayer &
QcOsmLayer::operator=(const QcOsmLayer & other)
{
  if (this != &other) {
    QcWmtsPluginLayer::operator=(other);
    m_base = other.m_base;
  }

  return *this;
}

QUrl
QcOsmLayer::url(const QcTileSpec & tile_spec) const
{
  return QUrl(QStringLiteral("http://") +
              m_base  + QLatin1Char('/') +
              QString::number(tile_spec.level()) + QLatin1Char('/') +
              QString::number(tile_spec.x()) + QLatin1Char('/') +
              QString::number(tile_spec.y()) +
              QLatin1Char('.') + image_format());
}

/**************************************************************************************************/

const QString QcOsmPlugin::PLUGIN_NAME = "osm";
const QString PLUGIN_TITLE = "Open Street Map";
constexpr int NUMBER_OF_LEVELS = 20;
constexpr int TILE_SIZE = 256;

QcOsmPlugin::QcOsmPlugin()
  : QcWmtsPlugin(PLUGIN_NAME, PLUGIN_TITLE,
                 new QcMercatorTileMatrixSet(NUMBER_OF_LEVELS, TILE_SIZE))
{
  int map_id = -1;
  add_layer(new QcOsmLayer(this,
                           ++map_id, // 1
                           1,
                           QLatin1Literal("Map"),
                           QLatin1Literal("map"),
                           QLatin1Literal("jpg"),
                           QLatin1Literal("otile1.mqcdn.com/tiles/1.0.0/map")
                           ));
  add_layer(new QcOsmLayer(this,
                           ++map_id, // 2
                           2,
                           QLatin1Literal("Satellite"),
                           QLatin1Literal("satellite"),
                           QLatin1Literal("jpg"),
                           QLatin1Literal("otile1.mqcdn.com/tiles/1.0.0/sat")
                           ));
  add_layer(new QcOsmLayer(this,
                           ++map_id, // 3
                           3,
                           QLatin1Literal("Cycle"),
                           QLatin1Literal("cycle"),
                           QLatin1Literal("png"),
                           QLatin1Literal("a.tile.thunderforest.com/cycle")
                           ));
  add_layer(new QcOsmLayer(this,
                           ++map_id, // 4
                           4,
                           QLatin1Literal("Transport"),
                           QLatin1Literal("transport"),
                           QLatin1Literal("png"),
                           QLatin1Literal("a.tile.thunderforest.com/transport")
                           ));
  add_layer(new QcOsmLayer(this,
                           ++map_id, // 5
                           5,
                           QLatin1Literal("Transport Dark"),
                           QLatin1Literal("transport_dark"),
                           QLatin1Literal("png"),
                           QLatin1Literal("a.tile.thunderforest.com/transport-dark")
                           ));
  add_layer(new QcOsmLayer(this,
                           ++map_id, // 6
                           6,
                           QLatin1Literal("Landscape"),
                           QLatin1Literal("landscape"),
                           QLatin1Literal("png"),
                           QLatin1Literal("a.tile.thunderforest.com/landscape")
                           ));
  add_layer(new QcOsmLayer(this,
                           ++map_id, // 7
                           7,
                           QLatin1Literal("Outdoors"),
                           QLatin1Literal("outdoors"),
                           QLatin1Literal("png"),
                           QLatin1Literal("a.tile.thunderforest.com/outdoors")
                           ));
}

QcOsmPlugin::~QcOsmPlugin()
{}

/**************************************************************************************************/

// #include "osm_plugin.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
