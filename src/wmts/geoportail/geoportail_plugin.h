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

#ifndef __GEOPORTAIL_PLUGIN_H__
#define __GEOPORTAIL_PLUGIN_H__

/**************************************************************************************************/

#include "wmts/geoportail/geoportail_elevation_service_reply.h"
#include "wmts/geoportail/geoportail_license.h"
#include "wmts/geoportail/geoportail_location_service_reply.h"
#include "wmts/geoportail/geoportail_wmts_tile_fetcher.h" // Fixme: circular
#include "wmts/wmts_plugin.h"

#include <QAuthenticator>
#include <QHash>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcGeoportailPlugin;

/**************************************************************************************************/

class QcGeoportailLayer : public QcWmtsPluginLayer
{
public:
  QcGeoportailLayer(QcGeoportailPlugin * plugin,
                    int map_id,
                    int position,
                    const QString & title,
                    const QString & name,
                    const QString & image_format,
                    const QString & style);
  QcGeoportailLayer(const QcGeoportailLayer & other);
  ~QcGeoportailLayer();

  QcGeoportailLayer & operator=(const QcGeoportailLayer & other);

  const QString & style() const { return m_style; }

  QUrl url(const QcTileSpec & tile_spec) const;

private:
  QString m_style;
};

/**************************************************************************************************/

class QcGeoportailPlugin : public QcWmtsPlugin
{
  Q_OBJECT

public:
  static const QString PLUGIN_NAME;

public:
  QcGeoportailPlugin(const QcGeoportailWmtsLicense & license);
  ~QcGeoportailPlugin();

  const QcGeoportailWmtsLicense & license() const { return m_license; }

  // Fixme: private ?
  QcGeoportailWmtsTileFetcher * tile_fetcher() { return &m_tile_fetcher; }

  bool has_location_service() override { return true; }
  QSharedPointer<QcLocationServiceReply> geocode_request(const QcLocationServiceQuery & query) const override;
  // virtual QSharedPointer<QcLocationServiceReply> inverse_geocode_request(const QcLocationServiceInverseQuery & query) const;

  bool has_coordinate_elevation_service() override { return true; }
  bool has_sampling_elevation_service() override { return true; }
  QSharedPointer<QcElevationServiceReply> coordinate_elevations(const QVector<QcGeoCoordinateWGS84> & coordinates) const override;
  QSharedPointer<QcLocationServiceReverseReply> reverse_geocode_request(const QcLocationServiceReverseQuery & query) const override;

  void set_user_agent(const QByteArray & user_agent) { m_user_agent = user_agent; }

  // Fixme: protect ?
  QNetworkReply * get(const QUrl & url) const;
  QNetworkReply * post(const QUrl & url, const QByteArray & data) const;

private Q_SLOTS:
  void on_authentication_request_slot(QNetworkReply * reply, QAuthenticator * authenticator);

private:
  QcGeoportailWmtsLicense m_license;
  QNetworkAccessManager * m_network_manager;
  QByteArray m_user_agent;
  QcGeoportailWmtsTileFetcher m_tile_fetcher;

  QSharedPointer<QcLocationServiceReply> m_location_reply;
  QSharedPointer<QcLocationServiceReverseReply> m_location_reverse_reply;
  QSharedPointer<QcElevationServiceReply> m_elevation_reply;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __GEOPORTAIL_PLUGIN_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
