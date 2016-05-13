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

#include "geoportail_wmts_tile_fetcher.h"

#include "geoportail_plugin.h"
#include "geoportail_wmts_reply.h"
#include "wmts/wmts_plugin.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QDebug>

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcGeoportailWmtsTileFetcher::QcGeoportailWmtsTileFetcher(const QcGeoportailPlugin * plugin)
  : QcWmtsTileFetcher(),
    m_plugin(plugin),
    m_network_manager(new QNetworkAccessManager(this)),
    m_user_agent("QtCarto based application")
{
  connect(m_network_manager,
	  SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)),
	  this,
	  SLOT(on_authentication_request_slot(QNetworkReply*, QAuthenticator*)));
}

QcGeoportailWmtsTileFetcher::~QcGeoportailWmtsTileFetcher()
{
  delete m_network_manager;
}

QcWmtsReply *
QcGeoportailWmtsTileFetcher::get_tile_image(const QcTileSpec & tile_spec)
{
  const QcWmtsPluginLayer * layer = m_plugin->layer(tile_spec);
  QUrl url = layer->url(tile_spec);
  // qInfo() << url;

  QNetworkRequest request;
  request.setRawHeader("User-Agent", m_user_agent);
  request.setUrl(url);

  QNetworkReply *reply = m_network_manager->get(request);
  if (reply->error() != QNetworkReply::NoError)
    qWarning() << __FUNCTION__ << reply->errorString();

  return new QcGeoportailWmtsReply(reply, tile_spec, layer->image_format());
}

void
QcGeoportailWmtsTileFetcher::on_authentication_request_slot(QNetworkReply *reply,
							    QAuthenticator *authenticator)
{
  Q_UNUSED(reply);
  // qInfo() << "on_authentication_request_slot";
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
