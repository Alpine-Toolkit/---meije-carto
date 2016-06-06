// -*- mode: c++ -*-

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

#ifndef MAP_GESTURE_AREA_H
#define MAP_GESTURE_AREA_H

/**************************************************************************************************/

#include "geo_coordinate_animation.h"
#include "coordinate/mercator.h"
#include "coordinate/wgs84.h"
#include "geometry/vector.h"

#include <QDebug> // Fixme: QtDebug ???
#include <QElapsedTimer>
#include <QTouchEvent>
#include <QtQuick/QQuickItem>

/**************************************************************************************************/

// QT_BEGIN_NAMESPACE

class QcMapItem;

/**************************************************************************************************/

class QcMapPinchEvent : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QcVectorDouble center READ center)
  Q_PROPERTY(qreal angle READ angle)
  Q_PROPERTY(QcVectorDouble point1 READ point1)
  Q_PROPERTY(QcVectorDouble point2 READ point2)
  Q_PROPERTY(int number_of_points READ number_of_points)
  Q_PROPERTY(bool accepted READ accepted WRITE set_accepted)

public:
  QcMapPinchEvent(const QcVectorDouble & center, qreal angle,
                  const QcVectorDouble & point1, const QcVectorDouble & point2, int number_of_points = 0,
                  bool accepted = true)
    : QObject(),
      m_center(center),
      m_point1(point1),
      m_point2(point2),
      m_angle(angle),
      m_number_of_points(number_of_points),
      m_accepted(accepted)
  {}
  QcMapPinchEvent()
    : QObject(),
      m_angle(0.0),
      m_number_of_points(0),
      m_accepted(true)
  {}

  QcVectorDouble center() const { return m_center; }
  void set_center(const QcVectorDouble & center) { m_center = center; }

  qreal angle() const { return m_angle; }
  void set_angle(qreal angle) { m_angle = angle; }

  QcVectorDouble point1() const { return m_point1; }
  void set_point1(const QcVectorDouble & p) { m_point1 = p; }

  QcVectorDouble point2() const { return m_point2; }
  void set_point2(const QcVectorDouble & p) { m_point2 = p; }

  int number_of_points() const { return m_number_of_points; }
  void set_number_of_points(int number_of_points) { m_number_of_points = number_of_points; }

  bool accepted() const { return m_accepted; }
  void set_accepted(bool status) { m_accepted = status; }

private:
  QcVectorDouble m_center;
  QcVectorDouble m_point1;
  QcVectorDouble m_point2;
  qreal m_angle;
  int m_number_of_points;
  bool m_accepted;
};

/**************************************************************************************************/

struct Zoom
{
  Zoom()
    : m_minimum(-1.0),
      m_maximum(-1.0),
      m_start(0.0),
      m_previous(0.0),
      maximum_change(4.0)
  {}

  qreal m_minimum;
  qreal m_maximum;
  qreal m_start;
  qreal m_previous;
  qreal maximum_change;
};

/**************************************************************************************************/

struct Pinch
{
  Pinch()
    : m_enabled(true),
      m_start_distance(0),
      m_last_angle(.0)
  {}

  bool m_enabled;
  QcMapPinchEvent m_event;
  struct Zoom m_zoom;
  QcVectorDouble m_last_point1;
  QcVectorDouble m_last_point2;
  qreal m_start_distance;
  qreal m_last_angle;
};

/**************************************************************************************************/

struct Pan
{
  bool m_enabled;
  qreal m_max_velocity;
  qreal m_deceleration;
  QcGeoCoordinateAnimation * m_animation; // Fixme
};

/**************************************************************************************************/

class QcMapGestureArea: public QQuickItem
{
  Q_OBJECT

  Q_ENUMS(GeoMapGesture)
  Q_FLAGS(Accepted_gestures)

  Q_PROPERTY(bool enabled READ enabled WRITE set_enabled NOTIFY enabledChanged)
  Q_PROPERTY(bool pinch_active READ is_pinch_active NOTIFY pinch_activeChanged)
  Q_PROPERTY(bool pan_active READ is_pan_active NOTIFY pan_activeChanged)
  Q_PROPERTY(Accepted_gestures accepted_gestures READ accepted_gestures WRITE set_accepted_gestures NOTIFY accepted_gesturesChanged)
  Q_PROPERTY(qreal maximum_zoom_level_change READ maximum_zoom_level_change WRITE set_maximum_zoom_level_change NOTIFY maximum_zoom_level_changeChanged)
  Q_PROPERTY(qreal flick_deceleration READ flick_deceleration WRITE set_flick_deceleration NOTIFY flick_decelerationChanged)
  Q_PROPERTY(bool prevent_stealing READ prevent_stealing WRITE set_prevent_stealing NOTIFY prevent_stealingChanged)

public:
  QcMapGestureArea(QcMapItem * map);
  ~QcMapGestureArea();

  enum GeoMapGesture {
    NoGesture = 0x0000,
    PinchGesture = 0x0001,
    PanGesture = 0x0002,
    FlickGesture = 0x004
  };

  Q_DECLARE_FLAGS(Accepted_gestures, GeoMapGesture)

  Accepted_gestures accepted_gestures() const { return m_accepted_gestures; }
  void set_accepted_gestures(Accepted_gestures accepted_gestures);

  bool is_pinch_active() const;
  bool is_pan_active() const;
  bool is_active() const;

  bool enabled() const { return m_enabled; }
  void set_enabled(bool enabled);

  qreal maximum_zoom_level_change() const;
  void set_maximum_zoom_level_change(qreal max_change);

  qreal flick_deceleration() const { return m_flick.m_deceleration; }
  void set_flick_deceleration(qreal deceleration);

  void set_minimum_zoom_level(qreal min);
  qreal minimum_zoom_level() const;

  void set_maximum_zoom_level(qreal max);
  qreal maximum_zoom_level() const;

  bool prevent_stealing() const { return m_prevent_stealing; }
  void set_prevent_stealing(bool prevent);

  void handle_touch_event(QTouchEvent * event);
  void handle_wheel_event(QWheelEvent * event);
  void handle_mouse_press_event(QMouseEvent * event);
  void handle_mouse_move_event(QMouseEvent * event);
  void handle_mouse_release_event(QMouseEvent * event);
  void handle_mouse_ungrab_event();
  void handle_touch_ungrab_event();

Q_SIGNALS:
  void pan_activeChanged();
  void pinch_activeChanged();
  void enabledChanged();
  void maximum_zoom_level_changeChanged();
  void accepted_gesturesChanged();
  void flick_decelerationChanged();
  void pinch_started(QcMapPinchEvent * pinch);
  void pinch_updated(QcMapPinchEvent * pinch);
  void pinch_finished(QcMapPinchEvent * pinch);
  void pan_started();
  void pan_finished();
  void flick_started();
  void flick_finished();
  void prevent_stealingChanged();

private:
  const QTouchEvent::TouchPoint & first_point() const  { return m_all_points.at(0); }
  const QTouchEvent::TouchPoint & second_point() const { return m_all_points.at(1); }

  void update();

  // Create general data relating to the touch points
  void touch_point_state_machine();
  void start_one_touch_point();
  void update_one_touch_point();
  void start_two_touch_points();
  void update_two_touch_points();

  // All pinch related code, which encompasses zoom
  void pinch_state_machine();
  bool can_start_pinch();
  void start_pinch();
  void update_pinch();
  void end_pinch();

  // Pan related code (regardles of number of touch points),
  // includes the flick based panning after letting go
  void pan_state_machine();
  bool can_start_pan();
  void update_pan();
  bool try_start_flick();
  void start_flick(int dx, int dy, int time_ms = 0);
  void stop_flick();

  bool pinch_enabled() const { return m_pinch.m_enabled; }
  void set_pinch_enabled(bool enabled);
  bool pan_enabled() const { return m_pan_enabled; }
  void set_pan_enabled(bool enabled);
  bool flick_enabled() const { return m_flick.m_enabled; }
  void set_flick_enabled(bool enabled);

private Q_SLOTS:
  void handle_flick_animation_stopped();

private:
  void stop_pan();
  void clear_touch_data();
  void update_velocity_list(const QcVectorDouble & pos);

private:
  // prototype state machine...
  enum TouchPointState
    {
      TouchPoints0,
      TouchPoints1,
      TouchPoints2
    } m_touch_point_state;

  enum PinchState
    {
      PinchInactive,
      PinchInactiveTwoPoints,
      PinchActive
    } m_pinch_state;

  enum FlickState
    {
      FlickInactive,
      PanActive,
      FlickActive
    } m_flick_state;

private:
  QcMapItem * m_map;
  bool m_enabled;
  Accepted_gestures m_accepted_gestures;

  // These are calculated regardless of gesture or number of touch points
  QScopedPointer<QTouchEvent::TouchPoint> m_mouse_point; // mouse event data (pointer so as to by undefined)
  QList<QTouchEvent::TouchPoint> m_touch_points; // touch event data
  QList<QTouchEvent::TouchPoint> m_all_points; // combined (touch and mouse) event data

  QcVectorDouble m_last_position_for_velocity; // used to compute velocity; first point or middle item position, then updated
  QElapsedTimer m_last_position_for_velocity_time; // used to compute velocity
  qreal m_velocity_x; // [px/s]
  qreal m_velocity_y;

  QcVectorDouble m_start_position1; // first point item position
  QcVectorDouble m_current_position; // first point or middle item position, then updated
  QcWgsCoordinate m_start_coordinate; // first point or middle coordinate, then updated when mouse pointer slides on the map

  // Only set when two points in contact
  QcVectorDouble m_start_position2; // second point position
  QcWgsCoordinate m_touch_center_coordinate; // scene center coordinate
  qreal m_two_touch_angle;
  qreal m_distance_between_touch_points;
  bool m_prevent_stealing;
  bool m_pan_enabled;

  struct Pinch m_pinch;
  struct Pan m_flick;
};

// QT_END_NAMESPACE

QML_DECLARE_TYPE(QcMapGestureArea)

#endif // MAP_GESTURE_AREA_H

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
