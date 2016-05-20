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

#include <QtTest>
#include <QtDebug>

/**************************************************************************************************/

#include "map/map_view.h"
#include "wmts/wmts_manager.h"
#include "wmts/wmts_plugin.h"
#include "wmts/wmts_reply.h"
#include "wmts/wmts_tile_fetcher.h"

/***************************************************************************************************/

class TestQcWmtsManager : public QObject
{
  Q_OBJECT;

 private slots:
  void constructor();
};

class FakeWmtsReply : public QcWmtsReply
{
  Q_OBJECT

 private:
  static QByteArray m_bytes;

 public:
  FakeWmtsReply(const QcTileSpec & tile_spec)
    : QcWmtsReply(tile_spec)
  {
    if (!m_bytes.size()) {
      QFile file("../wmts.jpg"); // Fixme
      file.open(QIODevice::ReadOnly);
      m_bytes = file.readAll();
    }

    set_map_image_data(m_bytes);
    set_map_image_format("jpg");
    set_finished(true);
  }
};

QByteArray FakeWmtsReply::m_bytes;

class FakeWmtsTileFetcher : public QcWmtsTileFetcher
{
  Q_OBJECT

 public:
  FakeWmtsTileFetcher()
    : QcWmtsTileFetcher()
  {}

 private:
  QcWmtsReply * get_tile_image(const QcTileSpec & tile_spec) {
    qInfo() << "FakeWmtsTileFetcher::get_tile_image" << tile_spec;
    return new FakeWmtsReply(tile_spec);
  }
};

void TestQcWmtsManager::constructor()
{
  QcWmtsPlugin wmts_plugin("foo", 20, 256);
  QcWmtsManager * wmts_manager = wmts_plugin.wmts_manager();
  FakeWmtsTileFetcher tile_fetcher;
  wmts_manager->set_tile_fetcher(&tile_fetcher);
  wmts_manager->tile_cache()->clear_all();

  QcMapView map_view1(&wmts_plugin);
  map_view1.setObjectName("map1");

  QcMapView map_view2(&wmts_plugin);
  map_view2.setObjectName("map2");

  QSet<QcTileSpec> tiles_added;
  QcTileSpecSet tiles_removed ;

  QcTileSpec tile_spec1("geoportail", 1, 16, 1, 1);
  tiles_added = {tile_spec1};
  tiles_removed = {} ;
  wmts_manager->update_tile_requests(&map_view1, tiles_added, tiles_removed);
  wmts_manager->dump();
  QTest::qWait(1);

  QcTileSpec tile_spec2("geoportail", 1, 16, 2, 2);
  tiles_added = {tile_spec2};
  tiles_removed = {} ;
  wmts_manager->update_tile_requests(&map_view2, tiles_added, tiles_removed);
  wmts_manager->dump();
  QTest::qWait(1);

  tiles_added = {tile_spec2};
  tiles_removed = {tile_spec1} ;
  wmts_manager->update_tile_requests(&map_view1, tiles_added, tiles_removed);
  wmts_manager->dump();
  QTest::qWait(1);

  QcTileSpec tile_spec3("geoportail", 1, 16, 3, 3);
  tiles_added = {tile_spec3};
  tiles_removed = {} ;
  wmts_manager->update_tile_requests(&map_view2, tiles_added, tiles_removed);
  wmts_manager->dump();
  QTest::qWait(1);

  qInfo() << "Release map2";
  wmts_manager->release_map(&map_view2);
  wmts_manager->dump();
}

/***************************************************************************************************/

QTEST_MAIN(TestQcWmtsManager)
#include "test_wmts_manager.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
