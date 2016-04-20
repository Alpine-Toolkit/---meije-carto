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
#include "map/wmts_plugin.h"

/***************************************************************************************************/

class TestQcWmtsRequestManager : public QObject
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

void TestQcWmtsRequestManager::constructor()
{
  QcWmtsPlugin wmts_plugin("foo", 20, 256);
  QcWmtsManager * wmts_manager = wmts_plugin.wmts_manager();
  FakeWmtsTileFetcher tile_fetcher;
  wmts_manager->set_tile_fetcher(&tile_fetcher);
  wmts_manager->tile_cache()->clear_all();

  QcMapView map_view(&wmts_plugin);
  QcWmtsRequestManager * request_manager = map_view.request_manager();

  QSet<QcTileSpec> tile_specs;

  QcTileSpec tile_spec1("geoportail", 1, 16, 1, 1);
  tile_specs = {tile_spec1};
  request_manager->request_tiles(tile_specs);
  wmts_manager->dump();
  QTest::qWait(1);

  QcTileSpec tile_spec2("geoportail", 1, 16, 2, 2);
  tile_specs = {tile_spec1, tile_spec2};
  request_manager->request_tiles(tile_specs);
  wmts_manager->dump();
  QTest::qWait(1);

  QcTileSpec tile_spec3("geoportail", 1, 16, 3, 3);
  tile_specs = {tile_spec2, tile_spec3};
  request_manager->request_tiles(tile_specs);
  wmts_manager->dump();
  QTest::qWait(1);
}

/***************************************************************************************************/

QTEST_MAIN(TestQcWmtsRequestManager)
#include "test_wmts_request_manager.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
