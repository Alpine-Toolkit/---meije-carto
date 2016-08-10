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

#include "offline_cache.h"

// #include "file_tile_cache.h"
#include "tile_image.h"

#include <QDir>

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcOfflineCachedTileDisk::QcOfflineCachedTileDisk()
  : tile_spec(),
    filename(),
    format()
{}

QcOfflineCachedTileDisk::QcOfflineCachedTileDisk(const QcOfflineCachedTileDisk & other)
  : tile_spec(other.tile_spec),
    filename(other.filename),
    format(other.format)
{}

QcOfflineCachedTileDisk &
QcOfflineCachedTileDisk::operator=(const QcOfflineCachedTileDisk & other)
{
  if (this != &other) {
    tile_spec = other.tile_spec;
    filename = other.filename;
    format = other.format;
  }

  return *this;
}

/**************************************************************************************************/

QcOfflineTileCache::QcOfflineTileCache(const QString & directory)
  : m_directory(directory),
    m_database(nullptr)
{
  QDir::root().mkpath(m_directory);
  QString sqlite_file_path = QDir(directory).absoluteFilePath(QStringLiteral("offline_cache.sqlite"));

  m_database = new QcOfflineCacheDatabase(sqlite_file_path);
}

QcOfflineTileCache::~QcOfflineTileCache()
{}

void
QcOfflineTileCache::clear_all()
{
  QStringList formats;
  formats << QLatin1Literal("*.*");

  // Fixme:
  for (int level = 0; level < 20; level++) {
    QDir directory(m_directory + QDir::separator() + QString::number(level));
    directory.setNameFilters(formats);
    directory.setFilter(QDir::Files);
    for(QString file : directory.entryList())
    directory.remove(file);
  }

  // Fixme: clear db
}

void
QcOfflineTileCache::load_tiles()
{
  QStringList formats;
  formats << QLatin1Literal("*.*");

  // Fixme:
  QDir root_directory(m_directory);
  for (int level = 0; level < 20; level++) {
    QDir directory(m_directory + QDir::separator() + QString::number(level));
    QStringList files = directory.entryList(formats, QDir::Files);
    // qInfo() << "level" << level << "Number of tiles" << files.size();

    for (const auto & relative_filename : files) {
      QcTileSpec tile_spec = filename_to_tile_spec(relative_filename);
      if (tile_spec.level() == -1)
        continue;
      QString filename = directory.filePath(relative_filename); // Fixme: use memory!
      // qInfo() << filename;
      add_to_disk_cache(tile_spec, filename);
    }
  }

  // for (const auto & key : m_offline_cache.keys())
  //   qInfo() << "key" << key;
}

bool
QcOfflineTileCache::contains(const QcTileSpec & tile_spec) const
{
  qInfo() << tile_spec;

  return m_database->has_tile(tile_spec) > 0;

  // return m_offline_cache.contains(tile_spec);
}

// QSharedPointer<QcOfflineCachedTileDisk>
QcOfflineCachedTileDisk
QcOfflineTileCache::get(const QcTileSpec & tile_spec)
{
  // Fixme: if not in cache ?

  QString directory = m_directory + QDir::separator() + QString::number(tile_spec.level());
  const QString format = "jpeg"; // Fixme:
  QString filename = tile_spec_to_filename(tile_spec, format, directory);

  QcOfflineCachedTileDisk tile_directory;
  tile_directory.tile_spec = tile_spec;
  tile_directory.filename = filename;

  return tile_directory;

  // return m_offline_cache.value(tile_spec);

  /*
  if (m_offline_cache.contains(tile_spec)) {
    return m_offline_cache.value(tile_spec);
  } else {
    qInfo() << "Missing tile" << tile_spec;
    return QSharedPointer<QcOfflineCachedTileDisk>(nullptr);
  }
  */
}

void
QcOfflineTileCache::insert(const QcTileSpec & tile_spec, const QByteArray & bytes, const QString & format)
{
  if (bytes.isEmpty())
    return;

  QString directory = m_directory + QDir::separator() + QString::number(tile_spec.level());
  QDir::root().mkpath(directory);
  QString filename = tile_spec_to_filename(tile_spec, format, directory);
  write_tile_image(filename, bytes);

  m_database->insert_tile(tile_spec);
}

void
QcOfflineTileCache::add_to_disk_cache(const QcTileSpec & tile_spec, const QString & filename)
{
  // QSharedPointer<QcOfflineCachedTileDisk> tile_directory(new QcOfflineCachedTileDisk);
  // tile_directory->tile_spec = tile_spec;
  // tile_directory->filename = filename;

  QcOfflineCachedTileDisk tile_directory;
  tile_directory.tile_spec = tile_spec;
  tile_directory.filename = filename;
  // tile_spec.format =

  if (!m_offline_cache.contains(tile_spec)) {
    // qWarning() << "insert" << tile_spec;
    m_offline_cache.insert(tile_spec, tile_directory);
  } else
    qWarning() << "Hash collision" << tile_spec;
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
