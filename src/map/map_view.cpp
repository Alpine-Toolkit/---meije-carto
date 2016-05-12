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

#include "map_view.h"

#include "wmts/tile_spec.h"

#include <QtDebug>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

QcMapViewLayer::QcMapViewLayer(const QcWmtsPluginLayer * plugin_layer, QcViewport * viewport, QcMapLayerScene * layer_scene)
  : QObject(),
    m_plugin_layer(plugin_layer),
    m_viewport(viewport),
    m_layer_scene(layer_scene),
    m_request_manager(new QcWmtsRequestManager(this, plugin()->wmts_manager()))
{}

QcMapViewLayer::~QcMapViewLayer()
{
  if (m_request_manager)
    delete m_request_manager;
}

float
QcMapViewLayer::opacity() const
{
  return m_layer_scene->opacity();
}

void
QcMapViewLayer::set_opacity(float opacity)
{
  m_layer_scene->set_opacity(opacity);
}

/*! Slot to add a tile to the layer scene
 */
void
QcMapViewLayer::update_tile(const QcTileSpec & tile_spec)
{
  qInfo() << tile_spec;
  if (m_visible_tiles.contains(tile_spec)) {
    QSharedPointer<QcTileTexture> texture = m_request_manager->tile_texture(tile_spec);
    if (!texture.isNull()) {
      m_layer_scene->add_tile(tile_spec, texture);
      emit scene_graph_changed();
    }
  }
}

QcTileSpecSet
QcMapViewLayer::intersec_polygon_with_grid(const QcPolygon & polygon, double tile_length_m, int zoom_level)
{
  QcTileSpecSet visible_tiles;
  QcTiledPolygon tiled_polygon = polygon.intersec_with_grid(tile_length_m);
  for (const QcTiledPolygonRun & run: tiled_polygon.runs()) {
    const QcIntervalInt & run_interval = run.interval();
    size_t y = run.y();
    qInfo() << "Run " << run.y() << " [" << run_interval.inf() << ", " << run_interval.sup() << "]";
    for (int x = run_interval.inf(); x <= run_interval.sup(); x++)
      visible_tiles.insert(m_plugin_layer->create_tile_spec(zoom_level, x, y));
  }
  return visible_tiles;
}

void
QcMapViewLayer::update_scene()
{
  qInfo();

  // Fixme: if layers share the same tile matrix ?

  // Compute visible tile set in viewport

  // Fixme: Done in map scene !!!
  QcTileMatrixSet tile_matrix_set = plugin()->tile_matrix_set();
  int zoom_level = m_viewport->zoom_level();
  const QcTileMatrix & tile_matrix = tile_matrix_set[zoom_level];
  double tile_length_m = tile_matrix.tile_length_m();

  // Fixme: better check ?
  // Fixme: clear first ?
  const QcPolygon & polygon = m_viewport->middle_polygon();
  const QcInterval2DDouble & interval = polygon.interval();
  if (interval.is_empty()) {
    m_west_visible_tiles.clear();
    m_middle_visible_tiles.clear();
    m_east_visible_tiles.clear();
    m_visible_tiles.clear();
    m_layer_scene->set_visible_tiles(m_visible_tiles, m_east_visible_tiles, m_middle_visible_tiles, m_west_visible_tiles);
  } else {
    // qInfo() << "Normalised Mercator polygon interval [m]"
    //         << "[" << (int) interval.x().inf() << ", " << (int) interval.x().sup() << "]"
    //         << "x"
    //         << "[" << (int) interval.y().inf() << ", " << (int) interval.y().sup() << "]";
    m_middle_visible_tiles = intersec_polygon_with_grid(polygon, tile_length_m, zoom_level);
    if (m_viewport->cross_west_line())
      m_west_visible_tiles = intersec_polygon_with_grid(m_viewport->west_polygon(), tile_length_m, zoom_level);
    else
      m_west_visible_tiles.clear();
    if (m_viewport->cross_east_line())
      m_east_visible_tiles = intersec_polygon_with_grid(m_viewport->east_polygon(), tile_length_m, zoom_level);
    else
      m_east_visible_tiles.clear();
    QcTileSpecSet visible_tiles = m_east_visible_tiles + m_middle_visible_tiles + m_west_visible_tiles;
    qInfo() << "visible west tiles: " << m_west_visible_tiles << '\n'
            << "visible middle tiles: " << m_middle_visible_tiles << '\n'
            << "visible east tiles: " << m_east_visible_tiles << '\n'
            << "visible tiles: " << visible_tiles << '\n'
            << "new visible tiles: " << visible_tiles - m_visible_tiles;

    // bool new_tiles_introduced = !m_visible_tiles.contains(visible_tiles);
    m_visible_tiles = visible_tiles;

    // Skip tiles
    // emit scene_graph_changed();
    // return;

    // Fixme: better ?
    m_layer_scene->set_visible_tiles(m_visible_tiles, m_west_visible_tiles, m_middle_visible_tiles, m_east_visible_tiles);

    // Don't request tiles that are already built and textured
    QcTileSpecSet tile_to_request = m_visible_tiles - m_layer_scene->textured_tiles();
    if (!tile_to_request.isEmpty()) {
        QList<QSharedPointer<QcTileTexture> > cached_tiles = m_request_manager->request_tiles(tile_to_request);
        for (const auto & texture : cached_tiles)
          m_layer_scene->add_tile(texture->tile_spec, texture);
        if (!cached_tiles.isEmpty())
          emit scene_graph_changed();
    }
  }

  // Fixme: viewport_changed -> scene_graph_changed
  emit scene_graph_changed();
}

/**************************************************************************************************/

QcMapView::QcMapView()
  : QObject(),
    m_viewport(nullptr), // initialised in ctor
    m_map_scene(nullptr) // initialised in ctor
{
  // Fixme: need to pass fake state
  QcGeoCoordinatePseudoWebMercator coordinate_origin(0, 0);
  int tile_size = 256; // map can have different tile size !
  QcTiledZoomLevel tiled_zoom_level(EQUATORIAL_PERIMETER, tile_size, 0);
  QcViewportState viewport_state(coordinate_origin, tiled_zoom_level, 0);
  QSize viewport_size(0, 0);
  m_viewport = new QcViewport(viewport_state, viewport_size);

  m_map_scene = new QcMapScene(m_viewport); // parent

  connect(m_viewport, SIGNAL(viewport_changed()),
	  this, SLOT(update_scene()),
	  Qt::QueuedConnection);
}

QcMapView::~QcMapView()
{
  for (auto * layer : m_layers)
    layer->deleteLater(); // Fixme: delete ?
}

QcMapViewLayer *
QcMapView::get_layer(const QcWmtsPluginLayer * plugin_layer)
{
  QString name = plugin_layer->hash_name();
  if (m_layer_map.contains(name))
    return m_layer_map[name];
  else
    return nullptr;
}

void
QcMapView::add_layer(const QcWmtsPluginLayer * plugin_layer)
{
  QString name = plugin_layer->hash_name();
  if (!m_layer_map.contains(name)) {
    QcMapLayerScene * layer_scene = m_map_scene->add_layer(plugin_layer);
    QcMapViewLayer * layer = new QcMapViewLayer(plugin_layer, m_viewport, layer_scene);
    // Fixme:
    // connect(layer, SIGNAL(scene_graph_changed()),
    //         this, SLOT(scene_graph_changed()),
    //         Qt::QueuedConnection);
    m_layers << layer;
    m_layer_map.insert(name, layer);
  }
}

void
QcMapView::remove_layer(const QcWmtsPluginLayer * plugin_layer)
{
  QcMapViewLayer * layer = get_layer(plugin_layer);
  if (layer) {
    m_layers.removeOne(layer);
    layer->deleteLater();
    // Fixme: deconnect ?
    m_map_scene->remove_layer(plugin_layer);
  }
}

QList<const QcWmtsPluginLayer *>
QcMapView::layers() const
{
  QList<const QcWmtsPluginLayer *> plugin_layers;
  for (auto * layer : m_layers)
    plugin_layers << layer->plugin_layer();
  return plugin_layers;
}

float
QcMapView::opacity(const QcWmtsPluginLayer * plugin_layer)
{
  QcMapViewLayer * layer = get_layer(plugin_layer);
  if (layer)
    return layer->opacity();
  else
    return .0;
}

void
QcMapView::set_opacity(const QcWmtsPluginLayer * plugin_layer, float opacity)
{
  QcMapViewLayer * layer = get_layer(plugin_layer);
  if (layer)
    layer->set_opacity(opacity);
}

void
QcMapView::update_scene()
{
  qInfo();
  for (auto * layer : m_layers)
    layer->update_scene();

  // Fixme: else black at startup
  emit scene_graph_changed();
}

/**************************************************************************************************/

// #include "map_view.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
