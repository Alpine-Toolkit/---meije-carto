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

#include <iostream>

#include <QtTest/QtTest>

/**************************************************************************************************/

#include "map/earth.h"
#include "map/tile_matrix_set.h"
#include "map/viewport.h"
// #include "map/mosaic_painter.h"

/***************************************************************************************************/

class TestQcViewport: public QObject
{
  Q_OBJECT

private slots:
  void constructor();
};

void TestQcViewport::constructor()
{
  size_t number_of_levels = 20;
  size_t tile_size = 256;
  size_t level = 16;
  QcTileMatrixSet tile_matrix_set("wtms", number_of_levels, tile_size);
  double resolution = tile_matrix_set[level].resolution();
  double tile_length_m = tile_matrix_set[level].tile_length_m();
  std::cout << "Resolution " << resolution << std::endl;
  std::cout << "tile length m " << tile_length_m << std::endl;

  QcGeoCoordinatePseudoWebMercator coordinate_origin(0, 0); // Fixme: need to pass fake state
  QcTiledZoomLevel tiled_zoom_level(EQUATORIAL_PERIMETER, tile_size, 0); // map can have different tile size !
  QcViewportState viewport_state(coordinate_origin, tiled_zoom_level, 0);
  QSize viewport_size(1000, 1000); // widget size
  QcViewport viewport(viewport_state, viewport_size);
  // QcMosaicPainter mosaic_painter(viewport); // unused

  viewport.zoom_at(QcGeoCoordinateWGS84(0, 0).web_mercator(), level); // zoom_factor == tiled_matrix resolution
  QcGeoCoordinateWGS84 coordinate = viewport.wgs84();
  std::cout << "wgs84 " << coordinate.longitude() << " " << coordinate.latitude() << std::endl;
  QcGeoCoordinateWebMercator web_mercator_coordinate = viewport.web_mercator();
  std::cout << "mercator " << web_mercator_coordinate.x() << " " << web_mercator_coordinate.y() << std::endl;
  // QcGeoCoordinateNormalisedWebMercator normalised_web_mercator_coordinate = viewport.normalised_web_mercator();
  // std::cout << "normalised mercator " << normalised_web_mercator_coordinate.x() << " " << normalised_web_mercator_coordinate.y() << std::endl;

  const QcPolygon & polygon1 = viewport.polygon();
  // const QcVectorDouble & vertex = polygon.vertexes()[0];
  // std::cout << "(" << vertex.x() << ", " << vertex.y() << ")" << std::endl;
  const QcInterval2DDouble & interval = polygon1.interval();
  std::cout << "polygon interval"
	    << "[" << interval.x().inf() << ", " << interval.x().sup() << "]"
	    << "x"
	    << "[" << interval.y().inf() << ", " << interval.y().sup() << "]"
	    << std::endl;
  QcTiledPolygon tiled_polygon1 = polygon1.intersec_with_grid(tile_length_m);
  for (const QcTiledPolygonRun & run:  tiled_polygon1.runs()) {
    const QcIntervalInt & run_interval = run.interval();
    std::cout << "Run " << run.y() << " [" << run_interval.inf() << ", " << run_interval.sup() << "]" << std::endl;
  }

  viewport.pan(700, 700);
  web_mercator_coordinate = viewport.web_mercator();
  std::cout << "mercator " << web_mercator_coordinate.x() << " " << web_mercator_coordinate.y() << std::endl;
  const QcPolygon & polygon2 = viewport.polygon();
  QcTiledPolygon tiled_polygon2 = polygon2.intersec_with_grid(tile_length_m);
  for (const QcTiledPolygonRun & run:  tiled_polygon2.runs()) {
    const QcIntervalInt & run_interval = run.interval();
    std::cout << "Run " << run.y() << " [" << run_interval.inf() << ", " << run_interval.sup() << "]" << std::endl;
  }

  QcTiledPolygonDiff tile_polygon_diff = tiled_polygon2.diff(tiled_polygon1);
  for (const QcTiledPolygonRun & run:  tile_polygon_diff.new_area()) {
    const QcIntervalInt & run_interval = run.interval();
    std::cout << "New Run " << run.y() << " [" << run_interval.inf() << ", " << run_interval.sup() << "]" << std::endl;
    for (const QcTileMatrixIndex & tile_index: run.tile_indexes())
      qInfo() << tile_index;
  }
  for (const QcTiledPolygonRun & run:  tile_polygon_diff.same_area()) {
    const QcIntervalInt & run_interval = run.interval();
    std::cout << "Same Run " << run.y() << " [" << run_interval.inf() << ", " << run_interval.sup() << "]" << std::endl;
  }
  for (const QcTiledPolygonRun & run:  tile_polygon_diff.old_area()) {
    const QcIntervalInt & run_interval = run.interval();
    std::cout << "Old Run " << run.y() << " [" << run_interval.inf() << ", " << run_interval.sup() << "]" << std::endl;
  }
}

/***************************************************************************************************/

QTEST_MAIN(TestQcViewport)
#include "test_viewport.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
