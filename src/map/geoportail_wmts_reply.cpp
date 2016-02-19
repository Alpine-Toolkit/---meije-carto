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

#include "geoportail_wmts_reply.h"

/**************************************************************************************************/

QcGeoportailWmtsReply::QcGeoportailWmtsReply(QNetworkReply * reply,
					     const QcTileSpec & tile_spec,
					     const QString & format,
					     QObject * parent)
  : QcWmtsReply(tile_spec, parent), m_reply(reply), m_format(format)
{
  connect(m_reply, SIGNAL(finished()),
	  this, SLOT(network_reply_finished()));
  connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)),
	  this, SLOT(network_reply_error(QNetworkReply::NetworkError)));
}

QcGeoportailWmtsReply::~QcGeoportailWmtsReply()
{
  if (m_reply) {
    m_reply->deleteLater();
    m_reply = nullptr;
  }
}

void
QcGeoportailWmtsReply::abort()
{
  if (!m_reply)
    return;

  m_reply->abort();
}

QNetworkReply *
QcGeoportailWmtsReply::network_reply() const
{
  return m_reply;
}

void
QcGeoportailWmtsReply::network_reply_finished()
{
  if (!m_reply)
    return;

  if (m_reply->error() != QNetworkReply::NoError)
    return;

  set_map_image_data(m_reply->readAll());
  set_map_image_format(m_format);
  set_finished(true);

  m_reply->deleteLater();
  m_reply = nullptr;
}

void
QcGeoportailWmtsReply::network_reply_error(QNetworkReply::NetworkError error)
{
  if (!m_reply)
    return;

  if (error != QNetworkReply::OperationCanceledError)
    set_error(QcWmtsReply::CommunicationError, m_reply->errorString());

  set_finished(true);
  m_reply->deleteLater();
  m_reply = nullptr;
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
