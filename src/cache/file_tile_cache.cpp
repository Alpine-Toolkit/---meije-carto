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

#include "file_tile_cache.h"
#include "tile_image.h"

#include <QDebug>
#include <QDir>
#include <QMetaType>
#include <QPixmap>
#include <QStandardPaths>

// Q_DECLARE_METATYPE(QList<QcTileSpec>)
// Q_DECLARE_METATYPE(QcTileSpecSet)

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcTileTexture::QcTileTexture()
  : texture_bound(false)
{}

QcTileTexture::~QcTileTexture()
{}

class QcCachedTileMemory
{
public:
  ~QcCachedTileMemory() {
    if (cache)
      cache->evict_from_memory_cache(this);
  }

  QcTileSpec tile_spec;
  QcFileTileCache *cache;
  QByteArray bytes;
  QString format;
};

/**************************************************************************************************/

void
QCache3QTileEvictionPolicy::about_to_be_removed(const QcTileSpec & key, QSharedPointer<QcCachedTileDisk> obj)
{
  Q_UNUSED(key);
  // set the cache pointer to zero so we can't call evict_from_disk_cache
  obj->cache = nullptr;
}

void
QCache3QTileEvictionPolicy::about_to_be_evicted(const QcTileSpec & key, QSharedPointer<QcCachedTileDisk> obj)
{
  Q_UNUSED(key);
  Q_UNUSED(obj);
  // leave the pointer set if it's a real eviction
}

/**************************************************************************************************/

QcCachedTileDisk::~QcCachedTileDisk()
{
  if (cache)
    cache->evict_from_disk_cache(this);
}

/**************************************************************************************************/

// Fixme: export
constexpr int KILO = 1000;
constexpr int KILO2 = 1024;
constexpr int MEGA = KILO * KILO;
constexpr int MEGA2 = KILO2 * KILO2;

constexpr int MAX_DISK_USAGE = 20 * MEGA2;
constexpr int MAX_MEMORY_USAGE = 3 * MEGA2;
constexpr int EXTRA_TEXTURE_USAGE = 6 * MEGA2;

constexpr int NUMBER_OF_QUEUES = 4;

/**************************************************************************************************/

QcFileTileCache::QcFileTileCache(const QString & directory)
  : QObject(),
    m_offline_cache(nullptr),
    m_directory(directory), m_min_texture_usage(0), m_extra_texture_usage(0)
{
  const QString base_path = base_cache_directory();

  if (m_directory.isEmpty()) {
    m_directory = base_path;
  }

  QDir::root().mkpath(m_directory);

  // default values
  set_max_disk_usage(MAX_DISK_USAGE);
  set_max_memory_usage(MAX_MEMORY_USAGE);
  set_extra_texture_usage(EXTRA_TEXTURE_USAGE);

  load_tiles();

  QString offline_cache_directory = m_directory + QDir::separator() + QLatin1Literal("offline");
  m_offline_cache = new QcOfflineTileCache(offline_cache_directory);
}

QcFileTileCache::~QcFileTileCache()
{
  // qInfo() << "Serialize cache queue";

  // For each disk cache queue write the list of filenames to a file
  QDir directory(m_directory);
  for (int i = 1; i <= NUMBER_OF_QUEUES; i++) {
    QString filename = queue_filename(i);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)){
      qWarning() << "Unable to write tile cache file " << filename;
      continue;
    }

    QList<QSharedPointer<QcCachedTileDisk> > queue;
    m_disk_cache.serialize_queue(i, queue);
    for(const auto & tile : queue)
      if (!tile.isNull()) {
	// we just want the filename here, not the full path
        const QString & tile_filename = tile->filename;
	int index = tile_filename.lastIndexOf(QLatin1Char('/'));
	QByteArray filename = tile_filename.mid(index + 1).toLatin1() + '\n';
	file.write(filename);
      }

    file.close();
  }

  delete m_offline_cache;
}

QString
QcFileTileCache::base_cache_directory()
{
  // Try the shared cache first and use a specific directory. (e.g. ~/.cache/QtLocation)
  // If this is not supported by the platform, use the application-specific cache location.
  // (e.g. ~/.cache/<app_name>/QtLocation)

  // /data/user/11/org.qtproject.example.mapviewer_atk/cache/QtCarto/
#ifdef ANDROID
  QString generic_data_location_path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
  // qInfo() << "GenericDataLocation:" << generic_data_location_path;
  QString application_user_directory_path = QDir(generic_data_location_path).filePath("alpine-toolkit");
  QString directory = application_user_directory_path + QDir::separator() + QLatin1Literal("cache");
#else
  QString directory = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation);
#endif
  // qInfo() << "base_cache_directory" << directory;

  // Check we can write on the cache directory
  if (!directory.isEmpty()) {
    // The shared cache may not be writable when application isolation is enforced.
    static bool cache_directory_writable = false; // static for later use
    static bool cache_directory_writable_checked = false;
    if (!cache_directory_writable_checked) {
      cache_directory_writable_checked = true;
      QDir::root().mkpath(directory);
      QFile write_test_file(QDir(directory).filePath(QLatin1Literal("qt_cache_check")));
      cache_directory_writable = write_test_file.open(QIODevice::WriteOnly);
      if (cache_directory_writable)
	write_test_file.remove();
    }
    if (!cache_directory_writable)
      directory = QString();
  }

  if (directory.isEmpty())
    directory = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

  if (!directory.endsWith(QLatin1Char('/')))
    directory += QLatin1Char('/');
  directory += QLatin1Literal("QtCarto/");

  // qInfo() << "base_cache_directory" << directory;
  return directory;
}

/*! Clear the cache and remove cached files on disk
 *
 */
void
QcFileTileCache::clear_all()
{
  m_texture_cache.clear();
  m_memory_cache.clear();
  m_disk_cache.clear();

  QStringList string_list;
  string_list << QLatin1Literal("*-*-*-*.*"); // tile pattern
  string_list << QLatin1Literal("queue?");
  QDir directory(m_directory);
  directory.setNameFilters(string_list);
  directory.setFilter(QDir::Files);
  for(QString file : directory.entryList())
    directory.remove(file);
}

QString
QcFileTileCache::queue_filename(int i) const
{
  // queue%u
  return QDir(m_directory).filePath(QLatin1Literal("queue") + QString::number(i));
}

void
QcFileTileCache::load_tiles()
{
  QStringList formats;
  formats << QLatin1Literal("*.*");

  QDir directory(m_directory);
  QStringList files = directory.entryList(formats, QDir::Files);

  // Method:
  // 1. read each queue file then, if each file exists, deserialize the data into the appropriate cache queue.
  for (int i = 1; i <= NUMBER_OF_QUEUES; i++) {
    QFile file(queue_filename(i));
    if (!file.open(QIODevice::ReadOnly))
      continue;
    QList<QSharedPointer<QcCachedTileDisk> > queue;
    QList<QcTileSpec> tile_specs;
    QList<int> costs;
    while (!file.atEnd()) {
      QByteArray line = file.readLine().trimmed();
      QString filename = QString::fromLatin1(line.constData(), line.length());
      if (directory.exists(filename)){
	files.removeOne(filename);
	QcTileSpec tile_spec = filename_to_tile_spec(filename);
	// qInfo() << "Load" << tile_spec;
	if (tile_spec.level() == -1) // Fixme: when ?
	  continue;
	QSharedPointer<QcCachedTileDisk> tile_disk(new QcCachedTileDisk);
	tile_disk->filename = directory.filePath(filename);
	tile_disk->cache = this;
	tile_disk->tile_spec = tile_spec;
	tile_specs.append(tile_spec);
	queue.append(tile_disk);
	costs.append(QFileInfo(filename).size());
      }
    }
    file.close();
    m_disk_cache.deserialize_queue(i, tile_specs, queue, costs);
  }

  // 2. remaining tiles that aren't registered in a queue get pushed into cache here
  // this is a backup, in case the queue manifest files get deleted or out of sync due to
  // the application not closing down properly
  // Fixme: or for off-line cache
  for (const auto & relative_filename : files) {
    QcTileSpec tile_spec = filename_to_tile_spec(relative_filename);
    if (tile_spec.level() == -1)
      continue;
    QString filename = directory.filePath(relative_filename);
    add_to_disk_cache(tile_spec, filename);
  }
}

void
QcFileTileCache::print_stats()
{
  m_texture_cache.print_stats();
  m_memory_cache.print_stats();
  m_disk_cache.print_stats();
}

void
QcFileTileCache::handle_error(const QcTileSpec & tile_spec, const QString & error)
{
  qWarning() << "tile request error " << tile_spec << error;
}

void
QcFileTileCache::set_max_disk_usage(int disk_usage)
{
  m_disk_cache.set_max_cost(disk_usage);
}

int
QcFileTileCache::max_disk_usage() const
{
  return m_disk_cache.max_cost();
}

int
QcFileTileCache::disk_usage() const
{
  return m_disk_cache.total_cost();
}

void
QcFileTileCache::set_max_memory_usage(int memory_usage)
{
  m_memory_cache.set_max_cost(memory_usage);
}

int
QcFileTileCache::max_memory_usage() const
{
  return m_memory_cache.max_cost();
}

int
QcFileTileCache::memory_usage() const
{
  return m_memory_cache.total_cost();
}

void
QcFileTileCache::set_extra_texture_usage(int texture_usage)
{
  m_extra_texture_usage = texture_usage;
  m_texture_cache.set_max_cost(m_min_texture_usage + m_extra_texture_usage);
}

void
QcFileTileCache::set_min_texture_usage(int texture_usage)
{
  m_min_texture_usage = texture_usage;
  m_texture_cache.set_max_cost(m_min_texture_usage + m_extra_texture_usage);
}

int
QcFileTileCache::max_texture_usage() const
{
  return m_texture_cache.max_cost();
}

int
QcFileTileCache::min_texture_usage() const
{
  return m_min_texture_usage;
}

int
QcFileTileCache::texture_usage() const
{
  return m_texture_cache.total_cost();
}

QSharedPointer<QcTileTexture>
QcFileTileCache::get(const QcTileSpec & tile_spec)
{
  // Try texture cache
  QSharedPointer<QcTileTexture> tile_texture = m_texture_cache.object(tile_spec);
  if (tile_texture)
    return tile_texture;

  // Try memory cache
  QSharedPointer<QcCachedTileMemory> tile_memory = m_memory_cache.object(tile_spec);
  if (tile_memory)
    return load_from_memory(tile_memory);

  // Try disk cache
  QSharedPointer<QcCachedTileDisk> tile_directory = m_disk_cache.object(tile_spec);
  if (tile_directory)
    return load_from_disk(tile_directory->tile_spec, tile_directory->filename);

  // Try offline cache
  // QSharedPointer<QcOfflineCachedTileDisk> offline_tile = m_offline_cache->get(tile_spec);
  // if (offline_tile) {
  if (m_offline_cache->contains(tile_spec)) {
    QcOfflineCachedTileDisk offline_tile = m_offline_cache->get(tile_spec);
    // qInfo() << "In offline cache" << tile_spec;
    // return load_from_disk(offline_tile->tile_spec, offline_tile->filename);
    return load_from_disk(offline_tile.tile_spec, offline_tile.filename);
  }

  // else
  return QSharedPointer<QcTileTexture>();
}

QSharedPointer<QcTileTexture>
// QcFileTileCache::load_from_disk(const QSharedPointer<QcCachedTileDisk> & tile_directory)
QcFileTileCache::load_from_disk(const QcTileSpec & tile_spec, const QString & filename)
{
  // const QcTileSpec & tile_spec = tile_directory->tile_spec;
  // const QString & filename = tile_directory->filename;

  QByteArray bytes = read_tile_image(filename);

  // Load PNG, JPEG from bytes
  QImage image;
  if (image.loadFromData(bytes)) {
    const QString format = QFileInfo(filename).suffix();
    add_to_memory_cache(tile_spec, bytes, format);

    QSharedPointer<QcTileTexture> tile_texture = add_to_texture_cache(tile_spec, image);
    if (tile_texture) // Fixme: when ? memory overflow ?
      return tile_texture;
  } else
    handle_error(tile_spec, QLatin1Literal("Problem with tile image"));

  // else
  return QSharedPointer<QcTileTexture>(nullptr);
}

QSharedPointer<QcTileTexture>
QcFileTileCache::load_from_memory(const QSharedPointer<QcCachedTileMemory> & tile_memory)
{
  const QcTileSpec & tile_spec = tile_memory->tile_spec;

  // Fixme: duplicated code, excepted add_to_memory_cache
  QImage image;
  if (image.loadFromData(tile_memory->bytes)) {
    QSharedPointer<QcTileTexture> tile_texture = add_to_texture_cache(tile_spec, image);
    if (tile_texture)
      return tile_texture;
  } else
    handle_error(tile_spec, QLatin1Literal("Problem with tile image"));

  // else
 return QSharedPointer<QcTileTexture>(0);
}

void
QcFileTileCache::insert(const QcTileSpec & tile_spec, const QByteArray & bytes, const QString & format)
// Fixme:
// QcTiledMappingManagerEngine::CacheAreas areas
{
  if (bytes.isEmpty())
    return;

  // if (areas & QcTiledMappingManagerEngine::DiskCache) {
  QString filename = tile_spec_to_filename(tile_spec, format, m_directory);
  write_tile_image(filename, bytes);
  add_to_disk_cache(tile_spec, filename);
  // }

  // if (areas & QcTiledMappingManagerEngine::MemoryCache) {
  add_to_memory_cache(tile_spec, bytes, format);
  // }

  /* Inserts do not hit the texture cache -- this actually reduces overall
   * cache hit rates because many tiles come too late to be useful
   * and act as a poison
   */
}

void
QcFileTileCache::evict_from_disk_cache(QcCachedTileDisk * tile_directory)
{
  QFile::remove(tile_directory->filename);
}

void
QcFileTileCache::evict_from_memory_cache(QcCachedTileMemory * /* tile_memory  */)
{}

QSharedPointer<QcCachedTileDisk>
QcFileTileCache::add_to_disk_cache(const QcTileSpec & tile_spec, const QString  & filename)
{
  QSharedPointer<QcCachedTileDisk> tile_directory(new QcCachedTileDisk);
  tile_directory->tile_spec = tile_spec;
  tile_directory->filename = filename;
  tile_directory->cache = this;

  QFileInfo file_info(filename);
  int disk_cost = filename.size();
  m_disk_cache.insert(tile_spec, tile_directory, disk_cost);
  return tile_directory;
}

QSharedPointer<QcCachedTileMemory>
QcFileTileCache::add_to_memory_cache(const QcTileSpec & tile_spec, const QByteArray & bytes, const QString & format)
{
  QSharedPointer<QcCachedTileMemory> tile_memory(new QcCachedTileMemory);
  tile_memory->tile_spec = tile_spec;
  tile_memory->cache = this;
  tile_memory->bytes = bytes;
  tile_memory->format = format;

  int cost = bytes.size();
  m_memory_cache.insert(tile_spec, tile_memory, cost);

  return tile_memory;
}

QSharedPointer<QcTileTexture>
QcFileTileCache::add_to_texture_cache(const QcTileSpec & tile_spec, const QImage & image)
{
  QSharedPointer<QcTileTexture> tile_texture(new QcTileTexture);
  tile_texture->tile_spec = tile_spec;
  tile_texture->image = image;

  int texture_cost = image.width() * image.height() * image.depth() / 8;
  m_texture_cache.insert(tile_spec, tile_texture, texture_cost);

  return tile_texture;
}

/**************************************************************************************************/

// #include "file_tile_cache.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
