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

QcGeoportailLayer::QcGeoportailLayer()
  : m_map_id(),
    m_position(),
    m_title(),
    m_name(),
    m_style(),
    m_image_format()
{}

QcGeoportailLayer::QcGeoportailLayer(int map_id,
                                     int position,
                                     const QString & title,
                                     const QString & name,
                                     const QString & style,
                                     const QString & image_format)
  : m_map_id(map_id),
    m_position(position),
    m_title(title),
    m_name(name),
    m_style(style),
    m_image_format(image_format)
{}

QcGeoportailLayer::QcGeoportailLayer(const QcGeoportailLayer & other)
  : m_map_id(other.m_map_id),
    m_position(other.m_position),
    m_title(other.m_title),
    m_name(other.m_name),
    m_style(other.m_style),
    m_image_format(other.m_image_format)
{}

QcGeoportailLayer::~QcGeoportailLayer()
{}

QcGeoportailLayer &
QcGeoportailLayer::operator=(const QcGeoportailLayer & other)
{
  if (this != &other) {
    m_map_id = other.m_map_id;
    m_position = other.m_position;
    m_title = other.m_title;
    m_name = other.m_name;
    m_style = other.m_style;
    m_image_format = other.m_image_format;
  }

  return *this;
}

QUrl
QcGeoportailLayer::url(const QcTileSpec & tile_spec, const QcGeoportailWmtsLicense & license) const
{
  // Fixme: name, split ???
  QStringList parameters;
  parameters << QStringLiteral("SERVICE=WMTS");
  parameters << QStringLiteral("VERSION=1.0.0");
  parameters << QStringLiteral("REQUEST=GetTile");
  parameters << QStringLiteral("LAYER=") + m_name;
  parameters << QStringLiteral("STYLE=") + m_style;
  parameters << QStringLiteral("FORMAT=image/") + m_image_format;
  parameters << QStringLiteral("TILEMATRIXSET=PM");
  parameters << QStringLiteral("TILEMATRIX=") + QString::number(tile_spec.level());
  parameters << QStringLiteral("TILEROW=") + QString::number(tile_spec.y());
  parameters << QStringLiteral("TILECOL=") + QString::number(tile_spec.x());

  return QUrl(QStringLiteral("https://wxs.ign.fr/") +
              license.api_key() +
              QStringLiteral("/geoportail/wmts?") +
              parameters.join(QString('&')));
}

/**************************************************************************************************/

constexpr const char * PLUGIN_NAME = "geoportail";
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
  add_layer(QcGeoportailLayer(++map_id, // 1
                              2,
                              QStringLiteral("Carte topographique"),
                              QStringLiteral("GEOGRAPHICALGRIDSYSTEMS.MAPS"), // .SCAN25TOUR
                              QStringLiteral("normal"),
                              QStringLiteral("jpeg")));
  add_layer(QcGeoportailLayer(++map_id, // 2
                              3,
                              QStringLiteral("Vue aérienne"),
                              QStringLiteral("ORTHOIMAGERY.ORTHOPHOTOS"),
                              QStringLiteral("normal"),
                              QStringLiteral("jpeg")));

  add_layer(QcGeoportailLayer(++map_id, // 0
                              1,
                              QStringLiteral("Carte"),
                              QStringLiteral("GEOGRAPHICALGRIDSYSTEMS.MAPS.SCAN-EXPRESS.STANDARD"),
                              QStringLiteral("normal"),
                              QStringLiteral("jpeg")));
  add_layer(QcGeoportailLayer(++map_id, // 3
                              4,
                              QStringLiteral("Routes"),
                              QStringLiteral("TRANSPORTNETWORKS.ROADS"),
                              QStringLiteral("normal"),
                              QStringLiteral("png")));
  add_layer(QcGeoportailLayer(++map_id, // 4
                              5,
                              QStringLiteral("Parcelles cadastrales"),
                              QStringLiteral("CADASTRALPARCELS.PARCELS"),
                              QStringLiteral("bdparcellaire"),
                              QStringLiteral("png")));
}

QcGeoportailPlugin::~QcGeoportailPlugin()
{}

void
QcGeoportailPlugin::add_layer(const QcGeoportailLayer & layer)
{
  // Fixme: check for errors
  m_layers.insert(layer.map_id(), layer);
}

// const QcGeoportailLayer &
QcGeoportailLayer
QcGeoportailPlugin::layer(const QString & title) const
{
  for (const auto & _layer : m_layers)
    if (_layer.title() == title)
      return _layer;
  return QcGeoportailLayer();
}

QUrl
QcGeoportailPlugin::make_layer_url(const QcTileSpec & tile_spec) const
{
  // Fixme: error
  return m_layers[tile_spec.map_id()].url(tile_spec, m_license);
}

/**************************************************************************************************/

// #include "geoportail_plugin.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
