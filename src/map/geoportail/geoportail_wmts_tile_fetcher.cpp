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
#include "geoportail_wmts_reply.h"
#include "geoportail_wmts_tile_fetcher.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QDebug>

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcGeoportailWmtsTileFetcher::QcGeoportailWmtsTileFetcher(const QcGeoportailPlugin * plugin)
  : QcWmtsTileFetcher(),
    m_network_manager(new QNetworkAccessManager(this)),
    m_plugin(plugin),
    m_user_agent("QtCarto based application"),
    m_reply_format("jpg")
{
  connect(m_network_manager,
	  SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)),
	  this,
	  SLOT(on_authentication_request_slot(QNetworkReply*, QAuthenticator*)));
}

/*
void QcGeoportailWmtsTileFetcher::setFormat(const QString &format)
{
  m_format = format;

  if (m_format == "jpg")
    m_replyFormat = "jpg";
  else
    qWarning() << "Unknown map format " << m_format;
}

void
QcGeoportailWmtsTileFetcher::setAccessToken(const QString &accessToken)
{
  m_accessToken = accessToken;
}
*/

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

QcWmtsReply *
QcGeoportailWmtsTileFetcher::get_tile_image(const QcTileSpec & tile_spec)
{
  QUrl url = m_plugin->make_layer_url(tile_spec);
  qInfo() << url;

  QNetworkRequest request;
  request.setRawHeader("User-Agent", m_user_agent);
  request.setUrl(url);

  QNetworkReply *reply = m_network_manager->get(request);
  if (reply->error() != QNetworkReply::NoError)
    qWarning() << __FUNCTION__ << reply->errorString();

  return new QcGeoportailWmtsReply(reply, tile_spec, m_reply_format);
}

void
QcGeoportailWmtsTileFetcher::on_authentication_request_slot(QNetworkReply *reply,
							    QAuthenticator *authenticator)
{
  Q_UNUSED(reply);
  qInfo() << "on_authentication_request_slot";
  const QcGeoportailWmtsLicense & _license = m_plugin->license();
  authenticator->setUser(_license.user());
  authenticator->setPassword(_license.password());
}

/**************************************************************************************************/

// #include "geoportail_wmts_tile_fetcher.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
