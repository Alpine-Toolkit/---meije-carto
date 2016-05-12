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

#include "geoportail_plugin.h"
#include "geoportail_wmts_tile_fetcher.h"

#include <QString>

/**************************************************************************************************/

QcGeoportailLayer::QcGeoportailLayer(QcGeoportailPlugin * plugin,
                                     int map_id,
                                     int position,
                                     const QString & title,
                                     const QString & name,
                                     const QString & image_format,
                                     const QString & style)
  : QcWmtsPluginLayer(plugin, map_id, position, title, name, image_format),
    m_style(style)
{}

QcGeoportailLayer::QcGeoportailLayer(const QcGeoportailLayer & other)
  : QcWmtsPluginLayer(other),
    m_style(other.m_style)
{}

QcGeoportailLayer::~QcGeoportailLayer()
{}

QcGeoportailLayer &
QcGeoportailLayer::operator=(const QcGeoportailLayer & other)
{
  if (this != &other) {
    QcWmtsPluginLayer::operator=(other);
    m_style = other.m_style;
  }

  return *this;
}

QUrl
QcGeoportailLayer::url(const QcTileSpec & tile_spec) const
{
  /*
   * Vue aérienne LAYER = ORTHOIMAGERY.ORTHOPHOTOS
   * http://wxs.ign.fr/<KEY>/geoportail/wmts?
   * SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=ORTHOIMAGERY.ORTHOPHOTOS
   * &STYLE=normal&TILEMATRIXSET=PM&TILEMATRIX=16&TILEROW=23327&TILECOL=33919&FORMAT=image/jpeg
   *
   * Carte LAYER = GEOGRAPHICALGRIDSYSTEMS.MAPS.SCAN-EXPRESS.STANDARD
   * http://wxs.ign.fr/<KEY>/geoportail/wmts?
   * SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=GEOGRAPHICALGRIDSYSTEMS.MAPS.SCAN-EXPRESS.STANDARD
   * &STYLE=normal&TILEMATRIXSET=PM&TILEMATRIX=16&TILEROW=23327&TILECOL=33920&FORMAT=image/jpeg
   *
   * Carte topographique LAYER = GEOGRAPHICALGRIDSYSTEMS.MAPS.SCAN25TOUR
   * http://wxs.ign.fr/<KEY>/geoportail/wmts?
   * SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=GEOGRAPHICALGRIDSYSTEMS.MAPS.SCAN25TOUR
   * &STYLE=normal&TILEMATRIXSET=PM&TILEMATRIX=16&TILEROW=23326&TILECOL=33920&FORMAT=image/jpeg
   *
   * Parcelles cadastrales
   * http://wxs.ign.fr/<KEY>/geoportail/wmts?
   * SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=CADASTRALPARCELS.PARCELS
   * &STYLE=bdparcellaire&TILEMATRIXSET=PM&TILEMATRIX=16&TILEROW=23325&TILECOL=33915&FORMAT=image/png
   *
   * Routes
   * http://wxs.ign.fr/<KEY>/geoportail/wmts?
   * SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=TRANSPORTNETWORKS.ROADS&STYLE=normal
   * &TILEMATRIXSET=PM&TILEMATRIX=16&TILEROW=23326&TILECOL=33917&FORMAT=image/png
   *
   */

  // Fixme: name, split ???
  QStringList parameters;
  parameters << QStringLiteral("SERVICE=WMTS");
  parameters << QStringLiteral("VERSION=1.0.0");
  parameters << QStringLiteral("REQUEST=GetTile");
  parameters << QStringLiteral("LAYER=") + name();
  parameters << QStringLiteral("STYLE=") + m_style;
  parameters << QStringLiteral("FORMAT=image/") + image_format();
  parameters << QStringLiteral("TILEMATRIXSET=PM");
  parameters << QStringLiteral("TILEMATRIX=") + QString::number(tile_spec.level());
  parameters << QStringLiteral("TILEROW=") + QString::number(tile_spec.y());
  parameters << QStringLiteral("TILECOL=") + QString::number(tile_spec.x());

  QString api_key = static_cast<QcGeoportailPlugin *>(plugin())->license().api_key();

  return QUrl(QStringLiteral("https://wxs.ign.fr/") +
              api_key +
              QStringLiteral("/geoportail/wmts?") +
              parameters.join(QString('&')));
}

/**************************************************************************************************/

const QString QcGeoportailPlugin::PLUGIN_NAME = "geoportail";
constexpr size_t NUMBER_OF_LEVELS = 20;
constexpr size_t TILE_SIZE = 256;

QcGeoportailPlugin::QcGeoportailPlugin(const QcGeoportailWmtsLicense & license)
  : QcWmtsPlugin(PLUGIN_NAME, NUMBER_OF_LEVELS, TILE_SIZE),
    m_license(license),
    m_tile_fetcher(this)
{
  wmts_manager()->set_tile_fetcher(&m_tile_fetcher);
  wmts_manager()->tile_cache(); // create a file tile cache

  // wmts_manager()->tile_cache()->clear_all();

  // Fixme: to json
  int map_id = -1;
  add_layer(new QcGeoportailLayer(this,
                                  ++map_id, // 1
                                  2,
                                  QStringLiteral("Carte topographique"),
                                  QLatin1Literal("GEOGRAPHICALGRIDSYSTEMS.MAPS"), // .SCAN25TOUR
                                  QLatin1Literal("jpeg"),
                                  QLatin1Literal("normal")
                                  ));
  add_layer(new QcGeoportailLayer(this,
                                  ++map_id, // 2
                                  3,
                                  QStringLiteral("Vue aérienne"),
                                  QLatin1Literal("ORTHOIMAGERY.ORTHOPHOTOS"),
                                  QLatin1Literal("jpeg"),
                                  QLatin1Literal("normal")
                                  ));
  add_layer(new QcGeoportailLayer(this,
                                  ++map_id, // 0
                                  1,
                                  QStringLiteral("Carte"),
                                  QLatin1Literal("GEOGRAPHICALGRIDSYSTEMS.MAPS.SCAN-EXPRESS.STANDARD"),
                                  QLatin1Literal("jpeg"),
                                  QLatin1Literal("normal")
                                  ));
  add_layer(new QcGeoportailLayer(this,
                                  ++map_id, // 3
                                  4,
                                  QStringLiteral("Routes"),
                                  QLatin1Literal("TRANSPORTNETWORKS.ROADS"),
                                  QLatin1Literal("png"),
                                  QLatin1Literal("normal")
                                  ));
  add_layer(new QcGeoportailLayer(this,
                                  ++map_id, // 4
                                  5,
                                  QStringLiteral("Parcelles cadastrales"),
                                  QLatin1Literal("CADASTRALPARCELS.PARCELS"),
                                  QLatin1Literal("png"),
                                  QLatin1Literal("bdparcellaire")
                                  ));
}

QcGeoportailPlugin::~QcGeoportailPlugin()
{}

/**************************************************************************************************/

// #include "geoportail_plugin.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
