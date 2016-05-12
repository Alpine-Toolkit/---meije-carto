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

#include "declarative_map_item.h"

#include "earth.h"
#include "coordinate/geo_coordinate.h"
#include "wmts/geoportail/geoportail_plugin.h"
#include "wmts/osm/osm_plugin.h"

#include <cmath>

#include <QDir>
#include <QStandardPaths>

#include <QSGSimpleRectNode>
#include <QtDebug>
#include <QtQuick/qsgnode.h>

// Fixme: Use QtQuick Private API !!!
#include <private/qquickwindow_p.h>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcWmtsPluginLayerData::QcWmtsPluginLayerData()
  : QObject(),
    m_plugin_layer(nullptr),
    m_status(false),
    m_opacity(1.)
{}

QcWmtsPluginLayerData::QcWmtsPluginLayerData(const QcWmtsPluginLayer * plugin_layer)
  : QObject(),
    m_plugin_layer(plugin_layer),
    m_status(false),
    m_opacity(1.)
{}

QcWmtsPluginLayerData::QcWmtsPluginLayerData(const QcWmtsPluginLayerData & other)
  : QObject(),
    m_plugin_layer(other.m_plugin_layer),
    m_status(other.m_status),
    m_opacity(other.m_opacity)
{}

QcWmtsPluginLayerData &
QcWmtsPluginLayerData::operator=(const QcWmtsPluginLayerData & other)
{
  if (this != &other) {
    m_plugin_layer = other.m_plugin_layer;
    m_status = other.m_status;
    m_opacity = other.m_opacity;
  }

  return *this;
}

void
QcWmtsPluginLayerData::set_status(bool status)
{
  qInfo() << status;
  if (m_status != status) {
    m_status = status;
    emit statusChanged(status);
  }
}

void
QcWmtsPluginLayerData::set_opacity(float opacity)
{
  qInfo() << opacity;
  if (m_opacity != opacity) {
    m_opacity = opacity;
    emit opacityChanged(opacity);
  }
}

/**************************************************************************************************/

QcMapItem::QcMapItem(QQuickItem * parent)
  : QQuickItem(parent),
    m_color(),
    m_gesture_area(new QcMapGestureArea(this)),
    m_plugin_manager(QcWmtsPluginManager::instance()),
    m_map_view(nullptr),
    m_viewport(nullptr)
{
  qInfo();
  setAcceptHoverEvents(false);
  setAcceptedMouseButtons(Qt::LeftButton);
  setFlags(QQuickItem::ItemHasContents | QQuickItem::ItemClipsChildrenToShape);
  setFiltersChildMouseEvents(true);

  m_map_view = new QcMapView();
  m_viewport = m_map_view->viewport();

  // Fixme:
  m_gesture_area->set_minimum_zoom_level(0);
  m_gesture_area->set_maximum_zoom_level(20);

  for (const auto & plugin_name : m_plugin_manager.plugin_names())
    m_plugin_layers.insert(plugin_name, make_plugin_layers(plugin_name));

  connect(m_map_view, &QcMapView::scene_graph_changed, this, &QQuickItem::update);

  // Fixme: remove
  // Set default viewport center and zoom level
  size_t level = 10;
  double longitude =  2.3491; // Paris Notre-Dame
  double latitude  = 48.8533;
  QcGeoCoordinateWGS84 coordinate(longitude, latitude);
  m_viewport->zoom_at(coordinate.pseudo_web_mercator(), level);
}

QcMapItem::~QcMapItem()
{
  qInfo();
  // delete m_plugin, m_viewport
}

void
QcMapItem::componentComplete()
{
  qInfo();
  // m_component_completed = true;
  // m_gesture_area->set_map(this);
  QQuickItem::componentComplete();
}

QVariantList
QcMapItem::make_plugin_layers(const QString & plugin_name)
{
  QVariantList plugin_layers;
  const QcWmtsPlugin * plugin = m_plugin_manager[plugin_name];
  if (plugin)
    for (const auto * layer : plugin->layers()) {
      QcWmtsPluginLayerData * layer_data = new QcWmtsPluginLayerData(layer);
      connect(layer_data, &QcWmtsPluginLayerData::statusChanged, this, &QcMapItem::layer_status_changed);
      connect(layer_data, &QcWmtsPluginLayerData::opacityChanged, this, &QcMapItem::layer_opacity_changed);
      plugin_layers << QVariant::fromValue(layer_data);
    }
  return plugin_layers;
}

QStringList
QcMapItem::plugin_names() const
{
  return m_plugin_manager.plugin_names();
}

QVariantList
QcMapItem::plugin_layers(const QString & plugin_name)
{
  if (m_plugin_layers.contains(plugin_name))
    return m_plugin_layers[plugin_name];
  else
    return QVariantList();
}

void
QcMapItem::layer_status_changed(bool status)
{
  QcWmtsPluginLayerData * layer_data = qobject_cast<QcWmtsPluginLayerData *>(QObject::sender());
  const QcWmtsPluginLayer * plugin_layer = layer_data->plugin_layer();
  qInfo() << plugin_layer->hash_name() << status;
  if (status)
    m_map_view->add_layer(plugin_layer);
  else
    m_map_view->remove_layer(plugin_layer);
  update();
}

void
QcMapItem::layer_opacity_changed(float opacity)
{
  QcWmtsPluginLayerData * layer_data = qobject_cast<QcWmtsPluginLayerData *>(QObject::sender());
  const QcWmtsPluginLayer * plugin_layer = layer_data->plugin_layer();
  qInfo() << plugin_layer->hash_name() << opacity;
  if (layer_data->status()) {
    m_map_view->set_opacity(plugin_layer, opacity);
    update();
  }
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
  qInfo();
  if (is_interactive())
    m_gesture_area->handle_mouse_press_event(event);
  else
    QQuickItem::mousePressEvent(event);
}

void QcMapItem::mouseMoveEvent(QMouseEvent *event)
{
  qInfo();
  if (is_interactive())
    m_gesture_area->handle_mouse_move_event(event);
  else
    QQuickItem::mouseMoveEvent(event);
}

void QcMapItem::mouseReleaseEvent(QMouseEvent *event)
{
  qInfo();
  if (is_interactive()) {
    m_gesture_area->handle_mouse_release_event(event);
    ungrabMouse();
  } else {
    QQuickItem::mouseReleaseEvent(event);
  }
}

void QcMapItem::mouseUngrabEvent()
{
  qInfo();
  if (is_interactive())
    m_gesture_area->handle_mouse_ungrab_event();
  else
    QQuickItem::mouseUngrabEvent();
}

void QcMapItem::touchUngrabEvent()
{
  qInfo();
  if (is_interactive())
    m_gesture_area->handle_touch_ungrab_event();
  else
    QQuickItem::touchUngrabEvent();
}

void
QcMapItem::touchEvent(QTouchEvent * event)
{
  qInfo();
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
  qInfo();
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
QcMapItem::to_coordinate(const QVector2D & position_px, bool clip_to_viewport) const
{
  QcVectorDouble _position_px(position_px.x(), position_px.y());
  QcGeoCoordinateWGS84 coordinate = m_viewport->to_coordinate(_position_px, clip_to_viewport).wgs84();
  return coordinate.to_qt();
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
  QcGeoCoordinateWGS84 _coordinate(coordinate);
  QcVectorDouble position_px = m_viewport->from_coordinate(_coordinate, clip_to_viewport);
  return QVector2D(position_px.x(), position_px.y());
}

void
QcMapItem::prefetch_data()
{
  qInfo();
}

void
QcMapItem::set_zoom_level(int zoom_level)
{
  qInfo() << zoom_level;

  if (zoom_level == m_viewport->zoom_level())
    return;

  // Fixme: check range
  if (zoom_level < 0 || zoom_level > 18)
    return;

  m_viewport->set_zoom_level(zoom_level);
  // update(); // Fixme: signal
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
  // Fixme: check latitude

  qInfo() << "WGS84 " << center;

  QcGeoCoordinateWGS84 coordinate(center);
  if (coordinate == m_viewport->wgs84())
    return;

  if (!center.isValid())
    return;
  // coordinate.setLatitude(qBound(-m_maximumViewportLatitude, center.latitude(), m_maximumViewportLatitude));

  m_viewport->set_coordinate(coordinate);
  // update(); // Fixme: signal
  emit centerChanged(center);

  // QcGeoCoordinateMercator mercator_coordinate = m_viewport->mercator();
  // qInfo() << "mercator " << mercator_coordinate.x() << mercator_coordinate.y();
  // QcGeoCoordinateNormalisedMercator normalised_mercator_coordinate = m_viewport->normalised_mercator();
  // qInfo() << "normalised mercator " << normalised_mercator_coordinate.x() << normalised_mercator_coordinate.y();
}

QGeoCoordinate
QcMapItem::center() const
{
  return m_viewport->wgs84().to_qt();
}

void
QcMapItem::pan(int dx, int dy)
{
  qInfo() << dx << dy;
  m_viewport->pan(dx, dy); // Fixme: unit is m
  update(); // Fixme: signal
}

void
QcMapItem::stable_zoom(QPointF position_px, int zoom_level)
{
  QcVectorDouble _position_px(position_px.x(), position_px.y());
  m_viewport->stable_zoom(_position_px, zoom_level);
}

void
QcMapItem::stable_zoom_by_increment(QPointF position_px, int zoom_increment)
{
  stable_zoom(position_px, zoom_level() + zoom_increment);
}

void
QcMapItem::geometryChanged(const QRectF & new_geometry, const QRectF & old_geometry)
{
  qInfo() << old_geometry << "->" << new_geometry;
  QQuickItem::geometryChanged(new_geometry, old_geometry);
  QSize viewport_size(new_geometry.width(), new_geometry.height()); // Fixme: QSizeF size()
  m_viewport->set_viewport_size(viewport_size);
}

QSGNode *
QcMapItem::updatePaintNode(QSGNode * old_node, UpdatePaintNodeData *)
{
  qInfo() << old_node;

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

bool
QcMapItem::childMouseEventFilter(QQuickItem * item, QEvent * event)
{
  Q_UNUSED(item);

  // item is QQuickMouseArea
  qInfo() << item << "\n" << event;

  if (!isVisible() || !isEnabled() || !is_interactive())
    return QQuickItem::childMouseEventFilter(item, event);

  switch (event->type()) {
  case QEvent::MouseButtonPress:
  case QEvent::MouseMove:
  case QEvent::MouseButtonRelease:
    return send_mouse_event(static_cast<QMouseEvent *>(event));

  case QEvent::UngrabMouse: {
    QQuickWindow * _window = window();
    if (!_window)
      break;
    if (!_window->mouseGrabberItem() ||
        (_window->mouseGrabberItem() && _window->mouseGrabberItem() != this)) {
      // child lost grab, we could even lost some events
      // if grab already belongs for example in item in diffrent window,
      // clear up states
      mouseUngrabEvent();
    }
    break;
  }

  case QEvent::TouchBegin:
  case QEvent::TouchUpdate:
  case QEvent::TouchEnd:
  case QEvent::TouchCancel:
    if (static_cast<QTouchEvent *>(event)->touchPoints().count() >= 2) {
      // 1 touch point = handle with MouseEvent (event is always synthesized)
      // let the synthesized mouse event grab the mouse,
      // note there is no mouse grabber at this point since
      // touch event comes first (see Qt::AA_SynthesizeMouseForUnhandledTouchEvents)
      return send_touch_event(static_cast<QTouchEvent *>(event));
    }

  default:
    break;
  }

  return QQuickItem::childMouseEventFilter(item, event);
}

bool
QcMapItem::send_mouse_event(QMouseEvent * event)
{
  QPointF local_position = mapFromScene(event->windowPos());
  QQuickWindow * _window = window();
  QQuickItem * grabber = _window ? _window->mouseGrabberItem() : nullptr;
  bool steal_event = m_gesture_area->is_active();

  // grabber is QQuickMouseArea
  qInfo() << event << "\ngrabber" << grabber << "\nsteal_event" << steal_event;

  if ((steal_event || contains(local_position)) && (!grabber || !grabber->keepMouseGrab())) {
    QScopedPointer<QMouseEvent> mouseEvent(QQuickWindowPrivate::cloneMouseEvent(event, &local_position));
    mouseEvent->setAccepted(false);

    switch (mouseEvent->type()) {
    case QEvent::MouseMove:
      m_gesture_area->handle_mouse_move_event(mouseEvent.data());
      break;
    case QEvent::MouseButtonPress:
      m_gesture_area->handle_mouse_press_event(mouseEvent.data());
      break;
    case QEvent::MouseButtonRelease:
      m_gesture_area->handle_mouse_release_event(mouseEvent.data());
      break;
    default:
      break;
    }

    // Fixme: duplicated code ???
    steal_event = m_gesture_area->is_active();
    grabber = _window ? _window->mouseGrabberItem() : nullptr;

    if (grabber && steal_event && !grabber->keepMouseGrab() && grabber != this) {
      qInfo() << "grab mouse";
      grabMouse();
    }

    if (steal_event) {
      // do not deliver
      event->setAccepted(true);
      return true;
    } else {
      return false;
    }
  }

  if (event->type() == QEvent::MouseButtonRelease) {
    if (_window && _window->mouseGrabberItem() == this) {
      qInfo() << "ungrab mouse";
      ungrabMouse();
    }
  }

  return false;
}

bool
QcMapItem::send_touch_event(QTouchEvent * event)
{
  QQuickWindowPrivate * _window = window() ? QQuickWindowPrivate::get(window()) : 0;
  const QTouchEvent::TouchPoint & point = event->touchPoints().first();
  QQuickItem * grabber = _window ? _window->itemForTouchPointId.value(point.id()) : 0;

  bool steal_event = m_gesture_area->is_active();
  bool contains_point = contains(mapFromScene(point.scenePos()));

  if ((steal_event || contains_point) && (!grabber || !grabber->keepTouchGrab())) {
    QScopedPointer<QTouchEvent> touchEvent(new QTouchEvent(event->type(), event->device(), event->modifiers(),
                                                           event->touchPointStates(), event->touchPoints()));
    touchEvent->setTimestamp(event->timestamp());
    touchEvent->setAccepted(false);

    m_gesture_area->handle_touch_event(touchEvent.data());
    steal_event = m_gesture_area->is_active();
    grabber = _window ? _window->itemForTouchPointId.value(point.id()) : nullptr;

    if (grabber && steal_event && !grabber->keepTouchGrab() && grabber != this) {
      QVector<int> ids;
      foreach (const QTouchEvent::TouchPoint &tp, event->touchPoints()) {
        if (!(tp.state() & Qt::TouchPointReleased)) {
          ids.append(tp.id());
        }
      }
      grabTouchPoints(ids);
    }

    if (steal_event) {
      // do not deliver
      event->setAccepted(true);
      return true;
    } else {
      return false;
    }
  }

  if (event->type() == QEvent::TouchEnd) {
    if (_window && _window->itemForTouchPointId.value(point.id()) == this)
      ungrabTouchPoints();
  }

  return false;
}

/**************************************************************************************************/

// #include "declarative_map_item.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
