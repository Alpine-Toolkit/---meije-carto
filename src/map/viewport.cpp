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

#include "map/viewport.h"

#include "coordinate/mercator.h"
#include "coordinate/debug_tools.h"
#include "math/qc_math.h"

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

QcViewportState::QcViewportState(const QcWgsCoordinate & coordinate, const QcTiledZoomLevel & tiled_zoom_level, double bearing)
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

QcViewportPart::QcViewportPart()
  : m_viewport(nullptr),
    m_position(),
    m_screen_interval(),
    m_polygon()
{}

QcViewportPart::QcViewportPart(const QcViewport * viewport,
                               int position,
                               const QcInterval2DDouble screen_interval,
                               const QcPolygon & polygon)
  : m_viewport(viewport),
    m_position(position),
    m_screen_interval(screen_interval),
    m_polygon(polygon)
{}

QcViewportPart::QcViewportPart(const QcViewportPart & other)
  : m_viewport(other.m_viewport),
    m_position(other.m_position),
    m_screen_interval(other.m_screen_interval),
    m_polygon(other.m_polygon)
{}

QcViewportPart::~QcViewportPart()
{}

QcViewportPart &
QcViewportPart::operator=(const QcViewportPart & other)
{
  if (this != &other) {
    m_viewport = other.m_viewport;
    m_position = other.m_position;
    m_screen_interval = other.m_screen_interval;
    m_polygon = other.m_polygon;
  }

  return *this;
}

void
QcViewportPart::clear()
{
  m_position = -1;
  m_polygon.clear();
}

bool
QcViewportPart::contains(const QcVectorDouble & projected_coordinate) const
{
  return m_polygon.interval().contains(projected_coordinate.x(), projected_coordinate.y());
}

QcVectorDouble
QcViewportPart::inf_position() const
{
  const QcInterval2DDouble & interval = m_polygon.interval();
  return QcVectorDouble(interval.x().inf(), interval.y().sup());
}

QcVectorDouble
QcViewportPart::map_vector(const QcVectorDouble & projected_coordinate) const
{
  return (projected_coordinate - inf_position()).mirror_y();
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug debug, const QcViewportPart & viewport_part)
{
    QDebugStateSaver saver(debug);

    debug.nospace().noquote() << "QcViewportPart(";
    if (viewport_part) {
      const QcProjection * projection = viewport_part.viewport()->projection_ptr();
      debug << viewport_part.position()
            << ", " << viewport_part.screen_interval()
            << ", " << format_interval(viewport_part.interval(), projection);
    }
    debug << ')';

    return debug;
}
#endif

/**************************************************************************************************/

QcViewport::QcViewport(const QcViewportState & viewport_state, const QSize & viewport_size)
  : m_state(viewport_state),
    m_previous_state(viewport_state), // else issue
    m_projection(nullptr), // Fixme: QcWebMercatorCoordinate::cls_projection
    m_viewport_size(viewport_size)
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
QcViewport::update_zoom_level_interval()
{
  // qInfo();
  if (m_smallest_tile_size != -1) {
    // int smallest_power_of_two_x = smallest_power_of_two(width() / m_smallest_tile_size);
    int smallest_power_of_two_y = smallest_power_of_two(height() / m_smallest_tile_size);
    // qInfo() << "viewport size" << m_viewport_size << m_map_zoom_level_interval << smallest_power_of_two_x << smallest_power_of_two_y;
    m_zoom_level_interval = QcIntervalInt(smallest_power_of_two_y, m_map_zoom_level_interval.sup());
  } else {
    m_zoom_level_interval = QcIntervalInt(-1, -1);
  }
}

void
QcViewport::set_zoom_level_interval(const QcIntervalInt & zoom_level_interval, int smallest_tile_size)
{
  m_smallest_tile_size = smallest_tile_size;
  m_map_zoom_level_interval = zoom_level_interval;
  update_zoom_level_interval();
}

void
QcViewport::set_viewport_size(const QSize & size, float device_pixel_ratio)
{
  Q_UNUSED(device_pixel_ratio);
  // qInfo() << "viewport size" << size << device_pixel_ratio;
  m_viewport_size = size; // * device_pixel_ratio;
  m_device_pixel_ratio = 1; // device_pixel_ratio;

  update_zoom_level_interval();

  update_area_size();
  update_area();
}

void
QcViewport::set_projection(const QcProjection * projection)
{
  if (!m_projection or *projection != *m_projection) {
    m_projection = projection; // Fixme: passing ‘const QcProjection’ as ‘this’ argument discards qualifiers
    m_is_web_mercator = *projection == QcWebMercatorCoordinate().cls_projection;
    QcWgsCoordinate coordinate(projection->wgs84_origin());
    set_center(coordinate);
  }
}

QcVectorDouble
QcViewport::to_projected_coordinate(const QcWgsCoordinate & coordinate) const
{
  if (m_is_web_mercator)
    return coordinate.web_mercator().vector();
  else
    return coordinate.transform(*m_projection); // Fixme:
}

QcWgsCoordinate
QcViewport::from_projected_coordinate(const QcVectorDouble & coordinate) const
{
  // adjust position if it is outside the valid domain (done ine QcWebMercatorCoordinate)
  if (m_is_web_mercator)
    return QcWebMercatorCoordinate(coordinate.x(), coordinate.y()).wgs84(); // Fixme:
  else {
    QcGeoCoordinate wgs = QcGeoCoordinate(m_projection, coordinate.x(), coordinate.y()).transform(&QcWgsCoordinate::cls_projection); // Fixme:
    // QcVectorDouble wgs = QcGeoCoordinateTrait::projection()
    return QcWgsCoordinate(wgs.x(), wgs.y());
  }
}

QcVectorDouble
QcViewport::projected_center_coordinate() const
{
  // Fixme: cache
  return to_projected_coordinate(center());
}

void
QcViewport::begin_state_transaction()
{
  if (m_state_transaction == 0)
    m_previous_state = m_state;
  m_state_transaction++;
}

void
QcViewport::end_state_transaction()
{
  if (m_state_transaction)
    m_state_transaction--;
}

void
QcViewport::set_center(const QcWgsCoordinate & coordinate)
{
  // qInfo() << coordinate;
  if (coordinate != m_state.coordinate()) {
    begin_state_transaction();
    m_state.set_coordinate(coordinate);
    update_area(); // move polygon
    end_state_transaction();
  }
}

void
QcViewport::set_bearing(double bearing)
{
  if (bearing != m_state.bearing()) {
    begin_state_transaction();
    m_state.set_bearing(bearing);
    update_area(); // rotate polygon
    end_state_transaction();
  }
}

void
QcViewport::set_zoom_level(unsigned int zoom_level)
{
  // Fixme: check is not performed at init
  if (zoom_level != m_state.zoom_level() and m_zoom_level_interval.contains(zoom_level)) {
    begin_state_transaction();
    m_state.set_zoom_level(zoom_level);
    update_area_size();
    update_area(); // scale polygon
    end_state_transaction();
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
QcViewport::zoom_at(const QcWgsCoordinate & coordinate, unsigned int zoom_level)
{
  // qInfo() << coordinate << zoom_level;
  if (zoom_level != m_state.zoom_level() and m_zoom_level_interval.contains(zoom_level)) {
    begin_state_transaction();
    m_state.set_zoom_level(zoom_level);
    set_center(coordinate); // move and scale polygon
    end_state_transaction();
  }
}

void
QcViewport::stable_zoom(const QcVectorDouble & screen_position, unsigned int zoom_level)
{
  // qInfo() << screen_position << zoom_level;

  // Fixme: (from map_gesture_area) event position == pre_zoom_point ???

  if (!m_zoom_level_interval.contains(zoom_level))
    return;

  begin_state_transaction(); // Fixme: ok ?

  // Fixme: ???
  QcWgsCoordinate coordinate = screen_to_coordinate(screen_position, false);
  QcVectorDouble pre_zoom_point = coordinate_to_screen(coordinate, false);
  if (!isnan(pre_zoom_point.x()))
    qWarning() << "pre is undefined";

  set_zoom_level(zoom_level);

  QcVectorDouble post_zoom_point = coordinate_to_screen(coordinate, false);
  if (!isnan(post_zoom_point.x()))
    qWarning() << "post is undefined";

  // qInfo() << coordinate << screen_position << '\n' << pre_zoom_point << '\n' << post_zoom_point;
  if (!isnan(pre_zoom_point.x()) and !isnan(post_zoom_point.x()) and pre_zoom_point != post_zoom_point) {
    // Keep location under pointer
    QcVectorDouble delta_px = post_zoom_point - pre_zoom_point;
    // Fixme: improve
    QcVectorDouble map_center_point = QcVectorDouble(width(), height()) * .5 + delta_px;
    QcWgsCoordinate center_coordinate = screen_to_coordinate(map_center_point, false);
    set_center(center_coordinate);
  }

  end_state_transaction();
}

void
QcViewport::pan(const QcVectorDouble & translation)
{
  // Fixme: translation unit are screen px
  QcVectorDouble position = projected_center_coordinate() + from_px(translation) * QcVectorDouble(1., -1.);
  set_center(from_projected_coordinate(position));
}

bool
QcViewport::is_interval_defined() const {
  return !m_viewport_polygon.interval().is_empty();
}

QcPolygon
QcViewport::compute_polygon() const
{
  QcVectorDouble center = projected_center_coordinate();

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

  // Bearing: make only sense for local map, with eventually one wrapping
  double _bearing = bearing();
  if (_bearing)
    polygon = polygon.rotate_counter_clockwise(_bearing); // Fixme: copy ?

  return polygon;
}

void
QcViewport::adjust_center()
{
  QcPolygon polygon = compute_polygon();
  // qInfo() << polygon.interval();

  // const QcIntervalDouble & x_projected_interval = m_projection->x_projected_interval();
  const QcIntervalDouble & y_projected_interval = m_projection->y_projected_interval();

  const QcInterval2DDouble & viewport_interval = polygon.interval();
  const QcIntervalDouble & x_viewport_interval = viewport_interval.x();
  const QcIntervalDouble & y_viewport_interval = viewport_interval.y();

  // Adjust y to fit in the projection domain
  m_center_map_vertically = false;
  if (!y_viewport_interval.is_included_in(y_projected_interval)) {
    // qInfo() << "Adjust y";
    QcVectorDouble projected_center = projected_center_coordinate();
    // Item height is larger than the map, map is centered
    if (y_projected_interval.length() < y_viewport_interval.length()) {
      QcWgsCoordinate center = from_projected_coordinate(QcVectorDouble(projected_center.x(), 0));
      m_state.set_coordinate(center);
      // reduce to the projected domain and center map
      polygon = interval_to_polygon(QcInterval2DDouble(x_viewport_interval, y_projected_interval));
      m_center_map_vertically = true;
    } else {
      // Ensure we cannot go outside of the projection domain
      double y = projected_center.y();
      if (y_projected_interval.sup() < y_viewport_interval.sup())
        y += y_projected_interval.sup() - y_viewport_interval.sup(); // < 0
      else if (y_viewport_interval.inf() < y_projected_interval.inf())
        y += y_projected_interval.inf() - y_viewport_interval.inf(); // > 0
      QcWgsCoordinate center = from_projected_coordinate(QcVectorDouble(projected_center.x(), y));
      m_state.set_coordinate(center);
      polygon = compute_polygon();
    }
  }

  // qInfo() << "final" << polygon.interval();

  // global polygon
  m_viewport_polygon = polygon;
}

void
QcViewport::update_area()
{
  // Fixme: to_px truncate to int, thus we have an incertitude of 1 px, ok ?

  // Skip if no map layer
  if (m_smallest_tile_size == -1)
    return;

  adjust_center();

  const QcIntervalDouble & x_projected_interval = m_projection->x_projected_interval();
  const QcIntervalDouble & y_projected_interval = m_projection->y_projected_interval();

  const QcInterval2DDouble & viewport_interval = m_viewport_polygon.interval();
  const QcIntervalDouble & x_viewport_interval = viewport_interval.x();
  const QcIntervalDouble & y_viewport_interval = viewport_interval.y();

  // qInfo() << width() << height() << viewport_interval;

  // Center map ?
  if (m_center_map_vertically) {
    double map_height = to_px(y_projected_interval.length());
    double y_offset = (height() - map_height) / 2;
    m_y_screen_interval = QcIntervalDouble(y_offset, y_offset + map_height);
  } else
    m_y_screen_interval = QcIntervalDouble(0, height()); // -1

  // Fixme: optimise
  m_west_part.clear();
  m_central_part.clear();
  m_east_part.clear();

  m_cross_boundaries = !x_viewport_interval.is_included_in(x_projected_interval);
  // qInfo() << "Cross boudaries" << m_cross_boundaries;
  if (m_cross_boundaries) {
    // double _bearing = bearing();
    // if (_bearing) {
    //   // Fixme: bearing and date line crossing
    // } else {
    // interval matches polygon
    double projected_x_inf = x_projected_interval.inf();
    double projected_x_sup = x_projected_interval.sup();
    double x_inf = x_viewport_interval.inf();
    double x_sup = x_viewport_interval.sup();
    m_cross_west_line = x_inf < projected_x_inf;
    m_cross_east_line = projected_x_sup < x_sup;
    // qInfo() << "west / est" << m_cross_west_line << m_cross_east_line;
    QcInterval2DDouble west_interval;
    QcInterval2DDouble central_interval;
    QcInterval2DDouble east_interval;

    // Fixme: if cross both ?

    /*
    QList<QcIntervalDouble> sub_intervals;
    double x_cut = x_viewport_interval.inf();
    int cut_count = 0;
    while (true) {
      double remainder = (!cut_count) ?
        fmod(fabs(x_cut), x_projected_interval.sup()) :
        x_projected_interval.sup();
      double next_x_cut = x_cut + remainder;
      if (next_x_cut < x_viewport_interval.sup())
        sub_intervals << QcIntervalDouble(x_cut, next_x_cut);
      else {
        sub_intervals << QcIntervalDouble(x_cut, x_viewport_interval.sup());
        break;
      }
      x_cut = next_x_cut;
      cut_count++;
    }
    qInfo() << sub_intervals;
    */

    int part_position = 0;
    double x_offset = .0;

    // West part
    if (m_cross_west_line) { // eastern tiles !
      // x_inf < projected_x_inf
      west_interval.x() = QcIntervalDouble(x_projected_interval.wrap(x_inf), projected_x_sup);
      west_interval.y() = y_viewport_interval;
      x_offset += west_interval.x_length();
      QcIntervalDouble x_screen_interval(0, to_px(x_offset));
      m_west_part = QcViewportPart(this, part_position++,
                                   QcInterval2DDouble(x_screen_interval, m_y_screen_interval),
                                   interval_to_polygon(west_interval));
    }

    // Central part
    m_number_of_full_maps = 0;
    if (m_cross_west_line and m_cross_east_line) {
      central_interval.x() = QcIntervalDouble(projected_x_inf, projected_x_sup);
      m_number_of_full_maps++;
    } else if (m_cross_west_line and !m_cross_east_line)
      central_interval.x() = QcIntervalDouble(projected_x_inf, x_sup);
    else if (!m_cross_west_line and m_cross_east_line)
      central_interval.x() = QcIntervalDouble(x_inf, projected_x_sup);
    central_interval.y() = y_viewport_interval;
    QcPolygon central_polygon = interval_to_polygon(central_interval);
    int x_screen_offset_inf = to_px(x_offset);
    double central_offset = central_interval.x_length();
    x_offset += central_offset;
    QcIntervalDouble x_central_screen_interval(x_screen_offset_inf, to_px(x_offset));
    m_central_part = QcViewportPart(this, part_position++,
                                    QcInterval2DDouble(x_central_screen_interval, m_y_screen_interval),
                                    central_polygon);
    m_central_part_clones.clear();
    double x_projected_interval_length = x_projected_interval.length();
    double x_viewport_interval_length = x_viewport_interval.length();
    while ((x_viewport_interval_length - x_offset) > x_projected_interval_length) {
      m_number_of_full_maps++;
      int x_screen_offset_inf = to_px(x_offset);
      x_offset += x_projected_interval_length;
      QcIntervalDouble x_screen_interval(x_screen_offset_inf, to_px(x_offset));
      QcViewportPart clone_part = QcViewportPart(this, part_position++,
                                                 QcInterval2DDouble(x_screen_interval, m_y_screen_interval),
                                                 central_polygon);
      m_central_part_clones << clone_part;
    }

    // East part
    if (m_cross_east_line) { // Fixme: tiles at west !
      // projected_x_sup < x_sup
      east_interval.x() = QcIntervalDouble(projected_x_inf, x_projected_interval.wrap(x_sup));
      east_interval.y() = y_viewport_interval;
      QcIntervalDouble x_screen_interval(to_px(x_offset), width()); // -1
      m_east_part = QcViewportPart(this, part_position,
                                   QcInterval2DDouble(x_screen_interval, m_y_screen_interval),
                                   interval_to_polygon(east_interval));
      // }
    }
  } else {
    m_cross_west_line = false;
    m_cross_east_line = false;
    m_number_of_full_maps = 0;
    QcIntervalDouble x_screen_interval(0, width()); // -1
    m_central_part = QcViewportPart(this, 0,
                                    QcInterval2DDouble(x_screen_interval, m_y_screen_interval),
                                    m_viewport_polygon);
  }

  // qInfo() << "West part" << m_west_part << '\n'
  //         << "Central part" << m_central_part << '\n'
  //         << "East part" << m_east_part << '\n'
  //         << "Number of full maps" << m_number_of_full_maps;

  emit viewport_changed();
}

const QcViewportPart *
QcViewport::find_part(const QcVectorDouble & projected_coordinate) const
{
  // Note: it is not a bijection when m_number_of_full_maps is > 1

  // Try central part first
  if (m_central_part.contains(projected_coordinate))
    return &m_central_part;
  else if (m_east_part.contains(projected_coordinate))
    return &m_east_part;
  else if (m_west_part.contains(projected_coordinate))
    return &m_west_part;
  else
    return nullptr;
}

/*!
    \qmlmethod coordinate QtLocation::Map::to_coordinate(QPointF position, bool clipToViewPort)

    Returns the coordinate which corresponds to the \a position relative to the map item.

    If \a cliptoViewPort is \c true, or not supplied then returns an invalid coordinate if
    \a position is not within the current viewport.
*/
QcVectorDouble
QcViewport::screen_to_projected_coordinate(const QcVectorDouble & screen_position, bool clip_to_viewport) const
{
  // qInfo() << screen_position << clip_to_viewport;

  // Fixme: purpose ?
  if (clip_to_viewport) {
    // Fixme: use interval, int versus double
    double x = screen_position.x();
    double y = screen_position.y();
    if ((x < 0) || (width() < x) || (y < 0) || (height() < y))
      return QcVectorDouble(qQNaN(), qQNaN());
  }

  // Fixme: screen_to_coordinate while return QcWgsCoordinate(0, 0)
  if (m_center_map_vertically and
      !m_y_screen_interval.contains(screen_position.y())) {
    qWarning() << "y screen position is out of map";
    return QcVectorDouble(qQNaN(), qQNaN());
  }

  const QcInterval2DDouble & interval = m_viewport_polygon.interval();
  QcVectorDouble projected_position(interval.x().inf(), interval.y().sup());
  QcVectorDouble screen_offset(0, m_y_screen_interval.inf());
  projected_position += from_px(screen_position - screen_offset).mirror_y();

  return projected_position; // Fixme: not wrapped !
}

QcWgsCoordinate
QcViewport::screen_to_coordinate(const QcVectorDouble & screen_position, bool clip_to_viewport) const
{
  QcVectorDouble projected_position = screen_to_projected_coordinate(screen_position, clip_to_viewport);
  if (isnan(projected_position.x()))
    return QcWgsCoordinate();
  else
    return from_projected_coordinate(projected_position);
}

/*!
    \qmlmethod point QtLocation::Map::from_coordinate(coordinate coordinate, bool clipToViewPort)

    Returns the position relative to the map item which corresponds to the \a coordinate.

    If \a cliptoViewPort is \c true, or not supplied then returns an invalid QPointF if
    \a coordinate is not within the current viewport.
*/
QcVectorDouble
QcViewport::coordinate_to_screen(const QcVectorDouble & projected_coordinate, bool clip_to_viewport) const
{
  // qInfo() << coordinate << clip_to_viewport
  //         << (int)projected_coordinate.x() << (int)projected_coordinate.y();

  const QcViewportPart * part = find_part(projected_coordinate);
  if (!part) {
    qWarning() << "out of domain";
    return QcVectorDouble(qQNaN(), qQNaN());
  }

  QcVectorDouble screen_position = to_px(part->map_vector(projected_coordinate));

  // Fixme: purpose
  if (clip_to_viewport) {
    int w = width();
    int h = height();
    double x = screen_position.x();
    double y = screen_position.y();
    if ((x < 0.0) || (x > w) || (y < 0) || (y > h) || qIsNaN(x) || qIsNaN(y))
      return QcVectorDouble(qQNaN(), qQNaN());
  }

  // qInfo() << screen_position;

  return screen_position;
}

QcVectorDouble
QcViewport::coordinate_to_screen(const QcWgsCoordinate & coordinate, bool clip_to_viewport) const
{
  QcVectorDouble projected_coordinate = to_projected_coordinate(coordinate);
  return coordinate_to_screen(projected_coordinate, clip_to_viewport);
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
  // qInfo() << max_length_px << max_length << number_of_digits << normalised_max_length << digit;
  double length = digit * power_10;

  return QcMapScale(length, ceil(to_px(length) / m_device_pixel_ratio));
}

/***************************************************************************************************
 *
 * END
 *
 **************************************************************************************************/
