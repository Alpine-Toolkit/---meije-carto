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

#include <QSGSimpleRectNode>
#include <QtDebug>
#include <QtQuick/qsgnode.h>

/**************************************************************************************************/

#include "declarative_map_item.h"
#include "map/earth.h"
#include "map/geo_coordinate.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcMapItem::QcMapItem(QQuickItem * parent)
  : QQuickItem(parent),
    m_color(),
    m_gesture_area(new QcMapGestureArea(this)),
    m_plugin(nullptr),
    m_map_view(nullptr),
    m_viewport(nullptr)
{
  qInfo() << "QcMapItem::QcMapItem";
  setAcceptHoverEvents(false);
  setAcceptedMouseButtons(Qt::LeftButton);
  setFlags(QQuickItem::ItemHasContents | QQuickItem::ItemClipsChildrenToShape);
  // setFlag(ItemHasContents, true);

  // Fixme: init plugin where ???
  QString json_path("geoportail-license.json");
  QcGeoportailWmtsLicence geoportail_license(json_path);
  m_plugin = new QcGeoportailPlugin(geoportail_license);

  m_map_view = new QcMapView(m_plugin);
  m_viewport = m_map_view->viewport();

  // Fixme: remove
  // Set default viewport center and zoom level
  size_t level = 16;
  double longitude =  2.3491; // Paris Notre-Dame
  double latitude  = 48.8533;
  QcGeoCoordinateWGS84 coordinate(longitude, latitude);
  m_viewport->zoom_at(coordinate.web_mercator(), level);
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

void
QcMapItem::set_color(const QColor & color)
{
  if (color != m_color) {
    m_color = color;
    update();
    emit colorChanged(m_color);
  }
}

QColor
QcMapItem::color() const
{
  return m_color;
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
    \qmlmethod coordinate QtLocation::Map::to_coordinate(QPointF position, bool clipToViewPort)

    Returns the coordinate which corresponds to the \a position relative to the map item.

    If \a cliptoViewPort is \c true, or not supplied then returns an invalid coordinate if
    \a position is not within the current viewport.
*/
QGeoCoordinate
QcMapItem::to_coordinate(const QVector2D & pos, bool clip_to_viewport) const
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
  QcGeoCoordinateWGS84 coordinate = QcGeoCoordinateNormalisedWebMercator(x, y).wgs84();

  QGeoCoordinate _coordinate(coordinate.latitude(), coordinate.longitude()); // QGeoProjection::mercatorToCoord(m_mapScene->itemPositionToMercator(pos));;
  qInfo() << "QcMapItem::item_position_to_coordinate" << _coordinate;
  // return QGeoCoordinate();
  return _coordinate;
}

/*!
    \qmlmethod point QtLocation::Map::from_coordinate(coordinate coordinate, bool clipToViewPort)

    Returns the position relative to the map item which corresponds to the \a coordinate.

    If \a cliptoViewPort is \c true, or not supplied then returns an invalid QPointF if
    \a coordinate is not within the current viewport.
*/
QVector2D
QcMapItem::from_coordinate(const QGeoCoordinate & coordinate, bool clip_to_viewport) const
{
  qInfo() << "QcMapItem::coordinate_to_item_position" << coordinate << clip_to_viewport;

  QcGeoCoordinateNormalisedWebMercator normalised_mercator = QcGeoCoordinateWGS84(coordinate.longitude(), coordinate.latitude()).normalised_web_mercator();

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
  // return QPointF(qQNaN(), qQNaN());
  return pos;
}

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
  m_map_view->map_scene()->set_item_size(viewport_size); // Fixme: cf. supra
}

QSGNode *
QcMapItem::updatePaintNode(QSGNode * old_node, UpdatePaintNodeData *)
{
  qInfo() << "QcMapItem::updatePaintNode" << old_node;

  // if (!m_map) {
  //   delete old_node;
  //   return nullptr;
  // }

  QSGSimpleRectNode * root = static_cast<QSGSimpleRectNode *>(old_node);
  if (!root)
    root = new QSGSimpleRectNode(boundingRect(), m_color);
  else {
    root->setRect(boundingRect());
    root->setColor(m_color);
  }

  QSGNode * content = root->childCount() ? root->firstChild() : 0;
  content = m_map_view->update_scene_graph(content, window());
  if (content && root->childCount() == 0)
    root->appendChildNode(content);

  return root;
}

/**************************************************************************************************/

#include "declarative_map_item.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
