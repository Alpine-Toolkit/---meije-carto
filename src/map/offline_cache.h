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

/**************************************************************************************************/

#ifndef __OFFLINE_CACHE_H__
#define __OFFLINE_CACHE_H__

/**************************************************************************************************/

#include <QHash>
#include <QSharedPointer>
#include <QString>

#include "qtcarto_global.h"
#include "map/tile_spec.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

/*
 * count tile
 * per layer
 *
 */

class QcOfflineCachedTileDisk
{
public:
  QcOfflineCachedTileDisk();
  QcOfflineCachedTileDisk(const QcOfflineCachedTileDisk & other);

  QcOfflineCachedTileDisk & operator=(const QcOfflineCachedTileDisk & other);

public:
  QcTileSpec tile_spec;
  QString filename;
  QString format;
};

class QC_EXPORT QcOfflineTileCache // : public QObject
{
  // Q_OBJECT

 public:
  QcOfflineTileCache(const QString & directory = QString());
  ~QcOfflineTileCache();

  void clear_all();

  bool contains(const QcTileSpec & tile_spec) const;
  // QSharedPointer<QcOfflineCachedTileDisk> get(const QcTileSpec & tile_spec); //  const
  QcOfflineCachedTileDisk get(const QcTileSpec & tile_spec); //  const
  void insert(const QcTileSpec & tile_spec, const QByteArray & bytes, const QString & format);

 private:
  void load_tiles();
  void add_to_disk_cache(const QcTileSpec & tile_spec, const QString & filename);

 private:
  QString m_directory;
  // QHash<QcTileSpec, QSharedPointer<QcOfflineCachedTileDisk>> m_offline_cache;
  QHash<QcTileSpec, QcOfflineCachedTileDisk> m_offline_cache;
};

// QC_END_NAMESPACE

#endif /* __OFFLINE_CACHE_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
