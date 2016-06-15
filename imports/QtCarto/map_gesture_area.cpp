/****************************************************************************
 **
 ** Copyright (C) 2015 The Qt Company Ltd.
 ** Contact: http://www.qt.io/licensing/
 **
 ** This file is part of the QtLocation module of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL3$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and The Qt Company. For licensing terms
 ** and conditions see http://www.qt.io/terms-conditions. For further
 ** information use the contact form at http://www.qt.io/contact-us.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 3 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPLv3 included in the
 ** packaging of this file. Please review the following information to
 ** ensure the GNU Lesser General Public License version 3 requirements
 ** will be met: https://www.gnu.org/licenses/lgpl.html.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 2.0 or later as published by the Free
 ** Software Foundation and appearing in the file LICENSE.GPL included in
 ** the packaging of this file. Please review the following information to
 ** ensure the GNU General Public License version 2.0 requirements will be
 ** met: http://www.gnu.org/licenses/gpl-2.0.html.
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

/**************************************************************************************************/

#include "map_gesture_area.h"

#include "declarative_map_item.h"

#include <cmath>

#include <QDebug>
#include <QPropertyAnimation>
#include <QtGui/QGuiApplication>
#include <QtGui/QStyleHints>
#include <QtGui/QWheelEvent>
#include <QtQuick/QQuickWindow>

/**************************************************************************************************/

/*!
  \qmltype MapPinchEvent
  \instantiates QcMapPinchEvent
  \inqmlmodule QtLocation

  \brief MapPinchEvent type provides basic information about pinch event.

  MapPinchEvent type provides basic information about pinch event. They are
  present in handlers of MapPinch (for example pinchStarted/pinchUpdated). Events are only
  guaranteed to be valid for the duration of the handler.

  Except for the \l accepted property, all properties are read-only.

  \section2 Example Usage

  The following example enables the pinch gesture on a map and reacts to the
  finished event.

  \code
  Map {
  id: map
  gesture.enabled: true
  gesture.onPinchFinished:{
  var coordinate1 = map.toCoordinate(gesture.point1)
  var coordinate2 = map.toCoordinate(gesture.point2)
  console.log("Pinch started at:")
  console.log("        Points (" + gesture.point1.x + ", " + gesture.point1.y + ") - (" + gesture.point2.x + ", " + gesture.point2.y + ")")
  console.log("   Coordinates (" + coordinate1.latitude + ", " + coordinate1.longitude + ") - (" + coordinate2.latitude + ", " + coordinate2.longitude + ")")
  }
  }
  \endcode

  \ingroup qml-QtLocation5-maps
  \since Qt Location 5.0
*/

/*!
  \qmlproperty QPoint QtLocation::MapPinchEvent::center

  This read-only property holds the current center point.
*/

/*!
  \qmlproperty real QtLocation::MapPinchEvent::angle

  This read-only property holds the current angle between the two points in
  the range -180 to 180. Positive values for the angles mean counter-clockwise
  while negative values mean the clockwise direction. Zero degrees is at the
  3 o'clock position.
*/

/*!
  \qmlproperty QPoint QtLocation::MapPinchEvent::point1
  \qmlproperty QPoint QtLocation::MapPinchEvent::point2

  These read-only properties hold the actual touch points generating the pinch.
  The points are not in any particular order.
*/

/*!
  \qmlproperty int QtLocation::MapPinchEvent::pointCount

  This read-only property holds the number of points currently touched.
  The MapPinch will not react until two touch points have initiated a gesture,
  but will remain active until all touch points have been released.
*/

/*!
  \qmlproperty bool QtLocation::MapPinchEvent::accepted

  Setting this property to false in the \c MapPinch::onPinchStarted handler
  will result in no further pinch events being generated, and the gesture
  ignored.
*/

/*!
  \qmltype MapGestureArea
  \instantiates QcMapGestureArea

  \inqmlmodule QtLocation

  \brief The MapGestureArea type provides Map gesture interaction.

  MapGestureArea objects are used as part of a Map, to provide for panning,
  flicking and pinch-to-zoom gesture used on touch displays.

  A MapGestureArea is automatically created with a new Map and available with
  the \l{Map::gesture}{gesture} property. This is the only way
  to create a MapGestureArea, and once created this way cannot be destroyed
  without its parent Map.

  The two most commonly used properties of the MapGestureArea are the \l enabled
  and \l acceptedGestures properties. Both of these must be set before a
  MapGestureArea will have any effect upon interaction with the Map.
  The \l flickDeceleration property controls how quickly the map pan slows after contact
  is released while panning the map.

  \section2 Performance

  The MapGestureArea, when enabled, must process all incoming touch events in
  order to track the shape and size of the "pinch". The overhead added on
  touch events can be considered constant time.

  \section2 Example Usage

  The following example enables the pinch and pan gestures on the map, but not flicking. So the
  map scrolling will halt immediately on releasing the mouse button / touch.

  \code
  Map {
  gesture.enabled: true
  gesture.acceptedGestures: MapGestureArea.PinchGesture | MapGestureArea.PanGesture
  }
  \endcode

  \ingroup qml-QtLocation5-maps
  \since Qt Location 5.0
*/

/*!
  \qmlproperty bool QtLocation::MapGestureArea::enabled

  This property holds whether the gestures are enabled.
*/

/*!
  \qmlproperty bool QtLocation::MapGestureArea::pinchActive

  This read-only property holds whether pinch gesture is active.
*/

/*!
  \qmlproperty bool QtLocation::MapGestureArea::panActive

  This read-only property holds whether pan gesture is active.

  \note Change notifications for this property were introduced in Qt 5.5.
*/

/*!
  \qmlproperty real QtLocation::MapGestureArea::maximumZoomLevelChange

  This property holds the maximum zoom level change per pinch, essentially
  meant to be used for setting the zoom sensitivity.

  It is an indicative measure calculated from the dimensions of the
  map area, roughly corresponding how much zoom level could change with
  maximum pinch zoom. Default value is 4.0, maximum value is 10.0
*/

/*!
  \qmlproperty real MapGestureArea::flickDeceleration

  This property holds the rate at which a flick will decelerate.

  The default value is 2500.
*/

/*!
  \qmlsignal QtLocation::MapGestureArea::pinchStarted(PinchEvent event)

  This signal is emitted when a pinch gesture is started.

  The corresponding handler is \c onPinchStarted.

  \sa pinchUpdated, pinchFinished
*/

/*!
  \qmlsignal QtLocation::MapGestureArea::pinchUpdated(PinchEvent event)

  This signal is emitted as the user's fingers move across the map,
  after the \l pinchStarted signal is emitted.

  The corresponding handler is \c onPinchUpdated.

  \sa pinchStarted, pinchFinished
*/

/*!
  \qmlsignal QtLocation::MapGestureArea::pinchFinished(PinchEvent event)

  This signal is emitted at the end of a pinch gesture.

  The corresponding handler is \c onPinchFinished.

  \sa pinchStarted, pinchUpdated
*/

/*!
  \qmlsignal QtLocation::MapGestureArea::panStarted()

  This signal is emitted when the map begins to move due to user
  interaction. Typically this means that the user is dragging a finger -
  or a mouse with one of more mouse buttons pressed - on the map.

  The corresponding handler is \c onPanStarted.
*/

/*!
  \qmlsignal QtLocation::MapGestureArea::panFinished()

  This signal is emitted when the map stops moving due to user
  interaction.  If a flick was generated, this signal is
  emitted before flick starts. If a flick was not
  generated, this signal is emitted when the
  user stops dragging - that is a mouse or touch release.

  The corresponding handler is \c onPanFinished.

*/

/*!
  \qmlsignal QtLocation::MapGestureArea::flickStarted()

  This signal is emitted when the map is flicked.  A flick
  starts from the point where the mouse or touch was released,
  while still in motion.

  The corresponding handler is \c onFlichStarted.
*/

/*!
  \qmlsignal QtLocation::MapGestureArea::flickFinished()

  This signal is emitted when the map stops moving due to a flick.

  The corresponding handler is \c onFlickFinished.
*/

/**************************************************************************************************/

/*
 * Use cases:
 *
 * Pan / flick:
 *  - press
 *  - move : move map if delta is significant
 *  - release : flick map if velocity is significant
 *
 */

/**************************************************************************************************/

// QT_BEGIN_NAMESPACE

constexpr int QML_MAP_FLICK_DEFAULT_MAX_VELOCITY = 2500; // [px/s]
constexpr int QML_MAP_FLICK_DEFAULT_DECELERATION = 2500;
constexpr qreal QML_MAP_FLICK_MINIMUM_DECELERATION = 500.;
constexpr qreal QML_MAP_FLICK_MAXIMUM_DECELERATION = 10000.;

constexpr int MINIMUM_PRESS_AND_HOLD_TIME = 1000; // [ms]
constexpr qreal MAXIMUM_PRESS_AND_HOLD_JITTER = 30.;

constexpr qreal MAXIMUM_DOUBLE_PRESS_TIME = 300.; // [ms]

constexpr int QML_MAP_FLICK_VELOCITY_SAMPLE_PERIOD = 50; // [s]

// FlickThreshold determines how far the "mouse" must have moved before we perform a flick.
constexpr int FLICK_THRESHOLD = 20; // [px]

// Really slow flicks can be annoying.
constexpr qreal MINIMUM_FLICK_VELOCITY = 75.0; // [px/s]

/**************************************************************************************************/

QMouseEvent *
copy_mouse_event(const QMouseEvent * event)
{
  return new QMouseEvent(QEvent::MouseButtonPress,
                         event->pos(),
                         event->button(), event->buttons(),
                         event->modifiers());
}

QTouchEvent::TouchPoint *
create_touch_point_from_mouse_event(const QMouseEvent * event, Qt::TouchPointState state)
{
  // qInfo();

  // this is only partially filled. But since it is only partially used it works
  // more robust would be to store a list of QPointFs rather than TouchPoints

  QTouchEvent::TouchPoint * new_point = new QTouchEvent::TouchPoint();
  new_point->setPos(event->localPos()); // relative to the item
  new_point->setScenePos(event->windowPos()); // relative to the window
  new_point->setScreenPos(event->screenPos()); // relative to the screen
  new_point->setState(state);
  new_point->setId(0);
  return new_point;
}

/**************************************************************************************************/

QcMapGestureArea::QcMapGestureArea(QcMapItem * map)
  : QQuickItem(map),
    m_map(map),
    m_enabled(true),
    m_accepted_gestures(PinchGesture | PanGesture | FlickGesture),
    m_prevent_stealing(false),
    m_pan_enabled(true)
{
  // qInfo();

  m_flick.m_enabled = true;
  m_flick.m_max_velocity = QML_MAP_FLICK_DEFAULT_MAX_VELOCITY;
  m_flick.m_deceleration = QML_MAP_FLICK_DEFAULT_DECELERATION;

  m_flick.m_animation = new QcGeoCoordinateAnimation(this);
  m_flick.m_animation->setTargetObject(m_map);
  m_flick.m_animation->setProperty(QStringLiteral("center"));
  m_flick.m_animation->setEasing(QEasingCurve(QEasingCurve::OutQuad));
  connect(m_flick.m_animation, &QQuickAbstractAnimation::stopped,
          this, &QcMapGestureArea::handle_flick_animation_stopped);

  m_touch_point_state = TouchPoints0;
  m_pinch_state = PinchInactive;
  m_flick_state = FlickInactive;

  m_press_timer.setSingleShot(true);
  m_press_timer.setInterval(MINIMUM_PRESS_AND_HOLD_TIME);
  connect(&m_press_timer, &QTimer::timeout,
          this, &QcMapGestureArea::handle_press_timer_timeout);

  m_press_time.invalidate();
  m_double_press_time.invalidate();
}

QcMapGestureArea::~QcMapGestureArea()
{}

/*!
  \qmlproperty bool QtQuick::MapGestureArea::preventStealing
  This property holds whether the mouse events may be stolen from this
  MapGestureArea.

  If a Map is placed within an item that filters child mouse
  and touch events, such as Flickable, the mouse and touch events
  may be stolen from the MapGestureArea if a gesture is recognized
  by the parent item, e.g. a flick gesture.  If preventStealing is
  set to true, no item will steal the mouse and touch events.

  Note that setting preventStealing to true once an item has started
  stealing events will have no effect until the next press event.

  By default this property is false.
*/

void QcMapGestureArea::set_prevent_stealing(bool prevent)
{
  // qInfo();

  if (prevent != m_prevent_stealing) {
    m_prevent_stealing = prevent;
    m_map->setKeepMouseGrab(m_prevent_stealing and m_enabled);
    m_map->setKeepTouchGrab(m_prevent_stealing and m_enabled);
    emit prevent_stealingChanged();
  }
}

/*!
  \qmlproperty enumeration QtLocation::MapGestureArea::acceptedGestures

  This property holds the gestures that will be active. By default
  the zoom, pan and flick gestures are enabled.

  \list
  \li MapGestureArea.NoGesture - Don't support any additional gestures (value: 0x0000).
  \li MapGestureArea.PinchGesture - Support the map pinch gesture (value: 0x0001).
  \li MapGestureArea.PanGesture  - Support the map pan gesture (value: 0x0002).
  \li MapGestureArea.FlickGesture  - Support the map flick gesture (value: 0x0004).
  \endlist
*/

void
QcMapGestureArea::set_accepted_gestures(Accepted_gestures accepted_gestures)
{
  // qInfo();

  if (accepted_gestures != m_accepted_gestures) {
    m_accepted_gestures = accepted_gestures;

    set_flick_enabled(accepted_gestures & FlickGesture);
    set_pan_enabled(accepted_gestures & PanGesture);
    set_pinch_enabled(accepted_gestures & PinchGesture);

    emit accepted_gesturesChanged();
  }
}

bool
QcMapGestureArea::is_active() const
{
  return is_pan_active() or is_pinch_active();
}

void
QcMapGestureArea::set_enabled(bool enabled)
{
  // qInfo();

  if (enabled != m_enabled) {
    m_enabled = enabled;

    if (enabled) {
      set_flick_enabled(m_accepted_gestures & FlickGesture);
      set_pan_enabled(m_accepted_gestures & PanGesture);
      set_pinch_enabled(m_accepted_gestures & PinchGesture);
    } else {
      set_flick_enabled(false);
      set_pan_enabled(false);
      set_pinch_enabled(false);
    }

    emit enabledChanged();
  }
}

bool
QcMapGestureArea::is_pinch_active() const
{
  return m_pinch_state == PinchActive;
}

void
QcMapGestureArea::set_pinch_enabled(bool enabled)
{
  // qInfo();

  if (enabled != m_pinch.m_enabled)
    m_pinch.m_enabled = enabled;
}

bool
QcMapGestureArea::is_pan_active() const
{
  return m_flick_state == PanActive or m_flick_state == FlickActive;
}

void
QcMapGestureArea::set_pan_enabled(bool enabled)
{
  // qInfo();

  if (enabled != m_flick.m_enabled) {
    m_pan_enabled = enabled;
    // unlike the pinch, the pan existing functionality is to stop immediately
    if (!enabled)
      stop_pan();
  }
}

void
QcMapGestureArea::set_flick_enabled(bool enabled)
{
  // qInfo();

  if (enabled != m_flick.m_enabled) {
    m_flick.m_enabled = enabled;
    // unlike the pinch, the flick existing functionality is to stop immediately
    if (!enabled) {
      stop_flick();
    }
  }
}

/*!
  \internal
  Used internally to set the minimum zoom level of the gesture area.
  The caller is responsible to only send values that are valid
  for the map plugin. Negative values are ignored.
*/
void
QcMapGestureArea::set_zoom_level_interval(const QcIntervalInt interval)
{
  // qInfo();

  m_pinch.m_zoom.m_interval = interval;
}

qreal
QcMapGestureArea::maximum_zoom_level_change() const
{
  return m_pinch.m_zoom.maximum_change;
}

void
QcMapGestureArea::set_maximum_zoom_level_change(qreal max_change)
{
  // qInfo();

  // Fixme: !()
  if (max_change == m_pinch.m_zoom.maximum_change or
      max_change < 0.1 or max_change > 10.0)
    return;

  m_pinch.m_zoom.maximum_change = max_change;
  emit maximum_zoom_level_changeChanged();
}

void
QcMapGestureArea::set_flick_deceleration(qreal deceleration)
{
  // qInfo();

  deceleration = qBound(QML_MAP_FLICK_MINIMUM_DECELERATION, deceleration, QML_MAP_FLICK_MAXIMUM_DECELERATION);
  if (deceleration != m_flick.m_deceleration) {
    m_flick.m_deceleration = deceleration;
    emit flick_decelerationChanged();
  }
}

void
QcMapGestureArea::clear_touch_data()
{
  // qInfo();

  // Fixme: vectorize
  m_current_position.set_x(0);
  m_current_position.set_y(0);
  m_start_coordinate.set_latitude(0);
  m_start_coordinate.set_longitude(0);
  m_touch_center_coordinate.set_latitude(0);
  m_touch_center_coordinate.set_longitude(0);
  m_velocity_x = 0;
  m_velocity_y = 0;
}

void
QcMapGestureArea::update_velocity_list(const QcVectorDouble & position)
{
  // qInfo();

  // position = m_current_position

  // Take velocity samples every sufficient period of time, used later to determine the flick
  // duration and speed (when mouse is released).

  qreal elapsed = qreal(m_last_position_for_velocity_time.elapsed()); // [ms]
  if (elapsed >= QML_MAP_FLICK_VELOCITY_SAMPLE_PERIOD) {
    elapsed /= 1000.; // to [s]
    QcVectorDouble velocity = (position - m_last_position_for_velocity) / elapsed; // [px/s]
    m_last_position_for_velocity = position;
    m_last_position_for_velocity_time.restart();
    m_velocity_x = qBound<qreal>(-m_flick.m_max_velocity, velocity.x(), m_flick.m_max_velocity);
    m_velocity_y = qBound<qreal>(-m_flick.m_max_velocity, velocity.y(), m_flick.m_max_velocity);
  }
}

/**************************************************************************************************/

void
QcMapGestureArea::handle_mouse_press_event(QMouseEvent * event)
{
  qInfo() << event;

  m_mouse_point.reset(create_touch_point_from_mouse_event(event, Qt::TouchPointPressed));
  m_mouse_event.reset(copy_mouse_event(event));
  m_press_timer.start();
  m_press_time.start(); // Fixme: start_one_touch_point ?
  if (m_touch_points.isEmpty()) {
    update();
    if (is_double_click())
      m_map->on_double_clicked(event);
  }
  event->accept();
}

void
QcMapGestureArea::handle_mouse_move_event(QMouseEvent * event)
{
  qInfo() << event;

  m_mouse_point.reset(create_touch_point_from_mouse_event(event, Qt::TouchPointMoved));
  if (m_touch_points.isEmpty())
    update();
  event->accept();
}

void
QcMapGestureArea::handle_mouse_release_event(QMouseEvent * event)
{
  qInfo() << event;

  if (!m_mouse_point.isNull()) {
    // this looks super ugly , however is required in case we do not get synthesized MouseReleaseEvent
    // and we reset the point already in handleTouchUngrabEvent
    // Fixme: ???
    m_mouse_point.reset(create_touch_point_from_mouse_event(event, Qt::TouchPointReleased));
    if (m_touch_points.isEmpty()) {
      update();
      // if (is_press_and_hold())
      //   m_map->on_press_and_hold(event);
    }
  }
  // Reset touch point state
  m_touch_point_state = TouchPoints0;
  m_press_time.invalidate();
  event->accept();
}

void
QcMapGestureArea::handle_mouse_ungrab_event()
{
  qInfo();

  if (m_touch_points.isEmpty() and !m_mouse_point.isNull()) {
    m_mouse_point.reset();
    update();
  } else
    m_mouse_point.reset();
}

void
QcMapGestureArea::handle_touch_ungrab_event()
{
  // qInfo();

  m_touch_points.clear();
  // this is needed since in some cases mouse release is not delivered
  // (second touch point brakes mouse synthesized events)
  m_mouse_point.reset();
  update();
}

void
QcMapGestureArea::handle_touch_event(QTouchEvent * event)
{
  // qInfo();

  m_touch_points.clear();
  m_touch_points.append(event->touchPoints());
  if (event->touchPoints().count() >= 2)
    event->accept();
  else
    // Fixme: press_and_hold, double click
    event->ignore();
  update();
}

void
QcMapGestureArea::handle_wheel_event(QWheelEvent * event)
{
  qInfo() << event;

  if (m_map)
    m_map->on_wheel_event(event);
}

/**************************************************************************************************/

// Simplify the gestures by using a state-machine format (easy to move to a future state machine)
void
QcMapGestureArea::update()
{
  qInfo() << "enter" << m_touch_point_state << m_flick_state << m_pinch_state;

  // if (!m_map)
  //   return;

  // First state machine is for the number of touch points

  // combine touch with mouse event
  m_all_points.clear();
  m_all_points << m_touch_points;
  // any touch points but mouse point
  if (m_all_points.isEmpty() and !m_mouse_point.isNull())
    m_all_points << *m_mouse_point.data();

  touch_point_state_machine();

  // Parallel state machine for pinch
  if (is_pinch_active() or (m_enabled and m_pinch.m_enabled and (m_accepted_gestures & (PinchGesture))))
    pinch_state_machine();

  // Parallel state machine for pan (since you can pan at the same time as pinching)
  // The stopPan function ensures that pan stops immediately when disabled,
  // but the line below allows pan continue its current gesture
  // if you disable the whole gesture (enabled_ flag),
  // this keeps the enabled_ consistent with the pinch
  if (is_pan_active() or (m_enabled and m_flick.m_enabled and (m_accepted_gestures & (PanGesture | FlickGesture))))
    pan_state_machine();

  qInfo() << "leave" << m_touch_point_state << m_flick_state << m_pinch_state;
}

/**************************************************************************************************/

void
QcMapGestureArea::handle_press_timer_timeout()
{
  qInfo();
  if (is_press_and_hold())
    m_map->on_press_and_hold(m_mouse_event.data());
  m_mouse_point.reset();
}

bool
QcMapGestureArea::is_press_and_hold()
{
  qInfo();

  // if (!m_map)
  //   return false;

  if (!m_all_points.size())
    return false;

  if (is_pan_active() or is_pinch_active())
    return false;

  // if (m_press_time.isValid() and m_press_time.elapsed() > MINIMUM_PRESS_AND_HOLD_TIME) {
  QcVectorDouble p1 = first_point().pos();
  QcVectorDouble delta_from_press = p1 - m_start_position1;
  return (qAbs(delta_from_press.x()) <= MAXIMUM_PRESS_AND_HOLD_JITTER or
          qAbs(delta_from_press.y()) <= MAXIMUM_PRESS_AND_HOLD_JITTER);
  // } else
  //   return false;
}

bool
QcMapGestureArea::is_double_click()
{
  // if (!m_map)
  //   return false;

  if (is_pan_active() or is_pinch_active())
    return false;

  bool status = m_double_press_time.isValid() and m_double_press_time.elapsed() <= MAXIMUM_DOUBLE_PRESS_TIME;
  m_double_press_time.restart();
  return status;
}

/**************************************************************************************************/

void
QcMapGestureArea::touch_point_state_machine()
{
  qInfo() << "enter" << m_touch_point_state;

  int number_of_points = m_all_points.count();

  // Transitions:
  switch (m_touch_point_state) {
  case TouchPoints0:
    if (number_of_points == 1) {
      clear_touch_data();
      start_one_touch_point();
      m_touch_point_state = TouchPoints1;
    } else if (number_of_points >= 2) {
      clear_touch_data();
      start_two_touch_points();
      m_touch_point_state = TouchPoints2;
    }
    break;

  case TouchPoints1:
    if (number_of_points == 0) {
      m_touch_point_state = TouchPoints0;
    } else if (number_of_points == 2) {
      m_touch_center_coordinate = m_map->to_coordinate(m_current_position, false);
      start_two_touch_points();
      m_touch_point_state = TouchPoints2;
    }
    break;

  case TouchPoints2:
    if (number_of_points == 0) {
      m_touch_point_state = TouchPoints0;
    } else if (number_of_points == 1) {
      m_touch_center_coordinate = m_map->to_coordinate(m_current_position, false);
      start_one_touch_point();
      m_touch_point_state = TouchPoints1;
    }
    break;
  };

  // Update
  switch (m_touch_point_state) {
  case TouchPoints0:
    break; // do nothing if no touch points down
  case TouchPoints1:
    update_one_touch_point();
    break;
  case TouchPoints2:
    update_two_touch_points();
    break;
  }

  qInfo() << "leave" << m_touch_point_state;
}

void
QcMapGestureArea::start_one_touch_point()
{
  qInfo();

  m_start_position1 = first_point().pos();
  m_last_position_for_velocity = m_start_position1;
  m_last_position_for_velocity_time.start();
  QcWgsCoordinate start_coordinate = m_map->to_coordinate(m_start_position1, false);
  // Ensures a smooth transition for panning (m_start_coordinate and m_touch_center_coordinate are cleared in clear_touch_data)
  // Fixme: ???
  m_start_coordinate.set_longitude(start_coordinate.longitude() + m_start_coordinate.longitude() - m_touch_center_coordinate.longitude());
  m_start_coordinate.set_latitude(start_coordinate.latitude() + m_start_coordinate.latitude() - m_touch_center_coordinate.latitude());
}

void
QcMapGestureArea::update_one_touch_point()
{
  qInfo();

  m_current_position = first_point().pos();
  update_velocity_list(m_current_position);
}

void
QcMapGestureArea::start_two_touch_points()
{
  // qInfo();

  m_start_position1 = first_point().pos();
  m_start_position2 = second_point().pos();
  QcVectorDouble start_position = middle(m_start_position1, m_start_position2);
  // Fixme: duplicated code, excepted middle
  m_last_position_for_velocity = start_position;
  m_last_position_for_velocity_time.start();
  QcWgsCoordinate start_coordinate = m_map->to_coordinate(start_position, false);
  m_start_coordinate.set_longitude(start_coordinate.longitude() + m_start_coordinate.longitude() - m_touch_center_coordinate.longitude());
  m_start_coordinate.set_latitude(start_coordinate.latitude() + m_start_coordinate.latitude() - m_touch_center_coordinate.latitude());
}

void
QcMapGestureArea::update_two_touch_points()
{
  // qInfo();

  QcVectorDouble p1 = first_point().pos();
  QcVectorDouble p2 = second_point().pos();
  QcVectorDouble delta = p2 - p1;
  m_distance_between_touch_points = delta.magnitude();
  m_current_position = middle(p1, p2);
  update_velocity_list(m_current_position);

  m_two_touch_angle = delta.orientation();
  // Fixme: required ?
  if (m_two_touch_angle > 180)
    m_two_touch_angle -= 360;
}

/**************************************************************************************************/

void
QcMapGestureArea::pinch_state_machine()
{
  // qInfo();

  int number_of_points = m_all_points.count();

  PinchState last_state = m_pinch_state;
  // Transitions:
  switch (m_pinch_state) {
  case PinchInactive:
    if (number_of_points >= 2) {
      if (can_start_pinch()) {
        m_map->setKeepMouseGrab(true);
        m_map->setKeepTouchGrab(true);
        start_pinch();
        m_pinch_state = PinchActive;
      } else {
        m_pinch_state = PinchInactiveTwoPoints;
      }
    }
    break;

  case PinchInactiveTwoPoints:
    if (number_of_points <= 1) {
      m_pinch_state = PinchInactive;
    } else {
      if (can_start_pinch()) {
        m_map->setKeepMouseGrab(true);
        m_map->setKeepTouchGrab(true);
        start_pinch();
        m_pinch_state = PinchActive;
      }
    }
    break;

  case PinchActive:
    if (number_of_points <= 1) {
      m_pinch_state = PinchInactive;
      m_map->setKeepMouseGrab(m_prevent_stealing);
      m_map->setKeepTouchGrab(m_prevent_stealing);
      end_pinch();
    }
    break;
  }

  // This line implements an exclusive state machine, where the
  // transitions and updates don't happen on the same frame
  if (m_pinch_state != last_state) {
    emit pinch_activeChanged();
    return;
  }

  // Update
  switch (m_pinch_state) {
  case PinchInactive:
  case PinchInactiveTwoPoints:
    break; // do nothing

  case PinchActive:
    update_pinch();
    break;
  }
}

bool
QcMapGestureArea::can_start_pinch()
{
  // qInfo();

  int number_of_points = m_all_points.count();
  const int start_drag_distance = qApp->styleHints()->startDragDistance();

  if (number_of_points >= 2) {
    QcVectorDouble p1 = first_point().pos();
    QcVectorDouble p2 = second_point().pos();
    if (qAbs(p1.x() - m_start_position1.x()) > start_drag_distance or
        qAbs(p1.y() - m_start_position1.y()) > start_drag_distance or
        qAbs(p2.x() - m_start_position2.x()) > start_drag_distance or
        qAbs(p2.y() - m_start_position2.y()) > start_drag_distance) {
      m_pinch.m_event.set_angle(m_two_touch_angle);
      m_pinch.m_event.set_center(m_current_position);
      m_pinch.m_event.set_number_of_points(number_of_points);
      m_pinch.m_event.set_point1(p1);
      m_pinch.m_event.set_point2(p2);
      m_pinch.m_event.set_accepted(true);
      emit pinch_started(&m_pinch.m_event);
      return m_pinch.m_event.accepted();
    }
  }

  return false;
}

void
QcMapGestureArea::start_pinch()
{
  // qInfo();

  m_pinch.m_last_angle = m_two_touch_angle;
  m_pinch.m_last_point1 = first_point().pos();
  m_pinch.m_last_point2 = second_point().pos();
  m_pinch.m_start_distance = m_distance_between_touch_points;
  m_pinch.m_zoom.m_previous = m_map->zoom_level();
  m_pinch.m_zoom.m_start = m_map->zoom_level();
}

void
QcMapGestureArea::update_pinch()
{
  // qInfo();

  // Calculate the new zoom level if we have distance (>= 2 touchpoints), otherwise stick with old.
  qreal new_zoom_level = m_pinch.m_zoom.m_previous;
  if (m_distance_between_touch_points) {
    new_zoom_level =
      // How much further/closer the current touchpoints are (in pixels) compared to pinch start
      ((m_distance_between_touch_points - m_pinch.m_start_distance)  *
       // How much one pixel corresponds in units of zoomlevel (and multiply by above delta)
       (m_pinch.m_zoom.maximum_change / ((width() + height()) / 2))) +
      // Add to starting zoom level. Sign of (dist-pinchstartdist) takes care of zoom in / out
      m_pinch.m_zoom.m_start;
  }

  // qreal delta_angle = m_pinch.m_last_angle - m_two_touch_angle;
  // if (delta_angle > 180)
  //   delta_angle -= 360;
  // else if (delta_angle < -180)
  //   delta_angle += 360;

  m_pinch.m_last_angle = m_two_touch_angle;
  m_pinch.m_last_point1 = first_point().pos();
  m_pinch.m_last_point2 = second_point().pos();

  m_pinch.m_event.set_angle(m_two_touch_angle);
  m_pinch.m_event.set_center(m_current_position);
  m_pinch.m_event.set_number_of_points(m_all_points.count());
  m_pinch.m_event.set_point1(m_pinch.m_last_point1);
  m_pinch.m_event.set_point2(m_pinch.m_last_point2);
  m_pinch.m_event.set_accepted(true);

  emit pinch_updated(&m_pinch.m_event);

  if (m_accepted_gestures & PinchGesture) {
    // Take maximum and minimumzoomlevel into account
    qreal per_pinch_minimum_zoom_level = qMax(m_pinch.m_zoom.m_start - m_pinch.m_zoom.maximum_change,
                                              static_cast<qreal>(m_pinch.m_zoom.m_interval.inf()));
    qreal per_pinch_maximum_zoom_level = qMin(m_pinch.m_zoom.m_start + m_pinch.m_zoom.maximum_change,
                                              static_cast<qreal>(m_pinch.m_zoom.m_interval.sup()));
    new_zoom_level = qMin(qMax(per_pinch_minimum_zoom_level, new_zoom_level), per_pinch_maximum_zoom_level);
    m_map->set_zoom_level(new_zoom_level);
    m_pinch.m_zoom.m_previous = new_zoom_level;
  }
}

void
QcMapGestureArea::end_pinch()
{
  // qInfo();

  QcVectorDouble p1 = m_pinch.m_last_point1;
  QcVectorDouble p2 = m_pinch.m_last_point2;

  m_pinch.m_event.set_angle(m_pinch.m_last_angle);
  m_pinch.m_event.set_center(middle(p1, p2));
  m_pinch.m_event.set_number_of_points(0);
  m_pinch.m_event.set_point1(p1);
  m_pinch.m_event.set_point2(p2);
  m_pinch.m_event.set_accepted(true);
  emit pinch_finished(&m_pinch.m_event);

  m_pinch.m_start_distance = 0;
}

/**************************************************************************************************/

void
QcMapGestureArea::pan_state_machine()
{
  qInfo();

  int number_of_points = m_all_points.count();
  FlickState last_state = m_flick_state;

  // Transitions
  switch (m_flick_state) {
  case FlickInactive:
    if (can_start_pan()) {
      qInfo() << "can_start_pan";
      // Update start_coordinate to ensure smooth start for panning when going over startDragDistance
      // Mouse pointer slides on the map until it goes over startDragDistance
      m_start_coordinate = m_map->to_coordinate(m_current_position, false);
      m_map->setKeepMouseGrab(true);
      m_flick_state = PanActive;
    }
    break;

  case PanActive:
    if (number_of_points == 0) {
      if (!try_start_flick()) {
          m_flick_state = FlickInactive;
          // mark as inactive for use by camera
          if (m_pinch_state == PinchInactive) {
            m_map->setKeepMouseGrab(m_prevent_stealing);
            m_map->prefetch_data();
          }
          emit pan_finished();
        } else {
        m_flick_state = FlickActive;
        emit pan_finished();
        emit flick_started();
      }
    }
    break;

  case FlickActive:
    if (number_of_points > 0) { // retouched before movement ended
      stop_flick();
      m_map->setKeepMouseGrab(true);
      m_flick_state = PanActive;
    }
    break;
  }

  if (m_flick_state != last_state)
    emit pan_activeChanged();

  // Update
  switch (m_flick_state) {
  case FlickInactive: // do nothing
    break;

  case PanActive:
    update_pan();
    // this ensures 'panStarted' occurs after the pan has actually started
    if (last_state != PanActive)
      emit pan_started();
    break;

  case FlickActive:
    break;
  }
}

bool
QcMapGestureArea::can_start_pan()
{
  qInfo() << first_point().pos() << m_start_position1;

  if (m_all_points.count() == 0 or (m_accepted_gestures & PanGesture) == 0) // Fixme: to func ?
    return false;

  // Check if thresholds for normal panning are met.
  // (normal panning vs flicking: flicking will start from mouse release event).
  const int start_drag_distance = qApp->styleHints()->startDragDistance() * 2;
  QcVectorDouble p1 = first_point().pos();
  QcVectorDouble delta_from_press = p1 - m_start_position1;
  return (qAbs(delta_from_press.x()) >= start_drag_distance or
          qAbs(delta_from_press.y()) >= start_drag_distance);
}

void
QcMapGestureArea::update_pan()
{
  qInfo();

  // Not used by animation/flick
  // Map follows the mouse pointer: move the map center according to delta px
  // Fixme: delta px -> delta projected coordinate -> new center

  QcVectorDouble start_point = m_map->from_coordinate(m_start_coordinate, false);
  // Fixme: start_coordinate is no longer in the viewport
  if (isnan(start_point.x())) {
    qWarning() << "Screen coordinate are nan";
    return;
  }
  QcVectorDouble delta = m_current_position - start_point;
  QcVectorDouble map_center_px = QcVectorDouble(m_map->width(), m_map->height()) * .5;
  QcVectorDouble map_center_point = map_center_px - delta;
  QcWgsCoordinate new_center = m_map->to_coordinate(map_center_point, false);
  m_map->set_center(new_center);
}

bool
QcMapGestureArea::try_start_flick()
{
  qInfo();

  if ((m_accepted_gestures & FlickGesture) == 0)
    return false;

  // If we drag then pause before release we should not cause a flick.
  qreal velocity_x = 0.0;
  qreal velocity_y = 0.0;
  if (m_last_position_for_velocity_time.elapsed() < QML_MAP_FLICK_VELOCITY_SAMPLE_PERIOD) {
    velocity_x = m_velocity_x;
    velocity_y = m_velocity_y;
  }

  int flick_time_x = 0;
  int flick_pixels_x = 0;
  if (qAbs(velocity_x) > MINIMUM_FLICK_VELOCITY and qAbs(m_current_position.x() - m_start_position1.x()) > FLICK_THRESHOLD) {
    // calculate X flick animation values
    qreal acceleration = m_flick.m_deceleration;
    if ((velocity_x > .0f) == (m_flick.m_deceleration > .0f))
      acceleration = -acceleration;
    flick_time_x = static_cast<int>(-1000 * velocity_x / acceleration);
    flick_pixels_x = (flick_time_x * velocity_x) / (1000. * 2);
  }

  // Same code for y axis
  int flick_time_y = 0;
  int flick_pixels_y = 0;
  if (qAbs(velocity_y) > MINIMUM_FLICK_VELOCITY and qAbs(m_current_position.y() - m_start_position1.y()) > FLICK_THRESHOLD) {
    // calculate Y flick animation values
    qreal acceleration = m_flick.m_deceleration;
    if ((velocity_y > .0f) == (m_flick.m_deceleration > .0f))
      acceleration = -acceleration;
    flick_time_y = static_cast<int>(-1000 * velocity_y / acceleration);
    flick_pixels_y = (flick_time_y * velocity_y) / (1000. * 2);
  }

  int flick_time = qMax(flick_time_y, flick_time_x);
  if (flick_time > 0) {
    start_flick(flick_pixels_x, flick_pixels_y, flick_time);
    return true;
  } else
    return false;
}

void
QcMapGestureArea::start_flick(int dx, int dy, int time_ms)
{
  qInfo();

  if (!m_flick.m_animation)
    return;
  if (time_ms < 0)
    return;

  QcWgsCoordinate animation_start_coordinate = m_map->center();

  // Fixme: could be placed before ?
  if (m_flick.m_animation->isRunning())
    m_flick.m_animation->stop();

  // Fixme: better calculation to roll the globe ?
  double zoom = pow(2.0, m_map->zoom_level());
  double longitude = animation_start_coordinate.longitude() - (dx / zoom);
  double latitude = animation_start_coordinate.latitude() + (dy / zoom);

  // Fixme: must be done according to projection (and viewport ?)
  // keep animation in correct bounds
  qInfo() << longitude << latitude;
  if (latitude > 85.05113)
    latitude = 85.05113;
  else if (latitude < -85.05113)
    latitude = -85.05113;
  if (longitude > 180)
    longitude = longitude - 360;
  else if (longitude < -180)
    longitude = longitude + 360;

  QcWgsCoordinate animation_end_coordinate(longitude, latitude);
  // qInfo() << animation_start_coordinate << animation_end_coordinate << dx;

  m_flick.m_animation->setFrom(animation_start_coordinate);
  m_flick.m_animation->setTo(animation_end_coordinate);
  m_flick.m_animation->setDirection(dx > 0 ? QcGeoCoordinateAnimation::East : QcGeoCoordinateAnimation::West);
  m_flick.m_animation->setDuration(time_ms);
  m_flick.m_animation->start();
}

// Called from set_pan_enabled
void
QcMapGestureArea::stop_pan()
{
  qInfo();

  if (m_flick_state == FlickActive)
    stop_flick();
  else if (m_flick_state == PanActive) {
    m_velocity_x = 0;
    m_velocity_y = 0;
    m_flick_state = FlickInactive;
    m_map->setKeepMouseGrab(m_prevent_stealing);
    emit pan_finished();
    emit pan_activeChanged();
    m_map->prefetch_data();
  }
}

void
QcMapGestureArea::stop_flick()
{
  qInfo();

  if (!m_flick.m_animation)
    return;

  m_velocity_x = 0;
  m_velocity_y = 0;
  if (m_flick.m_animation->isRunning())
    m_flick.m_animation->stop();
  else
    handle_flick_animation_stopped();
}

// Slot
void
QcMapGestureArea::handle_flick_animation_stopped()
{
  qInfo();

  m_map->setKeepMouseGrab(m_prevent_stealing);
  if (m_flick_state == FlickActive) {
    m_flick_state = FlickInactive;
    emit flick_finished();
    m_map->prefetch_data();
  }
}

/**************************************************************************************************/

QT_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
