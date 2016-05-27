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

#include "austria_plugin.h"

/**************************************************************************************************/

QcAustriaLayer::QcAustriaLayer(QcAustriaPlugin * plugin,
                               int map_id,
                               int position,
                               const QString & title,
                               const QString & name,
                               const QString & image_format)
  : QcWmtsPluginLayer(plugin, map_id, position, title, name, image_format)
{}

// QcAustriaLayer::QcAustriaLayer(const QcAustriaLayer & other)
//   : QcWmtsPluginLayer(other)
// {}

QUrl
QcAustriaLayer::url(const QcTileSpec & tile_spec) const
{
  // http://basemap.at/application
  // http://maps.wien.gv.at/basemap/geolandbasemap/normal/google3857/8/90/135.png
  // http://maps3.wien.gv.at/basemap/bmaphidpi/normal/google3857/14/5721/8846.jpeg

  return QUrl(QStringLiteral("http://maps.wien.gv.at/basemap/") +
              name() + QLatin1Char('/') +
              QLatin1Literal("normal/") +
              QLatin1Literal("google3857/") +
              QString::number(tile_spec.level()) + QLatin1Char('/') +
              QString::number(tile_spec.x()) + QLatin1Char('/') +
              QString::number(tile_spec.y()) +
              QLatin1Char('.') + image_format());
}

/**************************************************************************************************/

const QString QcAustriaPlugin::PLUGIN_NAME = "austria";
const QString PLUGIN_TITLE = "Austria";
constexpr int NUMBER_OF_LEVELS = 20;
constexpr int TILE_SIZE = 512; // 256

QcAustriaPlugin::QcAustriaPlugin()
  : QcWmtsPlugin(PLUGIN_NAME, PLUGIN_TITLE,
                 new QcMercatorTileMatrixSet(NUMBER_OF_LEVELS, TILE_SIZE)) // Fixme: offset ?
{
  int map_id = -1;
  add_layer(new QcAustriaLayer(this,
                               ++map_id, // 1
                               1,
                               QLatin1Literal("Standard Map"),
                               QLatin1Literal("geolandbasemap"),
                               QLatin1Literal("png")
                               ));
}

QcAustriaPlugin::~QcAustriaPlugin()
{}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
