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

#include "swiss_confederation_plugin.h"

/**************************************************************************************************/

QcSwissConfederationLayer::QcSwissConfederationLayer(QcSwissConfederationPlugin * plugin,
                                                     int map_id,
                                                     int position,
                                                     const QString & title,
                                                     const QString & name,
                                                     const QString & image_format)
  : QcWmtsPluginLayer(plugin, map_id, position, title, name, image_format)
{}

// QcSwissConfederationLayer::QcSwissConfederationLayer(const QcSwissConfederationLayer & other)
//   : QcWmtsPluginLayer(other)
// {}

QUrl
QcSwissConfederationLayer::url(const QcTileSpec & tile_spec) const
{
  // http://wmts20.geo.admin.ch/1.0.0/' + layername + '/default/' + timestamp + '/3857/{z}/{x}/{y}.jpeg'
  // http://wmts20.geo.admin.ch/1.0.0/ch.swisstopo.pixelkarte-farbe/default/20151231/3857/15/17122/11501.jpeg
  // http://wmts20.geo.admin.ch/1.0.0/ch.swisstopo.pixelkarte-farbe/default/current/3857/10/532/361.jpeg
  // https://wmts20.geo.admin.ch/1.0.0/ch.astra.wanderland/default/20160315/21781/10/527/362.png
  return QUrl(QStringLiteral("https://wmts20.geo.admin.ch/1.0.0/") +
              name()  + QLatin1Char('/') +
              QLatin1Literal("default/") +
              // QLatin1Literal("20151231") + QLatin1Char('/') +
              QLatin1Literal("20160315") + QLatin1Char('/') +
              // QLatin1Literal("current") + QLatin1Char('/') +
              // QLatin1Literal("3857/") +
              QLatin1Literal("21781/") +
              QString::number(tile_spec.level()) + QLatin1Char('/') +
              QString::number(tile_spec.x()) + QLatin1Char('/') +
              QString::number(tile_spec.y()) +
              QLatin1Char('.') + image_format());
}

/**************************************************************************************************/

const QString QcSwissConfederationPlugin::PLUGIN_NAME = "swiss";
const QString PLUGIN_TITLE = "Swiss Confederation";
constexpr int NUMBER_OF_LEVELS = 20;
constexpr int TILE_SIZE = 256;

QcSwissConfederationPlugin::QcSwissConfederationPlugin()
  : QcWmtsPlugin(PLUGIN_NAME, PLUGIN_TITLE,
                 new QcMercatorTileMatrixSet(NUMBER_OF_LEVELS, TILE_SIZE))
{
  int map_id = -1;
  // add_layer(new QcSwissConfederationLayer(this,
  //                                         ++map_id, // 1
  //                                         1,
  //                                         QLatin1Literal("Carte nationale 1:25'000 | CN25"),
  //                                         QLatin1Literal("ch.swisstopo.pixelkarte-farbe"),
  //                                         QLatin1Literal("jpeg")
  //                                         ));
  add_layer(new QcSwissConfederationLayer(this,
                                          ++map_id, // 1
                                          1,
                                          QLatin1Literal("La Suisse à pieds"),
                                          QLatin1Literal("ch.astra.wanderland"),
                                          QLatin1Literal("png")
                                          ));
}

QcSwissConfederationPlugin::~QcSwissConfederationPlugin()
{}

/**************************************************************************************************/

// #include "swiss_plugin.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
