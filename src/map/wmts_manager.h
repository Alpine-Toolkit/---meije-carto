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

#ifndef __WMTS_MANAGER_H__
#define __WMTS_MANAGER_H__

/**************************************************************************************************/

#include <QHash>
#include <QObject>
#include <QPair>
#include <QSet>
#include <QSize>

#include "qtcarto_global.h"
#include "file_tile_cache.h"
// #include "map_view.h" // circular
#include "wmts_tile_fetcher.h"

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcMapView;
typedef QSet<QcMapView *> QcMapViewPointerSet;

/**************************************************************************************************/

/*! This class implements a WMTS Manager for a WTMS provide.
 *
 * It manages requests from several concurrent map views and dispatch
 * them to the WTMS Tile Fetcher and store tile images in a cache.
 *
 * It notify the WTMS Request Manager when a tile is fetched or failed.
 */
class QC_EXPORT QcWmtsManager : public QObject
{
  Q_OBJECT

 public:
  enum CacheArea {
    DiskCache = 0x01,
    MemoryCache = 0x02,
    AllCaches = 0xFF
  };
  Q_DECLARE_FLAGS(CacheAreas, CacheArea)

 public:
  explicit QcWmtsManager();
  virtual ~QcWmtsManager();

  void release_map(QcMapView * map_view);

  QcWmtsTileFetcher * tile_fetcher();
  QcFileTileCache * tile_cache();

  void update_tile_requests(QcMapView * map_view,
			    const QcTileSpecSet & tiles_added,
			    const QcTileSpecSet & tiles_removed);

  QSharedPointer<QcTileTexture> get_tile_texture(const QcTileSpec & tile_spec);

  void dump() const;

  // QSize tileSize() const;
  // QcWmtsManager::CacheAreas cache_hint() const;

 private slots:
  // Fixme: name
  void fetcher_tile_finished(const QcTileSpec & tile_spec, const QByteArray & bytes, const QString & format);
  void fetcher_tile_error(const QcTileSpec & tile_spec, const QString & error_string);

 signals:
  void tile_error(const QcTileSpec & tile_spec, const QString & error_string);
  void tile_version_changed();

  // protected:
 public:
  void set_tile_fetcher(QcWmtsTileFetcher * tile_fetcher);
  void set_tile_cache(QcFileTileCache * cache);
  // void set_tile_size(const QSize &tileSize);
  // void set_cache_hint(QcWmtsManager::CacheAreas cacheHint);

 private:
  void remove_tile_spec(const QcTileSpec & tile_spec);

 private:
  QHash<QcMapView *, QcTileSpecSet > m_map_view_hash;
  QHash<QcTileSpec, QcMapViewPointerSet > m_tile_hash;
  QcFileTileCache * m_tile_cache;
  QcWmtsTileFetcher * m_tile_fetcher;
  // QSize m_tile_size;
  // QcWmtsManager::CacheAreas m_cache_hint;

  Q_DISABLE_COPY(QcWmtsManager);

  friend class QcWmtsTileFetcher;
};

// Q_DECLARE_OPERATORS_FOR_FLAGS(QcWmtsManager::CacheAreas)

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __WMTS_MANAGER_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
