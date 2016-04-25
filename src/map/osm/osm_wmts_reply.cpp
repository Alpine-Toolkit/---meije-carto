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

#include "osm_wmts_reply.h"

/**************************************************************************************************/

QcOsmWmtsReply::QcOsmWmtsReply(QNetworkReply * reply,
                               const QcTileSpec & tile_spec,
                               const QString & format)
  : QcWmtsReply(tile_spec), m_reply(reply), m_format(format)
{
  connect(m_reply, SIGNAL(finished()),
	  this, SLOT(network_reply_finished()));

  connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)),
	  this, SLOT(network_reply_error(QNetworkReply::NetworkError)));
}

QcOsmWmtsReply::~QcOsmWmtsReply()
{
  if (m_reply) {
    m_reply->deleteLater();
    m_reply = nullptr;
  }
}

void
QcOsmWmtsReply::abort()
{
  if (m_reply)
    m_reply->abort();
}

QNetworkReply *
QcOsmWmtsReply::network_reply() const
{
  return m_reply;
}

// Handle a successful request : store image data
void
QcOsmWmtsReply::network_reply_finished()
{
  if (!m_reply)
    return;

  if (m_reply->error() != QNetworkReply::NoError) // Fixme: when ?
    return;

  set_map_image_data(m_reply->readAll());
  set_map_image_format(m_format);

  // Fixme: duplicated code
  set_finished(true);
  m_reply->deleteLater();
  m_reply = nullptr;
}

// Handle an unsuccessful request : set error message
void
QcOsmWmtsReply::network_reply_error(QNetworkReply::NetworkError error)
{
  if (!m_reply)
    return;

  if (error != QNetworkReply::OperationCanceledError)
    set_error(QcWmtsReply::CommunicationError, m_reply->errorString());

  set_finished(true);
  m_reply->deleteLater();
  m_reply = nullptr;
}

/**************************************************************************************************/

// #include "osm_wmts_reply.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
