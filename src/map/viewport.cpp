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

#include "math/qc_math.h"
#include "map/viewport.h"

#include <cmath>

#include <QDataStream>
#include <QDebug>
#include <QtMath>
#include <qnumeric.h>

/**************************************************************************************************/

QcMapResolution::QcMapResolution()
  : QcMapResolution(1.)
{}

QcMapResolution::QcMapResolution(double resolution)
  : m_resolution(qQNaN())
{
  set_resolution(resolution);
}

QcMapResolution::QcMapResolution(const QcMapResolution & other)
  : m_resolution(other.m_resolution)
{}

QcMapResolution::~QcMapResolution()
{}

QcMapResolution &
QcMapResolution::operator=(const QcMapResolution & other)
{
  if (this != &other) {
    m_resolution = other.m_resolution;
  }

  return *this;
}

void
QcMapResolution::set_resolution(double resolution)
{
  // qInfo() << "set_resolution" << resolution;
  if (resolution > 0)
    m_resolution = resolution;
  else
    throw std::invalid_argument("Invalid zoom factor must be > 0");
}

/**************************************************************************************************/

QcTiledZoomLevel::QcTiledZoomLevel()
  : QcTiledZoomLevel(0, 0, 0)
{}

QcTiledZoomLevel::QcTiledZoomLevel(double map_size, unsigned int tile_size, unsigned int zoom_level)
  : QcMapResolution(),
    m_tile_size(tile_size),
    m_zoom_level(0),
    m_map_size(map_size)
{
  set_zoom_level(zoom_level);
}

QcTiledZoomLevel::QcTiledZoomLevel(const QcTiledZoomLevel & other)
  : QcMapResolution(other),
    m_tile_size(other.m_tile_size),
    m_zoom_level(other.m_zoom_level),
    m_map_size(other.m_map_size)
{}

QcTiledZoomLevel::~QcTiledZoomLevel()
{}

QcTiledZoomLevel &
QcTiledZoomLevel::operator=(const QcTiledZoomLevel & other)
{
  if (this != &other) {
    m_tile_size = other.m_tile_size;
    m_zoom_level = other.m_zoom_level;
    m_map_size = other.m_map_size;
    set_resolution(other.resolution());
  }

  return *this;
}

bool
QcTiledZoomLevel::operator==(const QcTiledZoomLevel & other) const
{
  return ((m_map_size == other.m_map_size) &&
	  (m_tile_size == other.m_tile_size) &&
	  (m_zoom_level == other.m_zoom_level) &&
	  QcMapResolution::operator==(other));
}

void
QcTiledZoomLevel::set_zoom_level(unsigned int zoom_level)
{
  m_zoom_level = zoom_level;
  double resolution = QcTileMatrixSet::resolution_for_level(m_map_size, m_tile_size, zoom_level); // unit is m/px
  set_resolution(resolution);
  // qInfo() << "set_zoom_level" << zoom_level << map_size_px;
}

/**************************************************************************************************/

// Fixme: move ?
QcInterval2DDouble
interval_from_center_and_radius(const QcVectorDouble & center, const QcVectorDouble & radius)
{
  QcVectorDouble point_inf = center - radius;
  QcVectorDouble point_sup = center + radius; // or point_inf + 2 * radius

  return QcInterval2DDouble(point_inf.x(), point_sup.x(),
			    point_inf.y(), point_sup.y());
}

QcPolygon
interval_to_polygon(const QcInterval2DDouble & interval)
{
  QcIntervalDouble x_interval = interval.x();
  QcIntervalDouble y_interval = interval.y();
  QcPolygon polygon;
  // clockwise
  polygon.add_vertex(QcVectorDouble(x_interval.inf(), y_interval.inf()));
  polygon.add_vertex(QcVectorDouble(x_interval.inf(), y_interval.sup()));
  polygon.add_vertex(QcVectorDouble(x_interval.sup(), y_interval.sup()));
  polygon.add_vertex(QcVectorDouble(x_interval.sup(), y_interval.inf()));
  return polygon;
}

/**************************************************************************************************/

QcViewportState::QcViewportState()
  : m_coordinate(), m_tiled_zoom_level(), m_bearing(0)
{}

QcViewportState::QcViewportState(const QcGeoCoordinateWGS84 & coordinate, const QcTiledZoomLevel & tiled_zoom_level, double bearing)
  : m_coordinate(coordinate.pseudo_web_mercator()), m_tiled_zoom_level(tiled_zoom_level), m_bearing(0)
{
  set_bearing(bearing);
}

QcViewportState::QcViewportState(const QcGeoCoordinateWebMercator & coordinate, const QcTiledZoomLevel & tiled_zoom_level, double bearing)
  : QcViewportState(coordinate.pseudo_web_mercator(), tiled_zoom_level, bearing)
    // : m_coordinate(coordinate.pseudo_web_mercator()), m_bearing(0), m_tiled_zoom_level(tiled_zoom_level)
{
  set_bearing(bearing);
}

QcViewportState::QcViewportState(const QcGeoCoordinatePseudoWebMercator & coordinate, const QcTiledZoomLevel & tiled_zoom_level, double bearing)
  : m_coordinate(coordinate), m_tiled_zoom_level(tiled_zoom_level), m_bearing(0)
{
  set_bearing(bearing);
}

QcViewportState::QcViewportState(const QcViewportState & other)
  : m_coordinate(other.m_coordinate), m_tiled_zoom_level(other.m_tiled_zoom_level), m_bearing(other.m_bearing)
{}

QcViewportState::~QcViewportState()
{}

void
QcViewportState::set_bearing(double bearing) {
  if (is_valid_bearing(bearing))
    m_bearing = bearing;
  else
    throw std::invalid_argument("Invalid bearing");
}

QcViewportState &
QcViewportState::operator=(const QcViewportState & other)
{
  if (this != &other) {
    m_coordinate = other.m_coordinate;
    m_bearing = other.m_bearing;
    m_tiled_zoom_level = other.m_tiled_zoom_level;
  }

  return *this;
}

bool
QcViewportState::operator==(const QcViewportState & other) const
{
  return (m_coordinate == other.m_coordinate
	  && m_bearing == other.m_bearing
	  && m_tiled_zoom_level == other.m_tiled_zoom_level);
}

/**************************************************************************************************/

QcViewport::QcViewport(const QcViewportState & viewport_state, const QSize & viewport_size)
  : m_state(viewport_state), m_viewport_size(viewport_size)
{
  update_area_size();
}

void
QcViewport::update_area_size()
{
  QcVectorDouble viewport_size = QcVectorDouble(m_viewport_size.width(), m_viewport_size.height());
  m_area_size_m = from_px(viewport_size);
  m_half_diagonal_m = m_area_size_m * .5;
}

void
QcViewport::set_viewport_size(const QSize & size, float device_pixel_ratio)
{
  qInfo() << "viewport size" << size << device_pixel_ratio;
  m_viewport_size = size* device_pixel_ratio;
  m_device_pixel_ratio = device_pixel_ratio;
  update_area_size();
  update_area();
}

void
QcViewport::set_coordinate(const QcGeoCoordinatePseudoWebMercator & coordinate)
{
  if (coordinate != m_state.pseudo_web_mercator()) {
    m_state.set_coordinate(coordinate);
    update_area(); // move polygon
  }
}

void
QcViewport::set_bearing(double bearing)
{
  if (bearing != m_state.bearing()) {
    m_state.set_bearing(bearing);
    update_area(); // rotate polygon
  }
}

void
QcViewport::set_zoom_level(unsigned int zoom_level)
{
  if (zoom_level != m_state.zoom_level()) {
    m_state.set_zoom_level(zoom_level);
    update_area_size();
    update_area(); // scale polygon
  }
}

/*
void
QcViewport::set_resolution(double resolution)
{
  // Fixme: !!!
  // require to compute zoom level
}
*/

void
QcViewport::zoom_at(const QcGeoCoordinatePseudoWebMercator & coordinate, unsigned int zoom_level)
{
  // qInfo() << "zoom_at" << coordinate << zoom_level;
  if (zoom_level != m_state.zoom_level())
    m_state.set_zoom_level(zoom_level);
  set_coordinate(coordinate); // move and scale polygon
}

void
QcViewport::stable_zoom(const QcVectorDouble & position_px, unsigned int zoom_level)
{
  // Fixme: (from map_gesture_area) event position == pre_zoom_point ???

  QcGeoCoordinatePseudoWebMercator coordinate = to_coordinate(position_px, false);
  QcVectorDouble pre_zoom_point = from_coordinate(coordinate, false);

  set_zoom_level(zoom_level);

  QcVectorDouble post_zoom_point = from_coordinate(coordinate, false);

  // qInfo() << event->posF() << pre_zoom_point << "\n" << post_zoom_point;
  if (pre_zoom_point != post_zoom_point) {
    // Keep location under pointer
    QcVectorDouble delta_px = post_zoom_point - pre_zoom_point;
    // Fixme: improve
    QcVectorDouble map_center_point(width() * .5 + delta_px.x(), height() * .5  + delta_px.y());
    QcGeoCoordinatePseudoWebMercator center_coordinate = to_coordinate(map_center_point, false);
    set_coordinate(center_coordinate);
  }
}

void
QcViewport::pan(const QcVectorDouble & translation)
{
  // Fixme: x are px
  QcVectorDouble position = pseudo_web_mercator().vector();
  position += from_px(translation);
  // qInfo() << x << "px" << x_m << "m";
  // Ctor will adjust position if it is outside the valid domain
  QcGeoCoordinatePseudoWebMercator new_coordinate(position.x(), position.y());
  set_coordinate(new_coordinate);
}

void
QcViewport::update_area()
{
  // const QcInterval2DDouble & current_interval = middle_interval();
  // qInfo() << "Current Pseudo Mercator polygon interval [m]"
  //         << "[" << (int) current_interval.x().inf() << ", " << (int) current_interval.x().sup() << "]"
  //         << "x"
  //         << "[" << (int) current_interval.y().inf() << ", " << (int) current_interval.y().sup() << "]";

  // Use pseudo mercator to match the origin to the top-left corner
  QcVectorDouble center = pseudo_web_mercator().vector();

  // qInfo() << "viewport_size" << viewport_size;
  // qInfo() << "resolution" << resolution() << "m/px";
  // qInfo() << "area_size" << m_area_size << "m";
  // qInfo() << "center as pseudo mercator" << (int) center.x() << (int) center.y();

  // QcInterval2DDouble new_area = interval_from_center_and_radius(center, m_half_diagonal_m);

  // Fixme: cache more ?
  // Fixme: anti-clockwise ???
  QcVectorDouble point1 = center + m_half_diagonal_m;
  QcVectorDouble point2 = center + m_half_diagonal_m.mirror_x();
  QcVectorDouble point3 = center + m_half_diagonal_m.rotate_180();
  QcVectorDouble point4 = center + m_half_diagonal_m.mirror_y();
  QcPolygon polygon; // = {}
  polygon.add_vertex(point3);
  polygon.add_vertex(point2);
  polygon.add_vertex(point1);
  polygon.add_vertex(point4);

  double _bearing = bearing();
  if (_bearing)
    polygon = polygon.rotate_counter_clockwise(_bearing); // Fixme: copy ?

  // Fixme: check y interval lie in mercator domain

  const QcInterval2DDouble & _interval = polygon.interval();
  m_cross_date_line = !_interval.is_included_in(QcGeoCoordinatePseudoWebMercator::domain());
  // qInfo() << "Cross date line" << m_cross_date_line;
  if (m_cross_date_line) {
    if (_bearing) {
      // Fixme: bearing and date line crossing
    } else {
      // interval matches polygon
      double mercator_x_sup = QcGeoCoordinatePseudoWebMercator::x_interval().sup();
      double x_inf = _interval.x().inf();
      double x_sup = _interval.x().sup();
      m_cross_west_line = x_inf < 0;
      m_cross_east_line = mercator_x_sup < x_sup;
      // qInfo() << m_cross_west_line << m_cross_east_line;
      QcInterval2DDouble west_interval;
      QcInterval2DDouble middle_interval;
      QcInterval2DDouble east_interval;
      // Fixme: cross both ?
      if (m_cross_west_line) { // Fixme: tiles at east !
        west_interval.x() = QcIntervalDouble(mercator_x_sup + x_inf, mercator_x_sup -1.); // x_inf < 0
        middle_interval.x() = QcIntervalDouble(0, x_sup);
        middle_interval.y() = _interval.y();
        west_interval.y() = _interval.y();
        m_west_polygon = interval_to_polygon(west_interval);
      }
      if (m_cross_east_line) { // Fixme: tiles at west !
        middle_interval.x() = QcIntervalDouble(x_inf, mercator_x_sup -1.); // Fixme: -1 else take tile on border
        east_interval.x() = QcIntervalDouble(0, x_sup - mercator_x_sup); // mercator_x_sup < x_sup
        middle_interval.y() = _interval.y();
        east_interval.y() = _interval.y();
        m_east_polygon = interval_to_polygon(east_interval);
      }
      // qInfo() << "West interval [m]"
      //         << "[" << (int) west_interval.x().inf() << ", " << (int) west_interval.x().sup() << "]"
      //         << "x"
      //         << "[" << (int) west_interval.y().inf() << ", " << (int) west_interval.y().sup() << "]"
      //         << "Middle interval [m]"
      //         << "[" << (int) middle_interval.x().inf() << ", " << (int) middle_interval.x().sup() << "]"
      //         << "x"
      //         << "[" << (int) middle_interval.y().inf() << ", " << (int) middle_interval.y().sup() << "]"
      //         << "East interval [m]"
      //         << "[" << (int) east_interval.x().inf() << ", " << (int) east_interval.x().sup() << "]"
      //         << "x"
      //         << "[" << (int) east_interval.y().inf() << ", " << (int) east_interval.y().sup() << "]";
      m_middle_polygon = interval_to_polygon(middle_interval);
    }
  } else {
    m_west_polygon.clear();
    m_middle_polygon = polygon;
    m_east_polygon.clear();

    // qInfo() << "Updated Pseudo Mercator polygon interval [m]"
    //         << "[" << (int) _interval.x().inf() << ", " << (int) _interval.x().sup() << "]"
    //         << "x"
    //         << "[" << (int) _interval.y().inf() << ", " << (int) _interval.y().sup() << "]";
  }

  emit viewport_changed();
}

QcVectorDouble
QcViewport::inf_point() const
{
  const QcInterval2DDouble & _interval = middle_interval();
  return QcVectorDouble(_interval.x().inf(), _interval.y().inf());
}

/*!
    \qmlmethod coordinate QtLocation::Map::to_coordinate(QPointF position, bool clipToViewPort)

    Returns the coordinate which corresponds to the \a position relative to the map item.

    If \a cliptoViewPort is \c true, or not supplied then returns an invalid coordinate if
    \a position is not within the current viewport.
*/
QcGeoCoordinatePseudoWebMercator
QcViewport::to_coordinate(const QcVectorDouble & position_px, bool clip_to_viewport) const
{
  // Fixme: if cross datum

  // qInfo() << position_px << clip_to_viewport;

  if (clip_to_viewport) {
    // Fixme: use interval, int versus double
    double x = position_px.x();
    double y = position_px.y();

    if ((x < 0) || (width() < x) || (y < 0) || (height() < y))
      return QcGeoCoordinatePseudoWebMercator();
  }

  QcVectorDouble position_m = inf_point();
  position_m += from_px(position_px);

  return QcGeoCoordinatePseudoWebMercator(position_m.x(), position_m.y());
}

/*!
    \qmlmethod point QtLocation::Map::from_coordinate(coordinate coordinate, bool clipToViewPort)

    Returns the position relative to the map item which corresponds to the \a coordinate.

    If \a cliptoViewPort is \c true, or not supplied then returns an invalid QPointF if
    \a coordinate is not within the current viewport.
*/
QcVectorDouble
QcViewport::from_coordinate(const QcGeoCoordinatePseudoWebMercator & coordinate, bool clip_to_viewport) const
{
  // Fixme: if cross datum

  // qInfo() << coordinate << clip_to_viewport;

  QcVectorDouble position_px = to_px(coordinate.vector() - inf_point());

  if (clip_to_viewport) {
    int w = width();
    int h = height();
    double x = position_px.x();
    double y = position_px.y();
    if ((x < 0.0) || (x > w) || (y < 0) || (y > h) || qIsNaN(x) || qIsNaN(y))
      return QcVectorDouble(qQNaN(), qQNaN());
  }

  // qInfo() << position_px;
  // Fixme: return QPointF(qQNaN(), qQNaN());
  return position_px;
}

double
find_scale_digit(double x)
{
  // 1 <= x < 10
  if (5 <= x)
    return 5.;
  else if (2 <= x)
    return 2.;
  else
    return 1.;
}

QcMapScale
QcViewport::make_scale(unsigned int max_length_px)
{
  max_length_px *= m_device_pixel_ratio;
  double max_length = from_px(max_length_px);

  int number_of_digits = trunc(log(max_length)/log(10.));
  // Fixme: number_of_digits == 0 ?
  double power_10 = pow(10., number_of_digits);
  double normalised_max_length = max_length / power_10;
  double digit = find_scale_digit(normalised_max_length);
  qInfo() << max_length_px << max_length << number_of_digits << normalised_max_length << digit;
  double length = digit * power_10;

  return QcMapScale(length, ceil(to_px(length) / m_device_pixel_ratio));
}

/**************************************************************************************************/

// #include "viewport.moc"

/***************************************************************************************************
 *
 * END
 *
 **************************************************************************************************/
