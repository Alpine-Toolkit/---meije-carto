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

#include "polygon.h"

#include <algorithm>
#include <exception>

#include <QVector>

#include "segment.h"
#include "line.h"

/**************************************************************************************************/

QcPolygon::QcPolygon()
  : m_vertexes()
{
}

/*
QcPolygon::QcPolygon(size_t number_of_vertexes)
  : m_vertexes(number_of_vertexes), m_edges(0)
{
  if (number_of_vertexes > 1)
    m_edges.reserve(number_of_vertexes -1);
}
*/

QcPolygon::QcPolygon(const QList<QcVectorDouble> & vertexes)
  : m_vertexes(vertexes)
{
  size_t number_of_vertexes = m_vertexes.size();

  if (number_of_vertexes) {
    m_interval = m_vertexes[0].to_interval();
    for (size_t i = 1; i < number_of_vertexes; i++)
      m_interval |= m_vertexes[i].to_interval();
  }
}

QcPolygon::QcPolygon(const QVector<double> & coordinates)
{
  size_t number_of_coordinates = coordinates.size();
  if (number_of_coordinates % 2 == 1)
    throw std::invalid_argument("Odd number of coordinates");

  for (size_t i = 0; i < number_of_coordinates; i += 2) {
    add_vertex(QcVectorDouble(coordinates[i], coordinates[i+1]));
  }
}

QcPolygon::QcPolygon(const QcPolygon & polygon)
  : m_vertexes(polygon.m_vertexes)
{}

QcPolygon::~QcPolygon()
{}

void
QcPolygon::add_vertex(const QcVectorDouble & vertex)
{
  // qinfo() << "add_vertex" << vertex;
  QcInterval2DDouble vertex_interval = vertex.to_interval();
  size_t number_of_vertexes = m_vertexes.size();
  if (! number_of_vertexes)
    m_interval = vertex_interval;
  else
    m_interval |= vertex_interval;
  m_vertexes.push_back(vertex);
}

/*
  Travelling from p0 to p1 to p2.
  @return -1 for counter-clockwise or if p0 is on the line segment between p1 and p2
           1 for clockwise or if p1 is on the line segment between p0 and p2
           0 if p2 in on the line segment between p0 and p1
*/
int
triangle_orientation(const QcVectorDouble & p0, const QcVectorDouble & p1, const QcVectorDouble & p2)
{
  double dx1 = p1.x() - p0.x();
  double dy1 = p1.y() - p0.y();
  double dx2 = p2.x() - p0.x();
  double dy2 = p2.y() - p0.y();

  // second slope is greater than the first one --> counter-clockwise
  if (dx1 * dy2 > dx2 * dy1) {
    return 1;
  }
  // first slope is greater than the second one --> clockwise
  else if (dx1 * dy2 < dx2 * dy1) {
    return -1;
  }
  // both slopes are equal --> collinear line segments
  else {
    // p0 is between p1 and p2
    if (dx1 * dx2 < 0 || dy1 * dy2 < 0) {
      return -1;
    }
    // p2 is between p0 and p1, as the length is compared
    // square roots are avoided to increase performance
    else if (dx1 * dx1 + dy1 * dy1 >= dx2 * dx2 + dy2 * dy2) {
      return 0;
    }
    // p1 is between p0 and p2
    else {
      return 1;
    }
  }
}

/*
  Checks if the line segments intersect.
  @return 1 if there is an intersection
          0 otherwise
*/
unsigned int
intersect(const QcSegmentDouble & line1, const QcSegmentDouble & line2)
{
  // triangle_orientation returns 0 if two points are identical, except from the situation
  // when p0 and p1 are identical and different from p2
  int ccw11 = triangle_orientation(line1.p1(), line1.p2(), line2.p1());
  int ccw12 = triangle_orientation(line1.p1(), line1.p2(), line2.p2());
  int ccw21 = triangle_orientation(line2.p1(), line2.p2(), line1.p1());
  int ccw22 = triangle_orientation(line2.p1(), line2.p2(), line1.p2());

  return (((ccw11 * ccw12 < 0) && (ccw21 * ccw22 < 0))
	  // one ccw value is zero to detect an intersection
	  || (ccw11 * ccw12 * ccw21 * ccw22 == 0)) ? 1 : 0;
}

/*
  @return next valid index (current + 1 or start index)
  for an array with n entries
  @param n entries count
  @param current current index
*/
inline size_t
get_next_index(size_t number_of_items, size_t current)
{
  return current == number_of_items - 1 ? 0 : current + 1;
}

// Test if the point is inside the polygon.
// Algorithm:
//   http://arxiv.org/abs/1207.3502
//   A correct even-odd algorithm for the point-in-polygon problem for complex polygons
//   Michael Galetzka, Patrick O. Glauner
bool
QcPolygon::contains(const QcVectorDouble & test_point) const
{
  size_t number_of_vertexes = m_vertexes.size();

  // Initial start point
  const QcVectorDouble origin(0, 0);
  QcVectorDouble start_point;
  QcVectorDouble end_point;

  // Create axes
  QcSegmentDouble x_axis(origin, origin);
  QcSegmentDouble x_axis_positive(origin, origin);

  int start_node_position = -1;

  // Is test_point on a node?
  // Move polygon to 0|0
  // Enlarge axes
  QList<QcVectorDouble> vertexes;
  for (size_t i = 0; i < number_of_vertexes; i++) {
    const QcVectorDouble & vertex = m_vertexes[i];
    if (test_point == vertex)
      return true;

    // Translate polygon to 0|0
    QcVectorDouble translated_vertex = vertex - test_point;
    vertexes.push_back(translated_vertex);

    // Find start point which is not on the x axis
    if (translated_vertex.y() != 0) {
      start_point = translated_vertex;
      start_node_position = i;
    }

    // Enlarge axes
    double x = translated_vertex.x();
    if (x > x_axis.p2().x()) {
      x_axis.p2().set_x(x);
      x_axis_positive.p2().set_x(x);
    }
    if (x < x_axis.p1().x()) {
      x_axis.p1().set_x(x);
    }
  }

  // Move test_point to 0|0
  QcSegmentDouble test_point_line(origin, origin);
  QcSegmentDouble edge;

  // Is test_point on an edge?
  for (size_t i = 0; i < number_of_vertexes; i++) {
    edge.set_p1(vertexes[i]);
    // Get correct index of successor edge
    edge.set_p2(vertexes[get_next_index(number_of_vertexes, i)]);
    if (intersect(test_point_line, edge) == 1) {
	return true;
    }
  }

  // No start point found and point is not on an edge or node
  // --> point is outside
  if (start_node_position == -1) {
    return false;
  }

  unsigned int count = 0;
  size_t seen_points = 0;
  size_t i = start_node_position;

  // Consider all edges
  while (seen_points < number_of_vertexes) {

    size_t saved_index = get_next_index(number_of_vertexes, i);
    int saved_x = vertexes[saved_index].x();

    // Move to next point which is not on the x-axis
    do {
      i = get_next_index(number_of_vertexes, i);
      seen_points++;
    } while (vertexes[i].y() == 0);
    // Found end point
    end_point = vertexes[i];

    // Only intersect lines that cross the x-axis
    if (start_point.y() * end_point.y() < 0) {
      edge.set_p1(start_point);
      edge.set_p2(end_point);

      // No nodes have been skipped
      // and the successor node has been chosen as the end point
      if (i == saved_index) {
	count += intersect(edge, x_axis_positive);
      }
      // If at least one node on the right side has been skipped,
      // the original edge would have been intersected
      // --> intersect with full x-axis
      else if (saved_x > 0) {
	count += intersect(edge, x_axis);
      }
    }

    // End point is the next start point
    start_point = end_point;
  }

  // Odd count --> in the polygon (1)
  // Even count --> outside (0)
  return bool(count % 2);
}

inline int
to_grid(double x, double grid_step)
{
  return int(x / grid_step);
}

class OpenInterval
{
public:
  OpenInterval(double x, int direction)
    : x(x), direction(direction)
  {}

  OpenInterval(const OpenInterval & other)
    : x(other.x), direction(other.direction)
  {}

  OpenInterval & operator=(const OpenInterval & other)
  {
    if (this != &other) {
      x = other.x;
      direction = other.direction;
    }

    return *this;
  }

  bool is_gap(const OpenInterval & other) const {
    return other.direction < 0 && direction > 0 && (x - other.x) >= 2;
  }

  friend bool operator<(const OpenInterval & open_interval1, const OpenInterval & open_interval2) {
    return open_interval1.x < open_interval2.x;
  }

  double x;
  double direction;
};

QcTiledPolygon
QcPolygon::intersec_with_grid(double grid_step) const {
  return QcTiledPolygon(*this, grid_step);
}

QcPolygon
QcPolygon::rotate_counter_clockwise(double angle) const
{
  QList<QcVectorDouble> vertexes(m_vertexes);
  for (QcVectorDouble & vertex: vertexes)
    vertex.rotate_counter_clockwise(angle);

  return QcPolygon(vertexes);
}

/**************************************************************************************************/

QcTiledPolygonRun::QcTiledPolygonRun(int y, const QcIntervalInt & interval)
  : m_y(y), m_interval(interval)
{
}

bool
QcTiledPolygonRun::operator==(const QcTiledPolygonRun & other) const
{
  return (m_y == other.m_y) && (m_interval == other.m_interval);
}

bool
QcTiledPolygonRun::cut(const QcTiledPolygonRun & other,
		       QcIntervalInt & intersection, QcIntervalInt & left,  QcIntervalInt & right,
		       bool & exchanged) const
{
  if (m_y == other.m_y) {
    intersection = m_interval.cut(other.m_interval, left, right, exchanged);
    return true;
  }
  else
    return false;
}

QVector<QcTileMatrixIndex>
QcTiledPolygonRun::tile_indexes() const
{
  QVector<QcTileMatrixIndex> _tile_indexes(m_interval.length());
  size_t i;
  int x;
  for (i = 0, x = m_interval.inf(); x <= m_interval.sup(); x++, i++)
    _tile_indexes[i] = QcTileMatrixIndex(x, m_y);
  return _tile_indexes;
}

/**************************************************************************************************/

QcTiledPolygon::QcTiledPolygon(const QcPolygon & polygon, double grid_step)
  : m_polygon(polygon), m_grid_step(grid_step)
{
  const QcInterval2DDouble & polygon_interval = polygon.interval();
  const QList<QcVectorDouble> & vertexes = polygon.vertexes();

  QcInterval2DInt interval_on_grid(to_grid(polygon_interval.x().inf(), grid_step),
				   to_grid(polygon_interval.x().sup(), grid_step),
				   to_grid(polygon_interval.y().inf(), grid_step),
				   to_grid(polygon_interval.y().sup(), grid_step));

  int Y_min = interval_on_grid.y().inf();
  size_t number_of_rows = interval_on_grid.y().length();
  QVector<QList<OpenInterval>> rows(number_of_rows);

  size_t number_of_vertexes = vertexes.size();
  for (size_t i = 0; i < number_of_vertexes; i++) {
    const QcVectorDouble & p0 = vertexes[i];
    size_t ii = i + 1;
    if (ii == number_of_vertexes)
      ii = 0;
    const QcVectorDouble & p1 = vertexes[ii];

    double X0 = to_grid(p0.x(), grid_step);
    double Y0 = to_grid(p0.y(), grid_step);
    double X1 = to_grid(p1.x(), grid_step);
    double Y1 = to_grid(p1.y(), grid_step);

    // qinfo() << "P0 - P1" << i << X0 << Y0 << X1 << Y1;
    QcLineDouble line = QcLineDouble::from_two_points(p0, p1);

    if (Y1 > Y0) {
      rows[Y0 - Y_min].push_back(OpenInterval(X0, 1));
      for (size_t Y = Y0 +1; Y < Y1 +1; Y++) {
	double y = Y * grid_step;
	double x = line.get_x_from_y(y);
	int X = to_grid(x, grid_step);
	int YY = Y - Y_min;
	if (X1 < X0)
	  YY -= 1;
	OpenInterval open_interval(X, 1);
	rows[YY].push_back(open_interval);
      }
      rows[Y1 - Y_min].push_back(OpenInterval(X1, 1));
    }
    else if (Y1 < Y0) {
      rows[Y1 - Y_min].push_back(OpenInterval(X1, -1));
      for (size_t Y = Y1 +1; Y < Y0 +1; Y++) {
	double y = Y * grid_step;
	double x = line.get_x_from_y(y);
	double X = to_grid(x, grid_step);
	int YY = Y - Y_min;
	if (X1 < X0)
	  YY -= 1;
	OpenInterval open_interval(X, -1);
	rows[YY].push_back(open_interval);
      }
      rows[Y0 - Y_min].push_back(OpenInterval(X0, -1));
    }
  }
  // qinfo() << "OpenInterval built";

  for (size_t i = 0; i < number_of_rows; i++) {
    QList<OpenInterval> & row = rows[i];
    if (!row.size()) // Fixme: check
      continue;
    std::sort(row.begin(), row.end());
    int Y = Y_min + i;
    OpenInterval previous_interval = row[0];
    double x_inf = previous_interval.x;
    QList<QcIntervalInt> intervals;
    intervals.push_back(QcIntervalInt(x_inf, x_inf));
    size_t number_of_intervals = row.size();
    if (number_of_intervals > 1)
      for (size_t j = 1; j < number_of_intervals; j++) {
 	const OpenInterval & open_interval = row[j];
	// qinfo() << "i,j: " << i << j << open_interval.x << open_interval.direction;
	if (open_interval.is_gap(previous_interval)) {
	  double x_inf = open_interval.x;
	  intervals.push_back(QcIntervalInt(x_inf, x_inf));
	}
	else {
	  intervals.last().set_sup(open_interval.x);
	  previous_interval = open_interval;
	}
      }
    for (const QcIntervalInt & interval : intervals)
      m_runs.push_back(QcTiledPolygonRun(Y, interval));
  }
}

QcTiledPolygonDiff
QcTiledPolygon::diff(const QcTiledPolygon & old_tiled_polygon)
{
  QcTiledPolygonDiff tiled_polygon_diff;
  QcTiledPolygonRunList old_runs = old_tiled_polygon.m_runs;

  bool exchanged;
  QcIntervalInt intersection;
  QcIntervalInt left;
  QcIntervalInt right;
  QVector<unsigned int> intersection_count(old_runs.size(), 0);

  for (const QcTiledPolygonRun & new_run : m_runs) {
    bool has_intersection = false;
    size_t i = 0;
    for (const QcTiledPolygonRun & old_run : old_runs) {
      if (new_run.cut(old_run, intersection, left, right, exchanged)) {
	int y = new_run.y();
	qInfo() << y << intersection << left << right << exchanged;
	has_intersection = true;
	intersection_count[i] += 1;

	tiled_polygon_diff.add_same_area(QcTiledPolygonRun(y, intersection));

	if (left.is_not_empty()) {
	  QcTiledPolygonRun run(y, left);
	  if (exchanged)
	    tiled_polygon_diff.add_old_area(run);
	  else
	    tiled_polygon_diff.add_new_area(run);
	}

	if (right.is_not_empty()) {
	  QcTiledPolygonRun run(y, right);
	  if (exchanged)
	    tiled_polygon_diff.add_new_area(run);
	  else
	    tiled_polygon_diff.add_old_area(run);
	}
      }
      i++;
    }

    if (!has_intersection)
      tiled_polygon_diff.add_new_area(new_run);
  }

  for (int i = 0; i < old_runs.size(); i++) {
    if (!intersection_count[i])
      tiled_polygon_diff.add_old_area(old_runs[i]);
  }

  return tiled_polygon_diff;
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
