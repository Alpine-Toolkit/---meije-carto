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

#include "germany_plugin.h"

/**************************************************************************************************/

QcGermanyLayer::QcGermanyLayer(QcGermanyPlugin * plugin,
                               int map_id,
                               int position,
                               const QString & title,
                               const QString & name,
                               const QString & image_format)
  : QcWmtsPluginLayer(plugin, map_id, position, title, name, image_format)
{}

// QcGermanyLayer::QcGermanyLayer(const QcGermanyLayer & other)
//   : QcWmtsPluginLayer(other)
// {}

QUrl
QcGermanyLayer::url(const QcTileSpec & tile_spec) const
{
  // http://sg.geodatenzentrum.de/wmts_webatlasde/tile/1.0.0/webatlasde/default/DE_EPSG_25832_ADV/3/3/7.png

  // http://sg.geodatenzentrum.de/wms_dtk200?FORMAT=image%2Fjpeg&TRANSPARENT=FALSE&SERVICE=WMS&VERSION=1.1.1&REQUEST=GetMap&STYLES=&LAYERS=DTK200&SRS=EPSG%3A25832&BBOX=550687.2074199,5679937.1602661,555579.17723015,5684829.1300764&WIDTH=256&HEIGHT=256

  return QUrl(QStringLiteral("http://sg.geodatenzentrum.de/wmts_webatlasde/tile/1.0.0/") +
              name()  + QLatin1Char('/') +
              QLatin1Literal("default/") +
              QLatin1Literal("DE_EPSG_25832_ADV/") +
              QString::number(tile_spec.level()) + QLatin1Char('/') +
              QString::number(tile_spec.x()) + QLatin1Char('/') +
              QString::number(tile_spec.y()) +
              QLatin1Char('.') + image_format());
}

/**************************************************************************************************/

const QString QcGermanyPlugin::PLUGIN_NAME = "germany";
const QString PLUGIN_TITLE = "Germany";
constexpr int NUMBER_OF_LEVELS = 20;
constexpr int TILE_SIZE = 256;

QcGermanyPlugin::QcGermanyPlugin()
  : QcWmtsPlugin(PLUGIN_NAME, PLUGIN_TITLE,
                 new QcMercatorTileMatrixSet(NUMBER_OF_LEVELS, TILE_SIZE))
{

  int map_id = -1;
  add_layer(new QcGermanyLayer(this,
                               ++map_id, // 1
                               1,
                               QLatin1Literal("Map"),
                               QLatin1Literal("webatlasde"),
                               QLatin1Literal("png")
                               ));
}

QcGermanyPlugin::~QcGermanyPlugin()
{}

/**************************************************************************************************/

// #include "swiss_plugin.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
