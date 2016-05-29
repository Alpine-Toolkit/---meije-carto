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

const QString &
QcWmtsPluginLayer::plugin_name() const
{
  return m_plugin->name();
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

QcWmtsPlugin::QcWmtsPlugin(const QString & name, const QString & title, QcTileMatrixSet * tile_matrix_set)
  : QObject(),
    m_name(name),
    m_title(title),
    m_tile_matrix_set(tile_matrix_set),
    m_user_agent("QtCarto based application"),
    m_network_manager(new QNetworkAccessManager()), // Fixme: delete ?, segfault if this is parent
    m_tile_fetcher(this),
    m_wmts_manager(name)
{
  wmts_manager()->set_tile_fetcher(&m_tile_fetcher);
  wmts_manager()->tile_cache(); // create a file tile cache

  // wmts_manager()->tile_cache()->clear_all();
}

QcWmtsPlugin::~QcWmtsPlugin()
{
  for (auto * layer : m_layers)
    delete layer;
}

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

QNetworkReply *
QcWmtsPlugin::get(const QUrl & url)
{
  QNetworkRequest request;
  request.setRawHeader("User-Agent", m_user_agent);
  request.setUrl(url);

  QNetworkReply * reply = m_network_manager->get(request);
  if (reply->error() != QNetworkReply::NoError)
    qWarning() << __FUNCTION__ << reply->errorString();

  return reply;
}

QNetworkReply *
QcWmtsPlugin::post(const QUrl & url, const QByteArray & data)
{
  QNetworkRequest request;
  request.setHeader(QNetworkRequest::UserAgentHeader, m_user_agent);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml;charset=UTF-8");
  request.setUrl(url);

  QNetworkReply * reply = m_network_manager->post(request, data);
  if (reply->error() != QNetworkReply::NoError)
    qWarning() << __FUNCTION__ << reply->errorString();

  return reply;
}

QSharedPointer<QcLocationServiceReply>
QcWmtsPlugin::geocode_request(const QcLocationServiceQuery & query)
{
  Q_UNUSED(query);
  return QSharedPointer<QcLocationServiceReply>(nullptr);
}

QSharedPointer<QcLocationServiceReverseReply>
QcWmtsPlugin::reverse_geocode_request(const QcLocationServiceReverseQuery & query)
{
  Q_UNUSED(query);
  return QSharedPointer<QcLocationServiceReverseReply>(nullptr);
}

QSharedPointer<QcElevationServiceReply>
QcWmtsPlugin::coordinate_elevations(const QVector<QcWgsCoordinate> & coordinates)
{
  Q_UNUSED(coordinates);
  return QSharedPointer<QcElevationServiceReply>(nullptr);
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
