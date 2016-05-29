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

#include "artic_web_map_plugin.h"

/**************************************************************************************************/

QcArticWebMapLayer::QcArticWebMapLayer(QcArticWebMapPlugin * plugin,
                                       int map_id,
                                       int position,
                                       const QString & title,
                                       const QString & name)
  : QcWmtsPluginLayer(plugin, map_id, position, title, name, QLatin1Literal("png"))
{}

QcArticWebMapLayer::QcArticWebMapLayer(const QcArticWebMapLayer & other)
  : QcWmtsPluginLayer(other)
{}

QcArticWebMapLayer::~QcArticWebMapLayer()
{}

QUrl
QcArticWebMapLayer::url(const QcTileSpec & tile_spec) const
{
  /* http://{s}.tiles.arcticconnect.org/osm_{projection}/{z}/{x}/{y}.png
   *
   * where {s} is either a, b or c, {projection} is one of 3571, 3572,
   * 3573, 3574, 3575 or 3576, {z} is the zoom level, {x} is the tile
   * x-index, and {y} is the tile y-index.
   */

  return QUrl(QStringLiteral("http://a.tiles.arcticconnect.org/osm_3571/") +
              QString::number(tile_spec.level()) + QLatin1Char('/') +
              QString::number(tile_spec.x()) + QLatin1Char('/') +
              QString::number(tile_spec.y()) +
              QLatin1Char('.') + image_format());
}

/**************************************************************************************************/

const QString QcArticWebMapPlugin::PLUGIN_NAME = "artic-web-map-3571";
const QString PLUGIN_TITLE = "Artic Web Map";
constexpr int NUMBER_OF_LEVELS = 20;
constexpr int TILE_SIZE = 256;

QcArticWebMapPlugin::QcArticWebMapPlugin()
  : QcWmtsPlugin(PLUGIN_NAME, PLUGIN_TITLE,
                 new  QcTileMatrixSet(QcProjection::by_srid(QLatin1Literal("epsg:3571")),
                                      QcVectorDouble(1., -1.),
                                      QcVectorDouble(-HALF_EQUATORIAL_PERIMETER, HALF_EQUATORIAL_PERIMETER), // Fixme: use projection
                                      QcWgsCoordinate(180., 90.),
                                      NUMBER_OF_LEVELS,
                                      TILE_SIZE))
{
  int map_id = -1;
  add_layer(new QcArticWebMapLayer(this,
                                   ++map_id, // 1
                                   1,
                                   QLatin1Literal("Bearing Sea Map ESPG:3571"),
                                   QLatin1Literal("espg-3571")));
}

QcArticWebMapPlugin::~QcArticWebMapPlugin()
{}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
