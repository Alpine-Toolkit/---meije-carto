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

#include <QtTest/QtTest>
#include <QtDebug>

/**************************************************************************************************/

#include "cache/file_tile_cache.h"

/***************************************************************************************************/

class TestQcFileTileCache: public QObject
{
  Q_OBJECT

private slots:
  void constructor();
};

void TestQcFileTileCache::constructor()
{
  QcFileTileCache * file_tile_cache = new QcFileTileCache();
  file_tile_cache->clear_all();

  QcTileSpec tile_spec("geoportail", 1, 16, 33885, 23658);

  QFile file("../wmts.jpg"); // Fixme
  file.open(QIODevice::ReadOnly);
  QByteArray bytes = file.readAll();
  file_tile_cache->insert(tile_spec, bytes, QStringLiteral("jpg"));

  delete file_tile_cache;
  file_tile_cache = new QcFileTileCache();
  QSharedPointer<QcTileTexture> tile_texture = file_tile_cache->get(tile_spec);
  QVERIFY(tile_texture->tile_spec == tile_spec);
}

/***************************************************************************************************/

QTEST_MAIN(TestQcFileTileCache)
#include "test_file_tile_cache.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
