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

  QcMapViewLayerPointerSet map_view_layers = m_tile_hash.value(tile_spec);

  // Fixme: inplace update ?
  for (auto map_view_layer : map_view_layers) {
    QcTileSpecSet tile_set = m_map_view_layer_hash.value(map_view_layer);
    tile_set.remove(tile_spec);
    if (tile_set.isEmpty())
      m_map_view_layer_hash.remove(map_view_layer);
    else
      m_map_view_layer_hash.insert(map_view_layer, tile_set);
  }

  m_tile_hash.remove(tile_spec);
}

void
QcWmtsManager::release_map(QcMapViewLayer * map_view_layer)
{
  m_map_view_layer_hash.remove(map_view_layer);

  // Update m_tile_hash
  QHash<QcTileSpec, QcMapViewLayerPointerSet > new_tile_hash = m_tile_hash;
  // for (auto & tile_spec : m_tile_hash.keys())
  typedef QHash<QcTileSpec, QcMapViewLayerPointerSet >::const_iterator hash_iterator;
  hash_iterator iter = m_tile_hash.constBegin();
  hash_iterator iter_end = m_tile_hash.constEnd();
  for (; iter != iter_end; ++iter) { // Fixme: cxx11
    QcMapViewLayerPointerSet map_view_layers = iter.value();
    if (map_view_layers.contains(map_view_layer)) {
      map_view_layers.remove(map_view_layer);
      if (map_view_layers.isEmpty())
	new_tile_hash.remove(iter.key());
      else
	new_tile_hash.insert(iter.key(), map_view_layers); // Fixme: inplace update ?
    }
  }
  m_tile_hash = new_tile_hash;
}

void
QcWmtsManager::update_tile_requests(QcMapViewLayer * map_view_layer,
				    const QcTileSpecSet & tiles_added,
				    const QcTileSpecSet & tiles_removed)
{
  typedef QcTileSpecSet::const_iterator tile_iter;
  tile_iter iter, iter_end;

  // add and remove tiles from tileset for this map_view_layer
  QcTileSpecSet old_tiles = m_map_view_layer_hash.value(map_view_layer);
  old_tiles += tiles_added;
  old_tiles -= tiles_removed;
  m_map_view_layer_hash.insert(map_view_layer, old_tiles);

  // add and remove map from mapset for the tiles

  // Fixme: duplicated code, inplace update ?
  QcTileSpecSet canceled_tiles;
  for (auto & tile_spec : tiles_removed) {
    QcMapViewLayerPointerSet map_view_layer_set = m_tile_hash.value(tile_spec);
    map_view_layer_set.remove(map_view_layer);
    if (map_view_layer_set.isEmpty()) {
      m_tile_hash.remove(tile_spec);
      canceled_tiles.insert(tile_spec);
    } else {
      m_tile_hash.insert(tile_spec, map_view_layer_set);
    }
  }

  QcTileSpecSet requested_tiles;
  for (auto & tile_spec : tiles_added) {
    QcMapViewLayerPointerSet map_view_layer_set = m_tile_hash.value(tile_spec);
    if (map_view_layer_set.isEmpty()) {
      requested_tiles.insert(tile_spec);
    }
    map_view_layer_set.insert(map_view_layer);
    m_tile_hash.insert(tile_spec, map_view_layer_set);
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
    QcMapViewLayerPointerSet map_view_layers = m_tile_hash.value(tile_spec);
    remove_tile_spec(tile_spec);
    tile_cache()->insert(tile_spec, bytes, format);
    for (QcMapViewLayer * map_view_layer : map_view_layers)
      map_view_layer->request_manager()->tile_fetched(tile_spec);
  } else
    qInfo() << "any client" << tile_spec;
}

void
QcWmtsManager::fetcher_tile_error(const QcTileSpec & tile_spec, const QString & error_string)
{
  qInfo();
  QcMapViewLayerPointerSet map_view_layers = m_tile_hash.value(tile_spec);
  remove_tile_spec(tile_spec);

  for (QcMapViewLayer * map_view_layer : map_view_layers)
    map_view_layer->request_manager()->tile_error(tile_spec, error_string);

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
  for (auto & map_view_layer : m_map_view_layer_hash.keys())
    qInfo() << map_view_layer << "--->" << m_map_view_layer_hash[map_view_layer];
}

/**************************************************************************************************/

// #include "wmts_manager.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
