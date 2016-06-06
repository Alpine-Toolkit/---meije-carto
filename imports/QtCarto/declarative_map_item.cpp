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
#include "coordinate/wgs84.h"
#include "map/map_path_editor.h"

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

QcMapItem::QcMapItem(QQuickItem * parent)
  : QQuickItem(parent),
    m_color(),
    m_gesture_area(new QcMapGestureArea(this)),
    m_plugin_manager(QcWmtsPluginManager::instance()),
    m_map_view(nullptr),
    m_viewport(nullptr)
{
  // qInfo();
  setAcceptHoverEvents(false);
  setAcceptedMouseButtons(Qt::LeftButton);
  setFlags(QQuickItem::ItemHasContents | QQuickItem::ItemClipsChildrenToShape);
  // Filter mouse events of child items
  // childMouseEventFilter() will be called when a mouse event is triggered for a child item.
  setFiltersChildMouseEvents(true);

  m_map_view = new QcMapView();
  m_viewport = m_map_view->viewport();

  // Fixme:
  m_gesture_area->set_minimum_zoom_level(0);
  m_gesture_area->set_maximum_zoom_level(20);

  // Fixme: direct ptr
  // Fixme: in map_view ?
  m_map_event_router.register_client(QcMapEventRouter::ClientSharedPointer(new QcMapPathEditor(m_map_view)));

  for (const auto & plugin_name : m_plugin_manager.plugin_names())
    m_plugin_layers.insert(plugin_name, make_plugin_layers(plugin_name));

  connect(m_map_view, &QcMapView::scene_graph_changed, this, &QQuickItem::update);

  // Fixme: remove
  // Set default viewport center and zoom level
  int level = 10;
  double longitude =  2.3491; // Paris Notre-Dame
  double latitude  = 48.8533;
  QcWgsCoordinate coordinate(longitude, latitude);
  m_viewport->zoom_at(coordinate, level);
}

QcMapItem::~QcMapItem()
{
  // qInfo();
  // delete m_plugin, m_viewport
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

void
QcMapItem::componentComplete()
{
  // qInfo();
  // m_component_completed = true;
  QQuickItem::componentComplete();
}

/**************************************************************************************************/

/*!
  \qmlproperty MapGestureArea QtLocation::Map::gesture

  Contains the MapGestureArea created with the Map. This covers pan, flick and pinch gestures.
  Use \c{gesture.enabled: true} to enable basic gestures, or see \l{MapGestureArea} for
  further details.
*/

/**************************************************************************************************/

bool
QcMapItem::is_interactive()
{
  return (m_gesture_area->enabled() and m_gesture_area->accepted_gestures()) or m_gesture_area->is_active();
}

void
QcMapItem::mousePressEvent(QMouseEvent * event)
{
  // Never called when a child (MouseArea) should receive th event
  qInfo();
  if (is_interactive())
    m_gesture_area->handle_mouse_press_event(event);
  else
    QQuickItem::mousePressEvent(event);
}

void
QcMapItem::mouseMoveEvent(QMouseEvent * event)
{
  // Called when mouse is grabbed
  qInfo();
  if (is_interactive())
    m_gesture_area->handle_mouse_move_event(event);
  else
    QQuickItem::mouseMoveEvent(event);
}

void
QcMapItem::mouseReleaseEvent(QMouseEvent * event)
{
  // Called when mouse is grabbed
  qInfo();
  if (is_interactive()) {
    m_gesture_area->handle_mouse_release_event(event);
    ungrabMouse();
  } else
    QQuickItem::mouseReleaseEvent(event);
}

void
QcMapItem::mouseUngrabEvent()
{
  // qInfo();
  if (is_interactive())
    m_gesture_area->handle_mouse_ungrab_event();
  else
    QQuickItem::mouseUngrabEvent();
}

void
QcMapItem::touchUngrabEvent()
{
  // qInfo();
  if (is_interactive())
    m_gesture_area->handle_touch_ungrab_event();
  else
    QQuickItem::touchUngrabEvent();
}

void
QcMapItem::touchEvent(QTouchEvent * event)
{
  // qInfo();
  if (is_interactive()) {
    m_gesture_area->handle_touch_event(event);
    if (event->type() == QEvent::TouchEnd or
        event->type() == QEvent::TouchCancel) {
      ungrabTouchPoints();
    }
  } else
    // ignore event so sythesized event is generated;
    QQuickItem::touchEvent(event);
}

void
QcMapItem::wheelEvent(QWheelEvent * event)
{
  // qInfo();
  if (is_interactive())
    m_gesture_area->handle_wheel_event(event);
  else
    QQuickItem::wheelEvent(event);
}

bool
QcMapItem::childMouseEventFilter(QQuickItem * item, QEvent * event)
{
  Q_UNUSED(item);

  // item is QQuickMouseArea
  qInfo() << item << "\n" << event;

  if (!isVisible() or !isEnabled() or !is_interactive())
    return QQuickItem::childMouseEventFilter(item, event);

  switch (event->type()) {
  case QEvent::MouseButtonPress:
  case QEvent::MouseMove:
  case QEvent::MouseButtonRelease:
    return send_mouse_event(static_cast<QMouseEvent *>(event));

  case QEvent::TouchBegin:
  case QEvent::TouchUpdate:
  case QEvent::TouchEnd:
  case QEvent::TouchCancel: {
    QTouchEvent * touch_event = static_cast<QTouchEvent *>(event);
    if (touch_event->touchPoints().count() >= 2)
      // 1 touch point = handle with MouseEvent (event is always synthesized)
      // let the synthesized mouse event grab the mouse,
      // note there is no mouse grabber at this point since
      // touch event comes first (see Qt::AA_SynthesizeMouseForUnhandledTouchEvents)
      return send_touch_event(touch_event);
  }

  case QEvent::UngrabMouse: {
    QQuickWindow * _window = window();
    if (!_window and
        (( !_window->mouseGrabberItem() or
          ( _window->mouseGrabberItem() and _window->mouseGrabberItem() != this ))))
      // child lost grab, we could even lost some events
      // if grab already belongs for example in item in diffrent window,
      // clear up states
      mouseUngrabEvent();
    break;
  }

  default:
    break;
  }

  return QQuickItem::childMouseEventFilter(item, event);
}

bool
QcMapItem::send_mouse_event(QMouseEvent * event)
{
  qInfo() << event;

  QPointF local_position = mapFromScene(event->windowPos());
  QQuickWindow * _window = window();
  QQuickItem * grabber = _window ? _window->mouseGrabberItem() : nullptr;
  bool steal_event = m_gesture_area->is_active(); // means pan or pinch is active

  // grabber is QQuickMouseArea, steal_event is false first then true
  qInfo() << event << "\ngrabber" << grabber << "\nsteal_event" << steal_event;

  if ((steal_event or contains(local_position)) and (!grabber or !grabber->keepMouseGrab())) {
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

    steal_event = m_gesture_area->is_active(); // recheck value
    // Fixme: duplicated code ???
    grabber = _window ? _window->mouseGrabberItem() : nullptr;
    qInfo() << "grabber" << grabber << "\nsteal_event" << steal_event;

    if (grabber and steal_event and !grabber->keepMouseGrab() and grabber != this) {
      // qInfo() << "grab mouse";
      grabMouse();
    }

    if (steal_event) {
      event->setAccepted(true);
      return true; // do not deliver event
    } else
      return false; // deliver event

  } else {
    // ungrab if necessary and deliver event
    if (event->type() == QEvent::MouseButtonRelease
        and (_window and _window->mouseGrabberItem() == this))
      // qInfo() << "ungrab mouse";
      ungrabMouse();
    return false; // deliver event
  }
}

bool
QcMapItem::send_touch_event(QTouchEvent * event)
{
  QQuickWindowPrivate * _window = window() ? QQuickWindowPrivate::get(window()) : nullptr;
  const QTouchEvent::TouchPoint & point = event->touchPoints().first();
  QQuickItem * grabber = _window ? _window->itemForTouchPointId.value(point.id()) : nullptr;

  bool steal_event = m_gesture_area->is_active();
  bool contains_point = contains(mapFromScene(point.scenePos()));

  if ((steal_event or contains_point) and (!grabber or !grabber->keepTouchGrab())) {
    QScopedPointer<QTouchEvent> touch_event(new QTouchEvent(event->type(), event->device(), event->modifiers(),
                                                            event->touchPointStates(), event->touchPoints()));
    touch_event->setTimestamp(event->timestamp());
    touch_event->setAccepted(false);

    m_gesture_area->handle_touch_event(touch_event.data());

    steal_event = m_gesture_area->is_active(); // recheck value
    // Fixme: duplicated code ???
    grabber = _window ? _window->itemForTouchPointId.value(point.id()) : nullptr;

    if (grabber and steal_event and !grabber->keepTouchGrab() and grabber != this) {
      QVector<int> ids;
      for (const auto & touch_point : event->touchPoints())
        if (!(touch_point.state() & Qt::TouchPointReleased))
          ids.append(touch_point.id());
      grabTouchPoints(ids);
    }

    if (steal_event) {
      // do not deliver
      event->setAccepted(true);
      return true;
    } else
      return false;

  } else {
    // ungrab if necessary and deliver event
    if (event->type() == QEvent::TouchEnd
        and (_window and _window->itemForTouchPointId.value(point.id()) == this))
      ungrabTouchPoints();
    return false;
  }
}

/**************************************************************************************************/

void
QcMapItem::set_zoom_level(unsigned int new_zoom_level)
{
  // qInfo() << new_zoom_level;

  if (new_zoom_level == zoom_level())
    return;

  // Fixme: check range
  // Fixme: int
  if (new_zoom_level < 0 or new_zoom_level > 18)
    return;

  m_viewport->set_zoom_level(new_zoom_level);
  // update(); // Fixme: signal
  emit zoom_levelChanged(new_zoom_level);
}

void
QcMapItem::set_center_qt(const QGeoCoordinate & coordinate)
{
  set_center(QcWgsCoordinate(coordinate));
}

void
QcMapItem::set_center(const QcWgsCoordinate & coordinate)
{
  // qInfo() << "WGS84 " << coordinate;

  if (coordinate == m_viewport->center())
    return;

  // Fixme: check latitude
  // if (!center.isValid())
  //   return;
  // coordinate.setLatitude(qBound(-m_maximumViewportLatitude, center.latitude(), m_maximumViewportLatitude));

  m_viewport->set_center(coordinate);
  // update(); // Fixme: signal

  emit centerChanged(coordinate);
}

QcWgsCoordinate
QcMapItem::center() const
{
  return m_viewport->center();
}

QGeoCoordinate
QcMapItem::center_qt() const
{
  return center().to_qt();
}

void
QcMapItem::pan(int dx, int dy)
{
  // qInfo() << dx << dy;
  m_viewport->pan(dx, dy); // Fixme: unit is m
  update(); // Fixme: signal
}

void
QcMapItem::stable_zoom(QPointF position_px, unsigned int new_zoom_level)
{
  if (new_zoom_level == zoom_level())
    return;

  // Fixme: check range
  // Fixme: int
  if (new_zoom_level < 0 or new_zoom_level > 18)
    return;

  QcVectorDouble _position_px(position_px.x(), position_px.y());
  m_viewport->stable_zoom(_position_px, new_zoom_level);
  emit zoom_levelChanged(new_zoom_level);
}

void
QcMapItem::stable_zoom_by_increment(QPointF position_px, int zoom_increment)
{
  int new_zoom_level = zoom_level() + zoom_increment;
  stable_zoom(position_px, new_zoom_level);
}

/**************************************************************************************************/

void
QcMapItem::geometryChanged(const QRectF & new_geometry, const QRectF & old_geometry)
{
  // qInfo() << old_geometry << "->" << new_geometry;
  QQuickItem::geometryChanged(new_geometry, old_geometry);
  QSize viewport_size(new_geometry.width(), new_geometry.height()); // Fixme: QSizeF size()
  // Fixme: pass devicePixelRatio here ?
  m_viewport->set_viewport_size(viewport_size, window()->devicePixelRatio());
}

QSGNode *
QcMapItem::updatePaintNode(QSGNode * old_node, UpdatePaintNodeData *)
{
  // qInfo() << old_node;

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
  if (content and root->childCount() == 0)
    root->appendChildNode(content);

  return root;
}

/**************************************************************************************************/

/*!
  \qmlmethod coordinate QtLocation::Map::to_coordinate(QPointF position, bool clipToViewPort)

  Returns the coordinate which corresponds to the \a position relative to the map item.

  If \a cliptoViewPort is \c true, or not supplied then returns an invalid coordinate if
  \a position is not within the current viewport.
*/
QcWgsCoordinate
QcMapItem::to_coordinate(const QcVectorDouble & position_px, bool clip_to_viewport) const
{
  return m_viewport->to_coordinate(position_px, clip_to_viewport);
}

QGeoCoordinate
QcMapItem::to_coordinate_qt(const QVector2D & position_px, bool clip_to_viewport) const
{
  QcVectorDouble _position_px(position_px.x(), position_px.y());
  return to_coordinate(_position_px, clip_to_viewport).to_qt();
}

/*!
    \qmlmethod point QtLocation::Map::from_coordinate(coordinate coordinate, bool clipToViewPort)

    Returns the position relative to the map item which corresponds to the \a coordinate.

    If \a cliptoViewPort is \c true, or not supplied then returns an invalid QPointF if
    \a coordinate is not within the current viewport.
*/
QcVectorDouble
QcMapItem::from_coordinate(const QcWgsCoordinate & coordinate, bool clip_to_viewport) const
{
  return m_viewport->from_coordinate(coordinate, clip_to_viewport);
}

QVector2D
QcMapItem::from_coordinate_qt(const QGeoCoordinate & coordinate, bool clip_to_viewport) const
{
  QcWgsCoordinate _coordinate(coordinate);
  QcVectorDouble position_px = from_coordinate(_coordinate, clip_to_viewport);
  return QVector2D(position_px.x(), position_px.y());
}

/**************************************************************************************************/

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

QVariantList
QcMapItem::plugins() const
{
  QVariantList plugins;
  for (auto * plugin : m_plugin_manager.plugins())
    plugins << QVariant::fromValue(QcWmtsPluginData(plugin->name(), plugin->title()));
  return plugins;
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
  if (layer_data->status()) {
    m_map_view->set_opacity(plugin_layer, opacity);
    update();
  }
}

/**************************************************************************************************/

void
QcMapItem::set_bearing(double bearing)
{
  m_viewport->set_bearing(bearing);
}

double
QcMapItem::bearing() const
{
  return 0.; // Fixme:
}

void
QcMapItem::set_gps_horizontal_precision(double horizontal_precision)
{
  Q_UNUSED(horizontal_precision)
}

double
QcMapItem::gps_horizontal_precision() const
{
  return 0.; // Fixme:
}

QcMapScale
QcMapItem::make_scale(unsigned int max_length_px) const
{
  return m_viewport->make_scale(max_length_px);
}

/**************************************************************************************************/

void
QcMapItem::prefetch_data()
{
  // qInfo();
}

/**************************************************************************************************/

// #include "declarative_map_item.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
