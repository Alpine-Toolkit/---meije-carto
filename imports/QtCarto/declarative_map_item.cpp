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
#include "location_circle_material_shader.h"
#include "map/earth.h"
#include "map/geo_coordinate.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcMapItem::QcMapItem(QQuickItem * parent)
  : QQuickItem(parent),
    m_plugin(nullptr),
    m_viewport(nullptr),
    m_gesture_area(new QcMapGestureArea(this))
{
  qInfo() << "QcMapItem::QcMapItem";
  setAcceptHoverEvents(false);
  setAcceptedMouseButtons(Qt::LeftButton);
  setFlags(QQuickItem::ItemHasContents | QQuickItem::ItemClipsChildrenToShape);
  // setFlag(ItemHasContents, true);

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

QcMapItem::~QcMapItem()
{
  qInfo() << "QcMapItem::~QcMapItem";
  // delete m_plugin, m_viewport
}

void
QcMapItem::componentComplete()
{
  qInfo() << "QcMapItem::componentComplete";
  // m_component_completed = true;
  m_gesture_area->set_map(this); // Fixme
  QQuickItem::componentComplete();
}

bool
QcMapItem::is_interactive()
{
  return (m_gesture_area->enabled() && m_gesture_area->accepted_gestures()) || m_gesture_area->is_active();
}

void
QcMapItem::mousePressEvent(QMouseEvent *event)
{
  qInfo() << "QcMapItem::mousePressEvent";
  if (is_interactive())
    m_gesture_area->handle_mouse_press_event(event);
  else
    QQuickItem::mousePressEvent(event);
}

void QcMapItem::mouseMoveEvent(QMouseEvent *event)
{
  qInfo() << "QcMapItem::mouseMoveEvent";
  if (is_interactive())
    m_gesture_area->handle_mouse_move_event(event);
  else
    QQuickItem::mouseMoveEvent(event);
}

void QcMapItem::mouseReleaseEvent(QMouseEvent *event)
{
  qInfo() << "QcMapItem::mouseReleaseEvent";
  if (is_interactive()) {
    m_gesture_area->handle_mouse_release_event(event);
    ungrabMouse();
  } else {
    QQuickItem::mouseReleaseEvent(event);
  }
}

void QcMapItem::mouseUngrabEvent()
{
  qInfo() << "QcMapItem::mouseUngrabEvent";
  if (is_interactive())
    m_gesture_area->handle_mouse_ungrab_event();
  else
    QQuickItem::mouseUngrabEvent();
}

void QcMapItem::touchUngrabEvent()
{
  qInfo() << "QcMapItem::touchUngrabEvent";
  if (is_interactive())
    m_gesture_area->handle_touch_ungrab_event();
  else
    QQuickItem::touchUngrabEvent();
}

void
QcMapItem::touchEvent(QTouchEvent * event)
{
  qInfo() << "QcMapItem::touchEvent";
  if (is_interactive()) {
    m_gesture_area->handle_touch_event(event);
    if (event->type() == QEvent::TouchEnd ||
        event->type() == QEvent::TouchCancel) {
      ungrabTouchPoints();
    }
  } else {
    //ignore event so sythesized event is generated;
    QQuickItem::touchEvent(event);
  }
}

void
QcMapItem::wheelEvent(QWheelEvent * event)
{
  qInfo() << "QcMapItem::wheelEvent";
  if (is_interactive())
    m_gesture_area->handle_wheel_event(event);
  else
    QQuickItem::wheelEvent(event);
}

/*!
  \qmlproperty MapGestureArea QtLocation::Map::gesture

  Contains the MapGestureArea created with the Map. This covers pan, flick and pinch gestures.
  Use \c{gesture.enabled: true} to enable basic gestures, or see \l{MapGestureArea} for
  further details.
*/
QcMapGestureArea *
QcMapItem::gesture()
{
  return m_gesture_area;
}

/*!
    \qmlmethod coordinate QtLocation::Map::toCoordinate(QPointF position, bool clipToViewPort)

    Returns the coordinate which corresponds to the \a position relative to the map item.

    If \a cliptoViewPort is \c true, or not supplied then returns an invalid coordinate if
    \a position is not within the current viewport.
*/
QGeoCoordinate
QcMapItem::to_coordinate(const QPointF & position, bool clip_to_view_port) const
{
  qInfo() << "QcMapItem::to_coordinate";
  // if (m_map)
  //   return m_map->itemPositionToCoordinate(QDoubleVector2D(position), clipToViewPort);
  // else
  return QGeoCoordinate();
}

/*!
    \qmlmethod point QtLocation::Map::fromCoordinate(coordinate coordinate, bool clipToViewPort)

    Returns the position relative to the map item which corresponds to the \a coordinate.

    If \a cliptoViewPort is \c true, or not supplied then returns an invalid QPointF if
    \a coordinate is not within the current viewport.
*/
QPointF
QcMapItem::from_coordinate(const QGeoCoordinate & coordinate, bool clip_to_view_port) const
{
  qInfo() << "QcMapItem::from_coordinate";
  // if (m_map)
  //   return m_map->coordinateToItemPosition(coordinate, clipToViewPort).toPointF();
  // else
  return QPointF(qQNaN(), qQNaN());
}

// QDoubleVector2D / QVector2D use float ...
QGeoCoordinate
QcMapItem::item_position_to_coordinate(const QVector2D & pos, bool clip_to_viewport) const
{
  qInfo() << "QcMapItem::item_position_to_coordinate" << pos << clip_to_viewport;

  if (clip_to_viewport) {
    int w = width();
    int h = height();

    if ((pos.x() < 0) || (w < pos.x()) || (pos.y() < 0) || (h < pos.y()))
      return QGeoCoordinate();
  }

  QcTileMatrixSet & tile_matrix_set = m_plugin->tile_matrix_set();
  const QcTileMatrix & tile_matrix = tile_matrix_set[m_viewport->zoom_level()];
  double resolution = tile_matrix.resolution(); // [m/px]
  const QcPolygon & polygon = m_viewport->polygon();
  const QcInterval2DDouble & interval = polygon.interval();
  double x_inf_m = interval.x().inf();
  double y_inf_m = interval.y().inf();
  double x = (x_inf_m + pos.x() * resolution) / EQUATORIAL_PERIMETER; // -> normalised_mercator
  double y = (y_inf_m + pos.y() * resolution) / EQUATORIAL_PERIMETER;
  QcGeoCoordinateWGS84 coordinate = QcGeoCoordinateNormalisedMercator(x, y).wgs84();

  QGeoCoordinate _coordinate(coordinate.latitude(), coordinate.longitude()); // QGeoProjection::mercatorToCoord(m_mapScene->itemPositionToMercator(pos));;
  qInfo() << "QcMapItem::item_position_to_coordinate" << _coordinate;
  return _coordinate;
}

QVector2D
QcMapItem::coordinate_to_item_position(const QGeoCoordinate & coordinate, bool clip_to_viewport) const
{
  qInfo() << "QcMapItem::coordinate_to_item_position" << coordinate << clip_to_viewport;

  QcGeoCoordinateNormalisedMercator normalised_mercator = QcGeoCoordinateWGS84(coordinate.longitude(), coordinate.latitude()).normalised_mercator();

  QcTileMatrixSet & tile_matrix_set = m_plugin->tile_matrix_set();
  const QcTileMatrix & tile_matrix = tile_matrix_set[m_viewport->zoom_level()];
  double resolution = tile_matrix.resolution(); // [m/px]
  const QcPolygon & polygon = m_viewport->polygon();
  const QcInterval2DDouble & interval = polygon.interval();
  double x_inf_m = interval.x().inf();
  double y_inf_m = interval.y().inf();
  double x = (normalised_mercator.x()*EQUATORIAL_PERIMETER - x_inf_m)/resolution;
  double y = (normalised_mercator.y()*EQUATORIAL_PERIMETER - y_inf_m)/resolution;
  QVector2D pos(x, y); //  = m_mapScene->mercatorToItemPosition(QGeoProjection::coordToMercator(coordinate));;

  if (clip_to_viewport) {
    int w = width();
    int h = height();
    double x = pos.x();
    double y = pos.y();
    if ((x < 0.0) || (x > w) || (y < 0) || (y > h) || qIsNaN(x) || qIsNaN(y))
      return QVector2D(qQNaN(), qQNaN());
  }

  qInfo() << "QcMapItem::coordinate_to_item_position" << pos;
  return pos;
}

/*
QVector2D
QcMapItem::item_position_to_mercator(const QVector2D & pos) const
{
  double x = m_mercator_width * (((pos.x() - m_screen_offset_x) / m_screen_width) - 0.5);
  x += m_mercator_center_x;

  if (x > 1.0 * m_side_length)
    x -= 1.0 * m_side_length;
  if (x < 0.0)
    x += 1.0 * m_side_length;

  x /= 1.0 * m_side_length;

  double y = m_mercator_height * (((pos.y() - m_screen_offset_y) / m_screen_height) - 0.5);
  y += m_mercator_center_y;
  y /= 1.0 * m_side_length;

  return QDoubleVector2D(x, y);
}

QVector2D
QcMapItem::mercator_to_item_position(const QVector2D & mercator) const
{
  double mx = m_side_length * mercator.x();

  double lb = m_mercator_center_x - m_mercator_width / 2.0;
  if (lb < 0.0)
    lb += m_side_length;
  double ub = m_mercator_center_x + m_mercator_width / 2.0;
  if (m_side_length < ub)
    ub -= m_side_length;

  double m = (mx - m_mercator_center_x) / m_mercator_width;

  double mWrapLower = (mx - m_mercator_center_x - m_side_length) / m_mercator_width;
  double mWrapUpper = (mx - m_mercator_center_x + m_side_length) / m_mercator_width;

  // correct for crossing dateline
  if (qFuzzyCompare(ub - lb + 1.0, 1.0) || (ub < lb) ) {
    if (m_mercator_center_x < ub) {
      if (lb < mx) {
        m = m_wrap_lower;
      }
    } else if (lb < m_mercator_center_x) {
      if (mx <= ub) {
        m = m_wrap_upper;
      }
    }
  }

  // apply wrapping if necessary so we don't return unreasonably large pos/neg screen positions
  // also allows map items to be drawn properly if some of their coords are out of the screen
  if ( qAbs(m_wrap_lower) < qAbs(m) )
    m = mWrapLower;
  if ( qAbs(m_wrap_upper) < qAbs(m) )
    m = mWrapUpper;

  double x = m_screen_width * (0.5 + m);
  double y = m_screen_height * (0.5 + (m_sideLength * mercator.y() - m_mercator_center_y) / m_mercator_height);

  return QDoubleVector2D(x + m_screen_offset_x, y + m_screen_offset_y);
}
*/

void
QcMapItem::prefetch_data()
{
  qInfo() << "QcMapItem::prefetch_data";
}

void
QcMapItem::set_zoom_level(int zoom_level)
{
  qInfo() << "QcMapItem::set_zoom_level " << zoom_level;

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
QcMapItem::zoom_level() const
{
  return m_viewport->zoom_level();
}

void
QcMapItem::set_center(const QGeoCoordinate & center)
{
  qInfo() << "QcMapItem::set_center WGS84 " << center;

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
QcMapItem::center() const
{
  QcGeoCoordinateWGS84 center = m_viewport->wgs84();
  return QGeoCoordinate(center.latitude(), center.longitude());
}

void
QcMapItem::pan(int dx, int dy)
{
  qInfo() << "QcMapItem::pan" << dx << dy;
  m_viewport->pan(dx, dy); // Fixme: unit is m
  update(); // Fixme: signal
}

void
QcMapItem::geometryChanged(const QRectF & new_geometry, const QRectF & old_geometry)
{
  qInfo() << "QcMapItem::geometryChanged" << old_geometry << "->" << new_geometry;
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

    QSGGeometryNode * location_circle_node = new QSGGeometryNode();

    QSGGeometry * location_circle_geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 1);
    location_circle_geometry->setLineWidth(100); // point size
    location_circle_geometry->setDrawingMode(GL_POINTS);
    location_circle_node->setGeometry(location_circle_geometry);
    location_circle_node->setFlag(QSGNode::OwnsGeometry);

    // Fixme: black instead of white
    QSGSimpleMaterial<QcLocationCircleMaterialShaderState> * location_circle_material = QcLocationCircleMaterialShader::createMaterial();
    location_circle_material->state()->r = 0;
    location_circle_material->state()->g = 0;
    location_circle_material->state()->b = 1;
    location_circle_material->state()->a = 0;
    location_circle_node->setMaterial(location_circle_material);
    location_circle_node->setFlag(QSGNode::OwnsMaterial);

    QSGGeometry::Point2D * location_circle_vertices = location_circle_geometry->vertexDataAsPoint2D();
    location_circle_vertices[0].set(0, 0);

    root->appendChildNode(location_circle_node);
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
QcMapItem::updatePaintNode(QSGNode * old_node, UpdatePaintNodeData *)
{
  qInfo() << "QcMapItem::updatePaintNode" << old_node;

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

  QSGGeometryNode * grid_node = new QSGGeometryNode();

  const int vertex_count = 2*(number_of_horizontal_lines + number_of_vertical_lines) + 100;
  QSGGeometry * grid_geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), vertex_count);
  grid_geometry->setLineWidth(2);
  grid_geometry->setDrawingMode(GL_LINES);
  grid_node->setGeometry(grid_geometry);
  grid_node->setFlag(QSGNode::OwnsGeometry);

  QSGFlatColorMaterial * grid_material = new QSGFlatColorMaterial;
  grid_material->setColor(QColor(255, 0, 0));
  grid_node->setMaterial(grid_material);
  grid_node->setFlag(QSGNode::OwnsMaterial);

  QSGGeometry::Point2D * vertices = grid_geometry->vertexDataAsPoint2D();
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
  container_node->appendChildNode(grid_node);
  // grid_node->markDirty(QSGNode::DirtyGeometry);

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
