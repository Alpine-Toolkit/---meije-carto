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

#include "esri_plugin.h"

#include <QUrlQuery>

/**************************************************************************************************/

QcEsriLayer::QcEsriLayer(QcEsriPlugin * plugin,
                         int map_id,
                         int position,
                         const QString & title,
                         const QString & name,
                         const QString & image_format,
                         const QString & base)
  : QcWmtsPluginLayer(plugin, map_id, position, title, name, image_format),
    m_base(base)
{}

QcEsriLayer::QcEsriLayer(const QcEsriLayer & other)
  : QcWmtsPluginLayer(other),
    m_base(other.m_base)
{}

QcEsriLayer::~QcEsriLayer()
{}

QcEsriLayer &
QcEsriLayer::operator=(const QcEsriLayer & other)
{
  if (this != &other) {
    QcWmtsPluginLayer::operator=(other);
    m_base = other.m_base;
  }

  return *this;
}

QUrl
QcEsriLayer::url(const QcTileSpec & tile_spec) const
{
  QUrlQuery url_query(QStringLiteral("layer=World_Topo_Map&style=default&tilematrixset=GoogleMapsCompatible&Service=WMTS&Request=GetTile&Version=1.0.0&Format=image/jpeg"));
  url_query.addQueryItem(QStringLiteral("TileMatrix"), QString::number(tile_spec.level()));
  url_query.addQueryItem(QStringLiteral("TileCol"), QString::number(tile_spec.x()));
  url_query.addQueryItem(QStringLiteral("TileRow"), QString::number(tile_spec.y()));
  QUrl url(QStringLiteral("https://") + m_base);
  // url.setScheme('https');
  url.setQuery(url_query);
  return url;
}

/**************************************************************************************************/

const QString QcEsriPlugin::PLUGIN_NAME = "esri";
const QString PLUGIN_TITLE = "Esri";
constexpr int NUMBER_OF_LEVELS = 20;
constexpr int TILE_SIZE = 256;

QcEsriPlugin::QcEsriPlugin()
  : QcWmtsPlugin(PLUGIN_NAME, PLUGIN_TITLE,
                 new QcMercatorTileMatrixSet(NUMBER_OF_LEVELS, TILE_SIZE))
{
  int map_id = -1;
  add_layer(new QcEsriLayer(this,
                            ++map_id, // 1
                            1,
                            QLatin1Literal("World Topo Map"),
                            QLatin1Literal("wolrd_topo_map"),
                            QLatin1Literal("jpeg"), // Fixme
                            QLatin1Literal("server.arcgisonline.com/arcgis/rest/services/World_Topo_Map/MapServer/WMTS")
                            ));
}

QcEsriPlugin::~QcEsriPlugin()
{}

/**************************************************************************************************/

// #include "esri_plugin.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
