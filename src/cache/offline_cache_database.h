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

#ifndef __OFFLINE_CACHE_DATABASE_H__
#define __OFFLINE_CACHE_DATABASE_H__

/**************************************************************************************************/

#include "wmts/tile_spec.h"
#include "database/database.h"

#include <QHash>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcOfflineCacheDatabase : public QcSqliteDatabase
{
public:
  QcOfflineCacheDatabase(const QString & sqlite_path);
  ~QcOfflineCacheDatabase();

  void insert_tile(const QcTileSpec & tile_spec);
  int has_tile(const QcTileSpec & tile_spec);
  void delete_tile(const QcTileSpec & tile_spec);

private:
  void create_tables();

  void init_cache();
  void load_providers();
  void load_map_levels();
  void init_version();
  int get_provider_id(const QString & provider);
  unsigned int hash_tile_spec(int provider_id, int map_id, int level);
  int get_map_level_id(const QcTileSpec & tile_spec);
  QString tile_where_clause(const QcTileSpec & tile_spec);

private:
  QHash<QString, int> m_providers;
  QHash<unsigned int, int> m_map_levels;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __OFFLINE_CACHE_DATABASE_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
