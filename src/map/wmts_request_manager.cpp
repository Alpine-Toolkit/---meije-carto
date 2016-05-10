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

#include "map/wmts_request_manager.h"
#include "map/map_view.h" // circular

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcRetryFuture::QcRetryFuture(const QcTileSpec & tile_spec, QcMapViewLayer * map_view_layer, QcWmtsManager * wmts_manager)
  : QObject(),
    m_tile_spec(tile_spec),
    m_map_view_layer(map_view_layer),
    m_wmts_manager(wmts_manager)
{}

void
QcRetryFuture::retry()
{
  if (!m_wmts_manager.isNull()) {
    QcTileSpecSet request_tiles = {m_tile_spec};
    QcTileSpecSet cancel_tiles;
    m_wmts_manager->update_tile_requests(m_map_view_layer, request_tiles, cancel_tiles);
  }
}

/**************************************************************************************************/

QcWmtsRequestManager::QcWmtsRequestManager(QcMapViewLayer * map_view_layer, QcWmtsManager * wmts_manager)
  : m_map_view_layer(map_view_layer),
    m_wmts_manager(wmts_manager)
{}

QcWmtsRequestManager::~QcWmtsRequestManager()
{}

/*! Request a new tile sets.
 *
 *  It performs 3 actions:
 *   - compute the canceled tiles sets
 *   - ask the WMTS Manager for cached tiles
 *   - update the tile request to the WTMS Manager
 *
 *  It returns cached tile textures.
 */
QList<QSharedPointer<QcTileTexture> >
QcWmtsRequestManager::request_tiles(const QcTileSpecSet & tile_specs)
{
  // Fixme: m_wmts_manager.isNull()?

  QcTileSpecSet canceled_tiles = m_requested - tile_specs;
  QcTileSpecSet requested_tiles = tile_specs - m_requested;

  // Remove tiles in cache from request tiles
  QcTileSpecSet cached_tiles;
  QList<QSharedPointer<QcTileTexture> > cached_textures;
  if (!m_wmts_manager.isNull()) {
    for (auto & tile_spec : requested_tiles) {
      QSharedPointer<QcTileTexture> texture = m_wmts_manager->get_tile_texture(tile_spec);
      if (texture) {
	cached_tiles.insert(tile_spec);
	cached_textures << texture;
      }
    }
  }
  requested_tiles -= cached_tiles;

  m_requested -= canceled_tiles;
  m_requested += requested_tiles;

  qInfo() << "currently requested" << m_requested << "\ncached" << cached_tiles << "\n+" << requested_tiles << "\n-" << canceled_tiles;

  if ((!requested_tiles.isEmpty() || !canceled_tiles.isEmpty())
      && (!m_wmts_manager.isNull())) {
    m_wmts_manager->update_tile_requests(m_map_view_layer, requested_tiles, canceled_tiles);

    // Fixme: ??? place ???
    // Remove any cancelled tiles from the error retry hash to avoid
    // re-using the numbers for a totally different request cycle.
    for (auto & tile_spec : canceled_tiles) {
      m_retries.remove(tile_spec);
      m_futures.remove(tile_spec);
    }
  }

  return cached_textures;
}

/*! Notify the map view that a tile is fetched.
 *
 */
void
QcWmtsRequestManager::tile_fetched(const QcTileSpec & tile_spec)
{
  qInfo();
  m_map_view_layer->update_tile(tile_spec);
  m_requested.remove(tile_spec);
  m_retries.remove(tile_spec);
  m_futures.remove(tile_spec);
}

/*! Retry to fetch an errored tile request.
 *
 */
void
QcWmtsRequestManager::tile_error(const QcTileSpec & tile_spec, const QString & error_string)
{
  qInfo();
  if (m_requested.contains(tile_spec)) {
    int count = m_retries.value(tile_spec, 0);
    m_retries.insert(tile_spec, count + 1);

    if (count >= 5) {
      qWarning("QcWmtsRequestManager: Failed to fetch tile (%d,%d,%d) 5 times, giving up. "
	       "Last error message was: '%s'",
           tile_spec.x(), tile_spec.y(), tile_spec.level(), qPrintable(error_string)); // Fixme: size_t int
      m_requested.remove(tile_spec);
      m_retries.remove(tile_spec);
      m_futures.remove(tile_spec);
    } else {
      qDebug() << "Retry x" << count << tile_spec;
      // Exponential time backoff when retrying
      int delay = (1 << count) * 500;
      QSharedPointer<QcRetryFuture> future(new QcRetryFuture(tile_spec, m_map_view_layer, m_wmts_manager));
      m_futures.insert(tile_spec, future);
      QTimer::singleShot(delay, future.data(), SLOT(retry()));
      // Passing .data() to singleShot is ok -- Qt will clean up the connection if the target qobject is deleted
    }
  }
}

/*! Get the tile texture from the WTMS Manager cache.
 *
 */
QSharedPointer<QcTileTexture>
QcWmtsRequestManager::tile_texture(const QcTileSpec & tile_spec)
{
  if (m_wmts_manager) // Fixme: isNull
    return m_wmts_manager->get_tile_texture(tile_spec);
  else
    return QSharedPointer<QcTileTexture>();
}

/**************************************************************************************************/

// #include "wmts_request_manager.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
