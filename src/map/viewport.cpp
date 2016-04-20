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

#include <QDataStream>
#include <QDebug>
#include <QtMath>
#include <qnumeric.h>

/**************************************************************************************************/

#include "math/qc_math.h"
#include "map/viewport.h"

/**************************************************************************************************/

QcZoomFactor::QcZoomFactor()
  : QcZoomFactor(1.)
{
}

QcZoomFactor::QcZoomFactor(double zoom_factor)
  : m_zoom_factor(qQNaN())
{
  set_zoom_factor(zoom_factor);
}

QcZoomFactor::QcZoomFactor(const QcZoomFactor & other)
  : m_zoom_factor(other.m_zoom_factor)
{
}

QcZoomFactor::~QcZoomFactor()
{
}

QcZoomFactor &
QcZoomFactor::operator=(const QcZoomFactor & other)
{
  if (this != &other) {
    m_zoom_factor = other.m_zoom_factor;
  }

  return *this;
}

void
QcZoomFactor::set_zoom_factor(double zoom_factor)
{
  qInfo() << "set_zoom_factor" << zoom_factor;
  if (zoom_factor > 0)
    m_zoom_factor = zoom_factor;
  else
    throw std::invalid_argument("Invalid zoom factor must be > 0");
}

/**************************************************************************************************/

QcTiledZoomLevel::QcTiledZoomLevel()
  : QcTiledZoomLevel(0, 0, 0)
{}

QcTiledZoomLevel::QcTiledZoomLevel(double map_size, unsigned int tile_size, unsigned int zoom_level)
  : QcZoomFactor(),
    m_tile_size(tile_size),
    m_zoom_level(0),
    m_map_size(map_size)
{
  set_zoom_level(zoom_level);
}

QcTiledZoomLevel::QcTiledZoomLevel(const QcTiledZoomLevel & other)
  : QcZoomFactor(other),
    m_tile_size(other.m_tile_size),
    m_zoom_level(other.m_zoom_level),
    m_map_size(other.m_map_size)
{
}

QcTiledZoomLevel::~QcTiledZoomLevel()
{
}

QcTiledZoomLevel &
QcTiledZoomLevel::operator=(const QcTiledZoomLevel & other)
{
  if (this != &other) {
    m_tile_size = other.m_tile_size;
    m_zoom_level = other.m_zoom_level;
    m_map_size = other.m_map_size;
    set_zoom_factor(other.zoom_factor());
  }

  return *this;
}

bool
QcTiledZoomLevel::operator==(const QcTiledZoomLevel & other) const
{
  return ((m_map_size == other.m_map_size) &&
	  (m_tile_size == other.m_tile_size) &&
	  (m_zoom_level == other.m_zoom_level) &&
	  QcZoomFactor::operator==(other));
}

void
QcTiledZoomLevel::set_zoom_level(unsigned int zoom_level)
{
  m_zoom_level = zoom_level;
  size_t mosaic_size = 1 << zoom_level;
  unsigned int map_size_px = m_tile_size * mosaic_size;
  double zoom_factor = m_map_size / double(map_size_px); // unit is m/px
  set_zoom_factor(zoom_factor);
  qInfo() << "set_zoom_level" << zoom_level << map_size_px;
}

/**************************************************************************************************/

QcInterval2DDouble
interval_from_center_and_size(const QcVectorDouble & center, const QcVectorDouble & size)
{
  QcVectorDouble point_inf = center - size * .5;
  QcVectorDouble point_sup = point_inf + size;

  return QcInterval2DDouble(point_inf.x(), point_sup.x(),
			    point_inf.y(), point_sup.y());
}

QcViewportState::QcViewportState()
  : m_coordinate(), m_tiled_zoom_level(), m_bearing(0)
{}

QcViewportState::QcViewportState(const QcGeoCoordinateNormalisedWebMercator & coordinate, const QcTiledZoomLevel & tiled_zoom_level, double bearing)
  : m_coordinate(coordinate), m_tiled_zoom_level(tiled_zoom_level), m_bearing(0)
{
  set_bearing(bearing);
}

QcViewportState::QcViewportState(const QcGeoCoordinateWebMercator & coordinate, const QcTiledZoomLevel & tiled_zoom_level, double bearing)
  : QcViewportState(coordinate.normalised_web_mercator(), tiled_zoom_level, bearing)
    // : m_coordinate(coordinate.normalised_web_mercator()), m_bearing(0), m_tiled_zoom_level(tiled_zoom_level)
{
  set_bearing(bearing);
}

QcViewportState::QcViewportState(const QcGeoCoordinateWGS84 & coordinate, const QcTiledZoomLevel & tiled_zoom_level, double bearing)
  : m_coordinate(coordinate.normalised_web_mercator()), m_tiled_zoom_level(tiled_zoom_level), m_bearing(0)
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
}

void
QcViewport::set_viewport_size(const QSize & size)
{
  m_viewport_size = size;
  update_area();
}

void
QcViewport::set_coordinate(const QcGeoCoordinateWebMercator & coordinate)
{
  const QcInterval2DDouble & domain = QcGeoCoordinateWebMercator::domain();
  double x = domain.x().wrap(coordinate.x());
  double y = domain.y().truncate(coordinate.y());
  QcGeoCoordinateWebMercator new_coordinate(x, y);
  if (new_coordinate != m_state.web_mercator()) {
    m_state.set_coordinate(coordinate);
    update_area(); // move polygon
  }
}

void
QcViewport::set_coordinate(const QcGeoCoordinateNormalisedWebMercator & coordinate)
{
  set_coordinate(coordinate.web_mercator());
}

void
QcViewport::set_coordinate(const QcGeoCoordinateWGS84 & coordinate)
{
  set_coordinate(coordinate.web_mercator());
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
    update_area(); // scale polygon
  }
}

void
QcViewport::set_zoom_factor(double zoom_factor)
{
  // require to compute zoom level
}

void
QcViewport::zoom_at(const QcGeoCoordinateWebMercator & coordinate, unsigned int zoom_level)
{
  qInfo() << "zoom_at" << coordinate << zoom_level;
  if (zoom_level != m_state.zoom_level())
    m_state.set_zoom_level(zoom_level);
  set_coordinate(coordinate); // move and scale polygon
}

void
QcViewport::pan(double x, double y)
{
  const QcGeoCoordinateWebMercator & old_coordinate = web_mercator();
  QcGeoCoordinateWebMercator new_coordinate(old_coordinate.x() + x, old_coordinate.y() + y); // could use vector api
  set_coordinate(new_coordinate);
}

void
QcViewport::update_area()
{
  // cache them ?
  QcVectorDouble viewport_size = QcVectorDouble(m_viewport_size.width(), m_viewport_size.height());
  QcVectorDouble new_area_size = viewport_size * zoom_factor(); // [px] * [m/px]
  // mercator is centered
  QcVectorDouble center = normalised_web_mercator().vector() * EQUATORIAL_PERIMETER;
  qInfo() << "viewport_size" << viewport_size;
  qInfo() << "zoom_factor" << zoom_factor() << "m/px";
  qInfo() << "new_area_size" << new_area_size << "m";
  qInfo() << "center" << (int) center.x() << (int) center.y();

  // QcInterval2DDouble new_area = interval_from_center_and_size(center, new_area_size);

  QcVectorDouble half_diagonal = new_area_size * .5;
  QcVectorDouble point1 = center + half_diagonal;
  QcVectorDouble point2 = center + half_diagonal.mirror_x();
  QcVectorDouble point3 = center + half_diagonal.rotate_180();
  QcVectorDouble point4 = center + half_diagonal.mirror_y();
  QcPolygon polygon; // = {}
  polygon.add_vertex(point3);
  polygon.add_vertex(point2);
  polygon.add_vertex(point1);
  polygon.add_vertex(point4);
  double _bearing = bearing();
  if (_bearing)
    m_polygon = polygon.rotate_counter_clockwise(_bearing);
  else
    m_polygon = polygon;
  m_cross_datum = m_polygon.interval().is_included_in(QcGeoCoordinateWebMercator::domain());

  emit viewport_changed();
}

/**************************************************************************************************/

// #include "viewport.moc"

/***************************************************************************************************
 *
 * END
 *
 **************************************************************************************************/
