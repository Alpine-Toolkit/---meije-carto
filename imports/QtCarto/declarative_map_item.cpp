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

#include <cmath>

#include <QMatrix4x4>
#include <QSGClipNode>
#include <QSGGeometry>
#include <QSGTransformNode>
#include <QtDebug>
#include <QtQuick/qsgflatcolormaterial.h>
#include <QtQuick/qsgnode.h>

/**************************************************************************************************/

#include "declarative_map_item.h"
#include "map/earth.h"
#include "map/geo_coordinate.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

MapItem::MapItem(QQuickItem *parent)
  : QQuickItem(parent),
    m_plugin(nullptr),
    m_viewport(nullptr)
{
  setFlag(ItemHasContents, true);

  QString json_path("geoportail-license.json");
  QcGeoPortailWmtsLicence geoportail_license(json_path);
  m_plugin = new QcGeoportailPlugin(geoportail_license);

  // Fixme: need to pass fake state
  QcGeoCoordinateNormalisedMercator coordinate_origin(0, 0);
  QcTiledZoomLevel tiled_zoom_level(EQUATORIAL_PERIMETER, m_plugin->tile_matrix_set().tile_size(), 0); // map can have different tile size !
  QcViewportState viewport_state(coordinate_origin, tiled_zoom_level, 0);
  QSize viewport_size(0, 0);
  m_viewport = new QcViewport(viewport_state, viewport_size);

  // Fixme: remove
  // Set default viewport center and zoom level
  size_t level = 16;
  double longitude =  2.3491; // Paris Notre-Dame
  double latitude  = 48.8533;
  QcGeoCoordinateWGS84 coordinate(longitude, latitude);
  m_viewport->zoom_at(coordinate.mercator(), level);
}

MapItem::~MapItem()
{
  // delete m_plugin, m_viewport
}

void
MapItem::set_zoom_level(int zoom_level)
{
  qInfo() << "MapItem::set_zoom_level " << zoom_level;

  if (zoom_level == m_viewport->zoom_level())
    return;

  // Fixme: check range
  if (zoom_level < 0 || zoom_level > 18)
    return;

  m_viewport->set_zoom_level(zoom_level);
  update(); // Fixme: signal
  emit zoom_levelChanged(zoom_level);
}

int
MapItem::zoom_level() const
{
  return m_viewport->zoom_level();
}

void
MapItem::set_center(const QGeoCoordinate & center)
{
  qInfo() << "MapItem::set_center WGS84 " << center;

  QcGeoCoordinateWGS84 coordinate(center.longitude(), center.latitude());
  if (coordinate == m_viewport->wgs84())
    return;

  if (!center.isValid())
    return;
  // coordinate.setLatitude(qBound(-m_maximumViewportLatitude, center.latitude(), m_maximumViewportLatitude));

  m_viewport->set_coordinate(coordinate);
  update(); // Fixme: signal
  emit centerChanged(center);

  // QcGeoCoordinateMercator mercator_coordinate = m_viewport->mercator();
  // qInfo() << "mercator " << mercator_coordinate.x() << mercator_coordinate.y();
  // QcGeoCoordinateNormalisedMercator normalised_mercator_coordinate = m_viewport->normalised_mercator();
  // qInfo() << "normalised mercator " << normalised_mercator_coordinate.x() << normalised_mercator_coordinate.y();
}

QGeoCoordinate
MapItem::center() const
{
  QcGeoCoordinateWGS84 center = m_viewport->wgs84();
  return QGeoCoordinate(center.latitude(), center.longitude());
}

void
MapItem::pan(int dx, int dy)
{
  qInfo() << "MapItem::pan" << dx << dy;
  m_viewport->pan(dx, dy); // Fixme: unit is m
  update(); // Fixme: signal
}

void
MapItem::geometryChanged(const QRectF & new_geometry, const QRectF & old_geometry)
{
  qInfo() << "MapItem::geometryChanged" << old_geometry << "->" << new_geometry;
  QQuickItem::geometryChanged(new_geometry, old_geometry);
  QSize viewport_size(new_geometry.width(), new_geometry.height()); // Fixme: QSizeF size()
  m_viewport->set_viewport_size(viewport_size);
}

class QcMapContainerNode : public QSGTransformNode
{
};

class QcMapRootNode : public QSGClipNode
{
public:
  QcMapRootNode()
    : geometry(QSGGeometry::defaultAttributes_Point2D(), 4),
      root(new QSGTransformNode()),
      tile_node(new QcMapContainerNode())
  {
    qInfo() << "Create QcMapContainerNode";
    setIsRectangular(true);
    setGeometry(&geometry);
    setGeometry(&geometry);
    root->appendChildNode(tile_node);
    appendChildNode(root);
  }

  void set_clip_rect(const QRect & rect)
  {
    if (rect != m_clip_rect) {
      QSGGeometry::updateRectGeometry(&geometry, rect);
      QSGClipNode::setClipRect(rect);
      m_clip_rect = rect;
      markDirty(DirtyGeometry);
    }
  }

public:
  QSGGeometry geometry;
  QRect m_clip_rect;
  QSGTransformNode * root;
  QcMapContainerNode * tile_node;
};

QSGNode *
MapItem::updatePaintNode(QSGNode * old_node, UpdatePaintNodeData *)
{
  qInfo() << "MapItem::updatePaintNode" << old_node;

  QRectF bounds = boundingRect(); // m_ ???
  qInfo() << "rect" << bounds;
  float width = bounds.width();
  float height = bounds.height();
  if (width <= 0 || height <= 0) {
    delete old_node;
    return nullptr;
  }

  QcMapRootNode * map_root_node = static_cast<QcMapRootNode *>(old_node);
  if (!map_root_node) {
    qInfo() << "map_root_node is null";
    map_root_node = new QcMapRootNode();
  }

  QRect rect = QRect(bounds.x(), bounds.y(), width, height);
  map_root_node->set_clip_rect(rect);

  QMatrix4x4 item_space_matrix;
  item_space_matrix.scale(width/2, height/2);
  item_space_matrix.translate(1, 1);
  item_space_matrix.scale(1, -1);
  map_root_node->root->setMatrix(item_space_matrix);
  qInfo() << "item_space_matrix" << item_space_matrix;

  QcTileMatrixSet & tile_matrix_set = m_plugin->tile_matrix_set();
  int tile_size = tile_matrix_set.tile_size();
  const QcTileMatrix & tile_matrix = tile_matrix_set[m_viewport->zoom_level()];
  double tile_length_m = tile_matrix.tile_length_m();
  double resolution = tile_matrix.resolution(); // [m/px]
  qInfo() << "tile_matrix" << m_viewport->zoom_level() << tile_length_m << resolution;

  const QcPolygon & polygon = m_viewport->polygon();
  const QcInterval2DDouble & interval = polygon.interval();
  qInfo() << "Normalised Mercator polygon interval [m]"
          << "[" << (int) interval.x().inf() << ", " << (int) interval.x().sup() << "]"
          << "x"
          << "[" << (int) interval.y().inf() << ", " << (int) interval.y().sup() << "]";
  QcTiledPolygon tiled_polygon = polygon.intersec_with_grid(tile_length_m);
  for (const QcTiledPolygonRun & run:  tiled_polygon.runs()) {
    const QcIntervalInt & run_interval = run.interval();
    qInfo() << "Run " << run.y() << " [" << run_interval.inf() << ", " << run_interval.sup() << "]";
  }

  QcGeoCoordinateMercator mercator_coordinate = m_viewport->mercator();
  QcGeoCoordinateNormalisedMercator normalised_mercator_coordinate = m_viewport->normalised_mercator();
  QcVectorDouble center = normalised_mercator_coordinate.vector() * EQUATORIAL_PERIMETER;
  qInfo() << "center mercator" << mercator_coordinate << normalised_mercator_coordinate << center;
  double x_inf_m = interval.x().inf();
  double y_inf_m = interval.y().inf();
  double x_center_px = (center.x() - x_inf_m) / resolution; // vector
  double y_center_px = (center.y() - y_inf_m) / resolution;
  qInfo() << "xy center px" << x_center_px << y_center_px; // == width/2 height/2
  // qInfo() << (int)center.x() << (int)x_inf_m << resolution;

  // Use QcInterval2D ???
  int x_grid_inf = ceil(interval.x().inf() / tile_length_m); // duplicate
  int x_grid_sup = ceil(interval.x().sup() / tile_length_m);
  int y_grid_inf = ceil(interval.y().inf() / tile_length_m);
  int y_grid_sup = ceil(interval.y().sup() / tile_length_m);
  size_t number_of_vertical_lines = x_grid_sup - x_grid_inf +1;
  size_t number_of_horizontal_lines = y_grid_sup - y_grid_inf +1;
  qInfo() << "x grid" << x_grid_inf << x_grid_sup << number_of_vertical_lines;
  qInfo() << "y grid" << y_grid_inf << y_grid_sup << number_of_horizontal_lines;

  QcMapContainerNode * container_node = map_root_node->tile_node;
  // QSGNode * node = container_node->firstChild();
  // container_node->removeAllChildNodes();
  // if (node) {
  //   qInfo() << "Delete node";
  //   // delete node;
  // }

  QMatrix4x4 container_space_matrix;
  container_space_matrix.scale(2/width, 2/height);
  container_space_matrix.translate(-width/2, -height/2);
  container_node->setMatrix(container_space_matrix);
  qInfo() << "container_space_matrix" << container_space_matrix;

  QSGGeometryNode * geometry_node = new QSGGeometryNode();

  const int vertex_count = 2*(number_of_horizontal_lines + number_of_vertical_lines) + 100;
  QSGGeometry * geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), vertex_count);
  geometry->setLineWidth(2);
  geometry->setDrawingMode(GL_LINES);
  geometry_node->setGeometry(geometry);
  geometry_node->setFlag(QSGNode::OwnsGeometry);

  QSGFlatColorMaterial * material = new QSGFlatColorMaterial;
  material->setColor(QColor(255, 0, 0));
  geometry_node->setMaterial(material);
  geometry_node->setFlag(QSGNode::OwnsMaterial);

  QSGGeometry::Point2D * vertices = geometry->vertexDataAsPoint2D();
  size_t vertex_index = 0;
  double x_inf_px = x_inf_m / resolution;
  double y_inf_px = y_inf_m / resolution;
  qInfo() << "inf px" << x_inf_px << y_inf_px;
  double x_offset = x_grid_inf * tile_size - x_inf_px;
  double y_offset = y_grid_inf * tile_size - y_inf_px;
  qInfo() << "offset px" << x_offset << y_offset;
  for (size_t i = 0; i <= number_of_vertical_lines; i++) {
    float x = i * tile_size + x_offset;
    qInfo() << i << x;
    vertices[vertex_index].set(x, 0);
    vertices[vertex_index +1].set(x, height);
    vertex_index += 2;
  }
  for (size_t i = 0; i <= number_of_horizontal_lines; i++) {
    float y = i * tile_size + y_offset;
    qInfo() << i << y;
    vertices[vertex_index].set(0, y);
    vertices[vertex_index +1].set(width, y);
    vertex_index += 2;
  }
  // parasite lines at left and bottom ???

  container_node->removeAllChildNodes(); // delete
  container_node->appendChildNode(geometry_node);
  // geometry_node->markDirty(QSGNode::DirtyGeometry);

  return map_root_node;
}

/**************************************************************************************************/

#include "declarative_map_item.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
