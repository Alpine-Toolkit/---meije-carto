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

#include <QCoreApplication>
#include <QThread>
#include <QTimer>
#include <QtDebug>

/**************************************************************************************************/

#include "cache/offline_cache.h"
#include "coordinate/mercator.h"
#include "coordinate/wgs84.h"
#include "geometry/polygon.h"
#include "geometry/vector.h"
#include "tools/logger.h"
#include "wmts/wmts_plugin_manager.h"

// #include "map/tile_loader.h"

/***************************************************************************************************/

class TileFetcherHelper : public QObject
{
  Q_OBJECT;

 public:
  TileFetcherHelper(QcOfflineTileCache * offline_cache)
    : m_offline_cache(offline_cache)
  {}

 public:
  int number_of_received_tiles() const {
    return m_finished_tile.size();
  }
  bool has_received(const QcTileSpec & tile_spec) const {
    return m_finished_tile.contains(tile_spec);
  }

 public slots:
  void tile_finished(const QcTileSpec & tile_spec, const QByteArray & bytes, const QString & format) {
    qInfo() << "Tile finished" << tile_spec;
    m_finished_tile.push_back(tile_spec);
    m_offline_cache->insert(tile_spec, bytes, format);
  }

 private:
  QcOfflineTileCache * m_offline_cache;
  QList<QcTileSpec> m_finished_tile;
};

/**************************************************************************************************/

class Application : public QCoreApplication
{
  Q_OBJECT;

public:
  Application(int & argc, char ** argv);

public slots:
  void main_task();
};

Application::Application(int & argc, char ** argv)
  : QCoreApplication(argc, argv)
{
  QTimer::singleShot(0, this, SLOT(main_task()));
}

void
Application::main_task()
{
  QcWmtsPluginManager & plugin_manager = QcWmtsPluginManager::instance();

  QcWmtsPlugin * geoportail_plugin = plugin_manager["geoportail"];

  const QcTileMatrixSet & tile_matrix_set = geoportail_plugin->tile_matrix_set();

  // QcTileLoader tile_loader(geoportail_plugin);

  // Annecy
  // double longitude = 6.311331853277862;
  // double latitude = 45.956298260767284;

  // Bezons
  // double longitude = 2.206142;
  // double latitude = 48.924482;

  // Bérarde
  // double longitude = 6.2914;
  // double latitude = 44.9328;

  // Vallouise
  // double longitude = 6.4895;
  // double latitude = 44.8461;

  // Centre
  // double longitude = 3;
  // double latitude = 46.5;

  // Freissinieres
  // double longitude = 6.5383;
  // double latitude = 44.7523;

  // Mont-dore
  double longitude = 2.816;
  double latitude = 45.54;

  QcWgsCoordinate center_wsg84(longitude, latitude);
  QcPseudoWebMercatorCoordinate center_mercator = center_wsg84.pseudo_web_mercator();
  QcVectorDouble center = center_mercator.vector();;
  double radius_m = 20 * 1000;
  QcVectorDouble half_diagonal_m(radius_m, radius_m);

  QcVectorDouble point1 = center + half_diagonal_m;
  QcVectorDouble point2 = center + half_diagonal_m.mirror_x();
  QcVectorDouble point3 = center + half_diagonal_m.rotate_180();
  QcVectorDouble point4 = center + half_diagonal_m.mirror_y();
  QcPolygon polygon;
  polygon.add_vertex(point3);
  polygon.add_vertex(point2);
  polygon.add_vertex(point1);
  polygon.add_vertex(point4);

  const QcInterval2DDouble & interval = polygon.interval();
  qInfo() << "polygon interval"
          << "[" << interval.x().inf() << ", " << interval.x().sup() << "]"
          << "x"
          << "[" << interval.y().inf() << ", " << interval.y().sup() << "]";

  QcWmtsNetworkTileFetcher * tile_fetcher = geoportail_plugin->tile_fetcher(); // subclass QcGeoportailPlugin ???

  QcOfflineTileCache * offline_cache = geoportail_plugin->wmts_manager()->tile_cache()->offline_cache();
  TileFetcherHelper tile_fetcher_helper(offline_cache);
  QObject::connect(tile_fetcher, &QcWmtsNetworkTileFetcher::tile_finished,
                   &tile_fetcher_helper, &TileFetcherHelper::tile_finished);

  QString layer_title = "Carte topographique";
  // QString layer_title = "Carte"; // error
  // QString layer_title = "Vue aérienne";
  int map_id = geoportail_plugin->layer(layer_title)->map_id();

  QSet<QcTileSpec> tiles_added;
  QSet<QcTileSpec> tiles_removed;

  // Fixme: level <= 10 : any tiles
  for (int level = 0; level <= 16; level++) {
    double tile_length_m = tile_matrix_set[level].tile_length_m();
    QcTiledPolygon tiled_polygon = polygon.intersec_with_grid(tile_length_m);
    for (const QcTiledPolygonRun & run:  tiled_polygon.runs()) {
      const QcIntervalInt & run_interval = run.interval();
      qInfo() << "Run " << run.y() << " [" << run_interval.inf() << ", " << run_interval.sup() << "]";
      for (int x = run_interval.inf(); x <= run_interval.sup(); x++) {
        QcTileSpec tile_spec = geoportail_plugin->create_tile_spec(map_id, level, x, run.y());
        if (offline_cache->contains(tile_spec)) {
          qInfo() << "Ok" << tile_spec;
        }
        else {
          qInfo() << "Missing" << tile_spec;
          tiles_added << tile_spec;
        }
      }
    }
  }
  int number_of_tiles = tiles_added.size();

  // Add tiles to disk
  //  tile_fetcher::tile_finished -> QcWmtsManager::fetcher_tile_finished -> tile_cache()->insert
  //
  // Fixme:
  //   check if on disk
  //   just save
  //   don't count in online cache

  tile_fetcher->update_tile_requests(tiles_added, tiles_removed);
  while (tile_fetcher_helper.number_of_received_tiles() != number_of_tiles) {
    processEvents();
    // QThread::msleep(10);
  }

  exit(0);
}

/**************************************************************************************************/

int
main(int argc, char *argv[])
{
  qInstallMessageHandler(message_handler);
  Application application(argc, argv);
  return application.exec();
}

/***************************************************************************************************/

#include "tile-loader.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
