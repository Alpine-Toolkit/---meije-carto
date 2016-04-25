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

/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

/**************************************************************************************************/

#include "wmts_tile_fetcher.h"

#include <QtCore/QTimerEvent>

#include <QtDebug>

/**************************************************************************************************/

QcWmtsTileFetcher::QcWmtsTileFetcher()
  : QObject(),
    m_enabled(true)
{
  // Fixme: useless ?
  // if (!m_queue.isEmpty())
  //   m_timer.start(0, this);
}

QcWmtsTileFetcher::~QcWmtsTileFetcher()
{}

void
QcWmtsTileFetcher::update_tile_requests(const QcTileSpecSet & tiles_added,
					const QcTileSpecSet & tiles_removed)
{
  qInfo();
  // qInfo() << tiles_added;
  // qInfo() << tiles_removed;

  QMutexLocker mutex_locker(&m_queue_mutex);

  cancel_tile_requests(tiles_removed);
  m_queue += tiles_added.toList();

  // Start timer to fetch tiles from queue
  if (m_enabled && !m_queue.isEmpty() && !m_timer.isActive()) {
    m_timer.start(0, this);
  }
}

void
QcWmtsTileFetcher::cancel_tile_requests(const QcTileSpecSet & tiles)
{
  // Delete objects and abort requests if they are still running
  for (const QcTileSpec & tile_spec: tiles) {
    QcWmtsReply * reply = m_invmap.value(tile_spec, nullptr);
    if (reply) {
      m_invmap.remove(tile_spec);
      reply->abort();
      if (reply->is_finished())
	reply->deleteLater();
    }
    // Fixme: else ?
    m_queue.removeAll(tile_spec);
  }
}

void
QcWmtsTileFetcher::request_next_tile()
{
  // qInfo();

  QMutexLocker mutex_locker(&m_queue_mutex);

  if (!m_enabled || m_queue.isEmpty())
    return;

  QcTileSpec tile_spec = m_queue.takeFirst();

  qInfo() << tile_spec;
  QcWmtsReply *wmts_reply = get_tile_image(tile_spec);

  // If the request is already finished then handle it
  // Else connect the finished signal
  if (wmts_reply->is_finished()) {
    handle_reply(wmts_reply, tile_spec);
  } else {
    connect(wmts_reply, SIGNAL(finished()),
	    this, SLOT(finished()),
	    Qt::QueuedConnection);
    m_invmap.insert(tile_spec, wmts_reply);
  }

  if (m_queue.isEmpty())
    m_timer.stop();
}

void
QcWmtsTileFetcher::finished()
{
  qInfo();

  QMutexLocker mutex_locker(&m_queue_mutex);

  QcWmtsReply *wmts_reply = qobject_cast<QcWmtsReply *>(sender());
  if (!wmts_reply) // Fixme: when ?
    return;

  QcTileSpec tile_spec = wmts_reply->tile_spec();

  if (!m_invmap.contains(tile_spec)) { // Fixme: when ?
    wmts_reply->deleteLater();
    return;
  }

  m_invmap.remove(tile_spec);

  handle_reply(wmts_reply, tile_spec);
}

void
QcWmtsTileFetcher::timerEvent(QTimerEvent * event)
{
  qInfo();
  if (event->timerId() != m_timer.timerId()) { // Fixme: when ?
    QObject::timerEvent(event);
    return;
  } else if (m_queue.isEmpty()) {
    m_timer.stop();
    return;
  } else
    request_next_tile();
}

void
QcWmtsTileFetcher::handle_reply(QcWmtsReply * wmts_reply, const QcTileSpec & tile_spec)
{
  qInfo();

  if (!m_enabled) {
    wmts_reply->deleteLater();
    return;
  }

  // emit signal according to the reply status
  if (wmts_reply->error() == QcWmtsReply::NoError) {
    qInfo() << "emit tile_finished";
    emit tile_finished(tile_spec, wmts_reply->map_image_data(), wmts_reply->map_image_format());
  } else {
    qInfo() << "emit tile_error";
    emit tile_error(tile_spec, wmts_reply->error_string());
  }

  wmts_reply->deleteLater();
}

/**************************************************************************************************/

// #include "wmts_tile_fetcher.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
