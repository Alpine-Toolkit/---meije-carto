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

#include "osm_wmts_tile_fetcher.h"
#include "osm_wmts_reply.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QDebug>

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcOsmWmtsTileFetcher::QcOsmWmtsTileFetcher()
  : QcWmtsTileFetcher(),
    m_network_manager(new QNetworkAccessManager(this)),
    m_user_agent("QtCarto based application"),
    m_reply_format("jpg")
{
  connect(m_network_manager,
	  SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)),
	  this,
	  SLOT(on_authentication_request_slot(QNetworkReply*, QAuthenticator*)));
}

/*
void QcOsmWmtsTileFetcher::setFormat(const QString &format)
{
  m_format = format;

  if (m_format == "jpg")
    m_replyFormat = "jpg";
  else
    qWarning() << "Unknown map format " << m_format;
}

void
QcOsmWmtsTileFetcher::setAccessToken(const QString &accessToken)
{
  m_accessToken = accessToken;
}
*/

QcWmtsReply *
QcOsmWmtsTileFetcher::get_tile_image(const QcTileSpec & tile_spec)
{
  QString url_prefix;
  QString suffix = QStringLiteral(".png");

  // int map_id = tile_spec.map_id(); // Fixme:
  int map_id = 1;
  switch (map_id) {
  case 1:
    url_prefix = QStringLiteral("http://otile1.mqcdn.com/tiles/1.0.0/map/");
    suffix = QStringLiteral(".jpg");
    break;
  case 2:
    url_prefix = QStringLiteral("http://otile1.mqcdn.com/tiles/1.0.0/sat/");
    suffix = QStringLiteral(".jpg");
    break;
  case 3:
    url_prefix = QStringLiteral("http://a.tile.thunderforest.com/cycle/");
    break;
  case 4:
    url_prefix = QStringLiteral("http://a.tile.thunderforest.com/transport/");
    break;
  case 5:
    url_prefix = QStringLiteral("http://a.tile.thunderforest.com/transport-dark/");
    break;
  case 6:
    url_prefix = QStringLiteral("http://a.tile.thunderforest.com/landscape/");
    break;
  case 7:
    url_prefix = QStringLiteral("http://a.tile.thunderforest.com/outdoors/");
    break;
    // Fixme: purpose ?
    // case 8:
    //   url_prefix = m_url_prefix;
    //   break;
  default:
    qWarning("Unknown map id %d\n", tile_spec.map_id());
  }

  QUrl url(url_prefix + QString::number(tile_spec.level()) + QLatin1Char('/') +
           QString::number(tile_spec.x()) + QLatin1Char('/') +
           QString::number(tile_spec.y()) + suffix);
  qInfo() << url;

  QNetworkRequest request;
  request.setRawHeader("User-Agent", m_user_agent);
  request.setUrl(url);

  QNetworkReply *reply = m_network_manager->get(request);
  if (reply->error() != QNetworkReply::NoError)
    qWarning() << __FUNCTION__ << reply->errorString();

  return new QcOsmWmtsReply(reply, tile_spec, m_reply_format);
}

/**************************************************************************************************/

// #include "osm_wmts_tile_fetcher.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
