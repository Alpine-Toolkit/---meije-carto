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

#include "spain_plugin.h"

/**************************************************************************************************/

QcSpainLayer::QcSpainLayer(QcSpainPlugin * plugin,
                           int map_id,
                           int position,
                           const QString & title,
                           const QString & name,
                           const QString & base,
                           const QString & image_format)
  : QcWmtsPluginLayer(plugin, map_id, position, title, name, image_format),
    m_base(base)
{}

// QcSpainLayer::QcSpainLayer(const QcSpainLayer & other)
//   : QcWmtsPluginLayer(other)
// {}

QUrl
QcSpainLayer::url(const QcTileSpec & tile_spec) const
{
 // http://www.ign.es/wmts/ign-base?service=WMTS&request=GetTile&version=1.0.0&layer=IGNBaseTodo&tilematrix=10&tilematrixset=EPSG%3A25830&tilerow=149&tilecol=124&format=image%2Fjpeg&style=default

 // http://www.ign.es/wmts/pnoa-ma?service=WMTS&request=GetTile&version=1.0.0&layer=OI.OrthoimageCoverage&tilematrix=10&tilematrixset=EPSG%3A25830&tilerow=149&tilecol=127&format=image%2Fjpeg&style=default

  QStringList parameters;
  parameters << QStringLiteral("service=WMTS");
  parameters << QStringLiteral("request=GetTile");
  parameters << QStringLiteral("version=1.0.0");
  parameters << QStringLiteral("layer=") + name();
  parameters << QStringLiteral("tilematrix=") + QString::number(tile_spec.level());
  // parameters << QStringLiteral("tilematrixset=EPSG:25830");
  parameters << QStringLiteral("tilematrixset=EPSG:3857");
  parameters << QStringLiteral("tilerow=") + QString::number(tile_spec.y());
  parameters << QStringLiteral("tilecol=") + QString::number(tile_spec.x());
  parameters << QStringLiteral("format=image/") + image_format();
  parameters << QStringLiteral("style=default");

  return QUrl(QStringLiteral("http://www.ign.es/wmts/") +
              m_base + QLatin1Char('?') +
              parameters.join(QString('&')));
}

/**************************************************************************************************/

const QString QcSpainPlugin::PLUGIN_NAME = "spain";
const QString PLUGIN_TITLE = "Spain Instituto Geografico National";
constexpr int NUMBER_OF_LEVELS = 20;
constexpr int TILE_SIZE = 256;

QcSpainPlugin::QcSpainPlugin()
  : QcWmtsPlugin(PLUGIN_NAME, PLUGIN_TITLE,
                 new QcMercatorTileMatrixSet(NUMBER_OF_LEVELS, TILE_SIZE))
{
  int map_id = -1;
  add_layer(new QcSpainLayer(this,
                             ++map_id, // 1
                             1,
                             QLatin1Literal("Map"),
                             QLatin1Literal("IGNBaseTodo"),
                             QLatin1Literal("ign-base"),
                             QLatin1Literal("jpeg")
                             ));
  add_layer(new QcSpainLayer(this,
                             ++map_id, // 1
                             2,
                             QLatin1Literal("Orthoimage"),
                             QLatin1Literal("OI.OrthoimageCoverage"),
                             QLatin1Literal("pnoa-ma"),
                             QLatin1Literal("jpeg")
                             ));
}

QcSpainPlugin::~QcSpainPlugin()
{}

/**************************************************************************************************/

// #include "swiss_plugin.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
