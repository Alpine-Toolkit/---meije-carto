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

#include <QString>
#include <QtDebug>
#include <QtTest>

/**************************************************************************************************/

#include "wmts/providers/geoportail/geoportail_plugin.h"

/***************************************************************************************************/

class TestQcGeoportailWmtsTileFetcher : public QObject
{
  Q_OBJECT;

 private slots:
  void constructor();
};

class TileFetcherHelper : public QObject
{
  Q_OBJECT;

 public:
  bool has_received(const QcTileSpec & tile_spec) {
    return m_finished_tile.contains(tile_spec);
  }

 public slots:
  void tile_finished(const QcTileSpec & tile_spec, const QByteArray & bytes, const QString & format) {
    qInfo() << "Tile finished" << tile_spec;
    m_finished_tile.push_back(tile_spec);;
  }

 private:
  QList<QcTileSpec> m_finished_tile;
};

void TestQcGeoportailWmtsTileFetcher::constructor()
{
  QString json_path("geoportail-license.json");
  QcGeoportailWmtsLicense geoportail_license(json_path);
  QcGeoportailPlugin geoportail_plugin(geoportail_license);
  QcGeoportailWmtsTileFetcher * tile_fetcher = geoportail_plugin.tile_fetcher(); // subclass QcGeoportailPlugin ???

  TileFetcherHelper tile_fetcher_helper;
  // connect(&tile_fetcher, SIGNAL(tile_finished()),
  // 	     &tile_fetcher_helper, SLOT(tile_finished()));
  connect(tile_fetcher, &QcGeoportailWmtsTileFetcher::tile_finished,
  	  &tile_fetcher_helper, &TileFetcherHelper::tile_finished);

  QString layer_title = "Carte topographique";
  // QString layer_title = "Carte"; // error
  // QString layer_title = "Vue aérienne";
  int map_id = geoportail_plugin.layer(layer_title).map_id();
  QcTileSpec tile_spec = geoportail_plugin.create_tile_spec(map_id, 16, 33885, 23658);
  qInfo() << tile_spec;
  QSet<QcTileSpec> tiles_added = {tile_spec};
  QSet<QcTileSpec> tiles_removed;
  tile_fetcher->update_tile_requests(tiles_added, tiles_removed);
  while (!tile_fetcher_helper.has_received(tile_spec))
    QTest::qWait(200);
}

/***************************************************************************************************/

QTEST_MAIN(TestQcGeoportailWmtsTileFetcher)
#include "test_geoportail_wmts_tile_fetcher.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
