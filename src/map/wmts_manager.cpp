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

#include "wmts_manager.h"
#include "map_view.h" // circular

#include <QDir>
#include <QLocale>
#include <QStandardPaths>
#include <QTimer>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

QcWmtsManager::QcWmtsManager(const QString & plugin_name)
  : QObject(),
    m_plugin_name(plugin_name),
    m_tile_cache(nullptr), // must call set_tile_fetcher() !!!
    m_tile_fetcher(nullptr) // created by a call to tile_cache()
{}

/*!
  Destroys this mapping manager.
*/
QcWmtsManager::~QcWmtsManager()
{
  if (m_tile_cache)
    delete m_tile_cache;
}

void
QcWmtsManager::set_tile_fetcher(QcWmtsTileFetcher * tile_fetcher)
{
  m_tile_fetcher = tile_fetcher;

  qRegisterMetaType<QcTileSpec>();

  // Connect tile fetcher signals
  connect(m_tile_fetcher, SIGNAL(tile_finished(QcTileSpec, QByteArray, QString)),
	  this, SLOT(fetcher_tile_finished(QcTileSpec, QByteArray, QString)),
	  Qt::QueuedConnection);
  connect(m_tile_fetcher, SIGNAL(tile_error(QcTileSpec, QString)),
	  this, SLOT(fetcher_tile_error(QcTileSpec, QString)),
	  Qt::QueuedConnection);

  // engine_initialized();
}

QcWmtsTileFetcher *
QcWmtsManager::tile_fetcher()
{
  return m_tile_fetcher;
}

void
QcWmtsManager::set_tile_cache(QcFileTileCache * cache)
{
  // Fixme: delete, legacy from Qt ???
  Q_ASSERT_X(!m_tile_cache, Q_FUNC_INFO, "This should be called only once");
  m_tile_cache = cache;
}

QcFileTileCache *
QcWmtsManager::tile_cache()
{
  if (!m_tile_cache) {
    QString cache_directory = QcFileTileCache::base_cache_directory() + QDir::separator() + m_plugin_name;
    m_tile_cache = new QcFileTileCache(cache_directory);
  }
  return m_tile_cache;
}

void
QcWmtsManager::remove_tile_spec(const QcTileSpec & tile_spec)
{
  // Remove tile_spec in sets

  QcMapViewPointerSet map_views = m_tile_hash.value(tile_spec);

  // Fixme: inplace update ?
  for (auto map_view : map_views) {
    QcTileSpecSet tile_set = m_map_view_hash.value(map_view);
    tile_set.remove(tile_spec);
    if (tile_set.isEmpty())
      m_map_view_hash.remove(map_view);
    else
      m_map_view_hash.insert(map_view, tile_set);
  }

  m_tile_hash.remove(tile_spec);
}

void
QcWmtsManager::release_map(QcMapView * map_view)
{
  m_map_view_hash.remove(map_view);

  // Update m_tile_hash
  QHash<QcTileSpec, QcMapViewPointerSet > new_tile_hash = m_tile_hash;
  // for (auto & tile_spec : m_tile_hash.keys())
  typedef QHash<QcTileSpec, QcMapViewPointerSet >::const_iterator hash_iterator;
  hash_iterator iter = m_tile_hash.constBegin();
  hash_iterator iter_end = m_tile_hash.constEnd();
  for (; iter != iter_end; ++iter) { // Fixme: cxx11
    QcMapViewPointerSet map_views = iter.value();
    if (map_views.contains(map_view)) {
      map_views.remove(map_view);
      if (map_views.isEmpty())
	new_tile_hash.remove(iter.key());
      else
	new_tile_hash.insert(iter.key(), map_views); // Fixme: inplace update ?
    }
  }
  m_tile_hash = new_tile_hash;
}

void
QcWmtsManager::update_tile_requests(QcMapView * map_view,
				    const QcTileSpecSet & tiles_added,
				    const QcTileSpecSet & tiles_removed)
{
  typedef QcTileSpecSet::const_iterator tile_iter;
  tile_iter iter, iter_end;

  // add and remove tiles from tileset for this map_view
  QcTileSpecSet old_tiles = m_map_view_hash.value(map_view);
  old_tiles += tiles_added;
  old_tiles -= tiles_removed;
  m_map_view_hash.insert(map_view, old_tiles);

  // add and remove map from mapset for the tiles

  // Fixme: duplicated code, inplace update ?
  QcTileSpecSet canceled_tiles;
  for (auto & tile_spec : tiles_removed) {
    QcMapViewPointerSet map_view_set = m_tile_hash.value(tile_spec);
    map_view_set.remove(map_view);
    if (map_view_set.isEmpty()) {
      m_tile_hash.remove(tile_spec);
      canceled_tiles.insert(tile_spec);
    } else {
      m_tile_hash.insert(tile_spec, map_view_set);
    }
  }

  QcTileSpecSet requested_tiles;
  for (auto & tile_spec : tiles_added) {
    QcMapViewPointerSet map_view_set = m_tile_hash.value(tile_spec);
    if (map_view_set.isEmpty()) {
      requested_tiles.insert(tile_spec);
    }
    map_view_set.insert(map_view);
    m_tile_hash.insert(tile_spec, map_view_set);
  }

  // Fixme: why ?
  canceled_tiles -= requested_tiles;

  // async call
  // qInfo() << "async call update_tile_requests +" << requested_tiles << "-" << canceled_tiles;
  QMetaObject::invokeMethod(m_tile_fetcher, "update_tile_requests",
			    Qt::DirectConnection,
			    // Fixme: segfault requested_tiles ???
  			    // Qt::QueuedConnection,
  			    Q_ARG(QSet<QcTileSpec>, requested_tiles), // QcTileSpecSet
  			    Q_ARG(QSet<QcTileSpec>, canceled_tiles));
  qInfo() << "end of";
}

// Fixme: name
void
QcWmtsManager::fetcher_tile_finished(const QcTileSpec & tile_spec, const QByteArray & bytes, const QString & format)
{
  qInfo();
  // Is tile requested by a map view ?
  if (m_tile_hash.contains(tile_spec)) {
    QcMapViewPointerSet map_views = m_tile_hash.value(tile_spec);
    remove_tile_spec(tile_spec);
    tile_cache()->insert(tile_spec, bytes, format);
    for (QcMapView * map_view : map_views)
      map_view->request_manager()->tile_fetched(tile_spec);
  } else
    qInfo() << "any client" << tile_spec;
}

void
QcWmtsManager::fetcher_tile_error(const QcTileSpec & tile_spec, const QString & error_string)
{
  qInfo();
  QcMapViewPointerSet map_views = m_tile_hash.value(tile_spec);
  remove_tile_spec(tile_spec);

  for (QcMapView * map_view : map_views)
    map_view->request_manager()->tile_error(tile_spec, error_string);

  emit tile_error(tile_spec, error_string);
}

QSharedPointer<QcTileTexture>
QcWmtsManager::get_tile_texture(const QcTileSpec & tile_spec)
{
  return m_tile_cache->get(tile_spec);
}

void
QcWmtsManager::dump() const
{
  qInfo() << "Dump";
  for (auto & tile_spec : m_tile_hash.keys())
    qInfo() << tile_spec << "--->" << m_tile_hash[tile_spec];
  for (auto & map_view : m_map_view_hash.keys())
    qInfo() << map_view << "--->" << m_map_view_hash[map_view];
}

/**************************************************************************************************/

// #include "wmts_manager.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
