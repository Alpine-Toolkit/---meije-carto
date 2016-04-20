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

#include "map/map_view.h"
#include "map/tile_spec.h"

#include <QtDebug>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

QcMapView::QcMapView(QcWmtsPlugin * wmts_plugin)
  : QObject(),
    m_plugin(wmts_plugin),
    m_wmts_manager(wmts_plugin->wmts_manager()),
    m_request_manager(nullptr), // initialised in ctor
    m_viewport(nullptr), // initialised in ctor
    m_map_scene(nullptr) // initialised in ctor
{
  m_request_manager = new QcWmtsRequestManager(this, m_wmts_manager);

  // Fixme: need to pass fake state
  QcGeoCoordinateNormalisedWebMercator coordinate_origin(0, 0);
  QcTiledZoomLevel tiled_zoom_level(EQUATORIAL_PERIMETER, m_plugin->tile_matrix_set().tile_size(), 0); // map can have different tile size !
  QcViewportState viewport_state(coordinate_origin, tiled_zoom_level, 0);
  QSize viewport_size(0, 0);
  m_viewport = new QcViewport(viewport_state, viewport_size);

  m_map_scene = new QcMapScene(m_viewport, m_plugin->tile_matrix_set()); // parent

  connect(m_viewport, SIGNAL(viewport_changed()),
	  this, SLOT(update_scene()),
	  Qt::QueuedConnection);
}

QcMapView::~QcMapView()
{
  if (m_request_manager)
    delete m_request_manager;
}

void
QcMapView::update_tile(const QcTileSpec & tile_spec)
{
  QSharedPointer<QcTileTexture> texture = m_request_manager->tile_texture(tile_spec);
  if (!texture.isNull()) {
    qInfo() << "QcMapView::update_tile" << tile_spec;
  }
}

void
QcMapView::update_scene()
{
  qInfo() << "QcMapView::update_scene";

  // Done in map scene !!!
  QcTileMatrixSet tile_matrix_set = m_plugin->tile_matrix_set();
  int zoom_level = m_viewport->zoom_level();
  const QcTileMatrix & tile_matrix = tile_matrix_set[zoom_level];
  double tile_length_m = tile_matrix.tile_length_m();

  const QcPolygon & polygon = m_viewport->polygon();
  const QcInterval2DDouble & interval = polygon.interval();
  qInfo() << "Normalised Mercator polygon interval [m]"
          << "[" << (int) interval.x().inf() << ", " << (int) interval.x().sup() << "]"
          << "x"
          << "[" << (int) interval.y().inf() << ", " << (int) interval.y().sup() << "]";
  QcTiledPolygon tiled_polygon = polygon.intersec_with_grid(tile_length_m);
  QcTileSpecSet tile_specs;
  for (const QcTiledPolygonRun & run:  tiled_polygon.runs()) {
    const QcIntervalInt & run_interval = run.interval();
    size_t y = run.y();
    qInfo() << "Run " << run.y() << " [" << run_interval.inf() << ", " << run_interval.sup() << "]";
    for (size_t x = run_interval.inf(); x <= run_interval.sup(); x++)
      tile_specs.insert(m_plugin->create_tile_spec(0, zoom_level, x, y));
  }
  m_request_manager->request_tiles(tile_specs);
}

/**************************************************************************************************/

// #include "map_view.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
