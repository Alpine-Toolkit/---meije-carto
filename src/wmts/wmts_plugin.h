// -*- mode: c++ -*-

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

#ifndef __WMTS_PLUGIN_H__
#define __WMTS_PLUGIN_H__

/**************************************************************************************************/

#include "coordinate/mercator.h"
#include "wmts/elevation_service_reply.h"
#include "wmts/location_service_query.h"
#include "wmts/location_service_reply.h"
#include "wmts/tile_matrix_set.h"
#include "wmts/wmts_manager.h"
#include "wmts/wmts_network_tile_fetcher.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QString>
#include <QUrl>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcWmtsPlugin;

/**************************************************************************************************/

class QcWmtsPluginLayer
{
public:
  QcWmtsPluginLayer(QcWmtsPlugin * plugin,
                    int map_id,
                    int position,
                    const QString & title,
                    const QString & name,
                    const QString & image_format);
  QcWmtsPluginLayer(const QcWmtsPluginLayer & other);
  virtual ~QcWmtsPluginLayer();

  QcWmtsPluginLayer & operator=(const QcWmtsPluginLayer & other);

  QcWmtsPlugin * plugin() const { return m_plugin; }
  const QString & plugin_name() const;
  int map_id() const { return m_map_id; }
  int position() const { return m_position; }
  const QString & title() const { return m_title; }
  const QString & name() const { return m_name; }
  const QString & image_format() const { return m_image_format; }
  const QString & projection_name() const;

  QString hash_name() const;
  QcTileSpec create_tile_spec(int level, int x, int y) const;

  virtual QUrl url(const QcTileSpec & tile_spec) const = 0;

private:
  QcWmtsPlugin * m_plugin;
  int m_map_id;
  int m_position;
  QString m_title;
  QString m_name;
  QString m_image_format;
};

/**************************************************************************************************/

class QcWmtsPlugin : public QObject
{
  Q_OBJECT

public:
  QcWmtsPlugin(const QString & name, const QString & title, QcTileMatrixSet * tile_matrix_set);
  ~QcWmtsPlugin();

  const QString & name() const { return m_name; }
  const QString & title() const { return m_title; }
  QcTileMatrixSet & tile_matrix_set() { return *m_tile_matrix_set; } // Fixme: const ?
  const QcProjection & projection() { return m_tile_matrix_set->projection(); }

  // Fixme: & or *
  QNetworkAccessManager * network_manager() { return m_network_manager; }
  QcWmtsNetworkTileFetcher * tile_fetcher() { return &m_tile_fetcher; }
  QcWmtsManager * wmts_manager() { return &m_wmts_manager; }

  void set_user_agent(const QByteArray & user_agent) { m_user_agent = user_agent; }

  void add_layer(const QcWmtsPluginLayer * layer);
  const QList<const QcWmtsPluginLayer *> & layers() const { return m_layers; }
  // Fimxe: * vs & ?
  const QcWmtsPluginLayer * layer(int map_id) const;
  const QcWmtsPluginLayer * layer(const QcTileSpec & tile_spec) const;
  const QcWmtsPluginLayer * layer(const QString & title) const;

  // Fixme: protected ?
  QcTileSpec create_tile_spec(int map_id, int level, int x, int y) const {
    return QcTileSpec(m_name, map_id, level, x, y);
  }
  // Fixme: usefull ?
  QUrl make_layer_url(const QcTileSpec & tile_spec) const;

  virtual bool has_location_service() { return false; }
  virtual QSharedPointer<QcLocationServiceReply> geocode_request(const QcLocationServiceQuery & query);
  virtual QSharedPointer<QcLocationServiceReverseReply> reverse_geocode_request(const QcLocationServiceReverseQuery & query);

  virtual bool has_coordinate_elevation_service() { return false; }
  virtual bool has_sampling_elevation_service() { return false; }
  virtual QSharedPointer<QcElevationServiceReply> coordinate_elevations(const QVector<QcWgsCoordinate> & coordinates);

  // Fixme: protect ?
  // Fixme: networking could be moved in a dedicated class (QcWmtsNetworkTileFetcher but ols)
  QNetworkReply * get(const QUrl & url);
  QNetworkReply * post(const QUrl & url, const QByteArray & data);

  // off-line cache : load tiles from a polygon

private:
  QString m_name;
  QString m_title;
  QList<const QcWmtsPluginLayer *> m_layers;
  QHash<int, const QcWmtsPluginLayer *> m_layer_map;
  QSharedPointer<QcTileMatrixSet> m_tile_matrix_set;
  QByteArray m_user_agent;
  QNetworkAccessManager * m_network_manager; // share network manager for all requests: tile, ols, ...
  QcWmtsNetworkTileFetcher m_tile_fetcher;
  QcWmtsManager m_wmts_manager;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __WMTS_PLUGIN_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
