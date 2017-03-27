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

#include "path.h"

#include <algorithm>
#include <exception>
#include <limits>

#include <QVector>

#include "segment.h"
#include "line.h"

/**************************************************************************************************/

template <typename T, template<typename> class Vector>
QcPath<T, Vector>::QcPath()
  : m_vertexes(),
    m_edges(),
    m_interval(),
    m_closed(false)
{}

/*
QcPath<T>::QcPath(int number_of_vertexes)
  : m_vertexes(number_of_vertexes), m_edges(0)
{
  if (number_of_vertexes > 1)
    m_edges.reserve(number_of_vertexes -1);
}
*/

template <typename T, template<typename> class Vector>
QcPath<T, Vector>::QcPath(const VertexListType & vertexes, bool closed)
  : m_vertexes(vertexes),
    m_edges(),
    m_interval(),
    m_closed(closed)
{
  int _number_of_vertexes = number_of_vertexes();

  if (_number_of_vertexes) {
    m_interval = m_vertexes[0].to_interval();
    for (int i = 1; i < _number_of_vertexes; i++) {
      const VertexType & vertex = m_vertexes[i];
      m_interval |= vertex.to_interval();
      m_edges << EdgeType(vertex, m_vertexes[i-1]);
    }
  }
}

/*
template <typename T, template<typename> class Vector>
QcPath<T, Vector>::QcPath(const QVector<T> & coordinates, bool closed)
  : m_vertexes(),
    m_edges(),
    m_interval(),
    m_closed(closed)
{
  int number_of_coordinates = coordinates.size();
  int dimension = Vector<T>::dimension();
  if (number_of_coordinates % dimension == 1)
    throw std::invalid_argument("Odd number of coordinates");

  for (int i = 0; i < number_of_coordinates; i += dimension)
    add_vertex(VertexType(coordinates[i], coordinates[i+1]));
}
*/

template <typename T, template<typename> class Vector>
QcPath<T, Vector>::QcPath(const QcPath & path)
  : QcPath(path.m_vertexes, path.m_closed)
{}

template <typename T, template<typename> class Vector>
QcPath<T, Vector>::~QcPath()
{}

template <typename T, template<typename> class Vector>
QcPath<T, Vector> &
QcPath<T, Vector>::operator=(const QcPath & other)
{
  if (this != &other) {
    m_vertexes = other.m_vertexes;
    m_edges = other.m_edges;
    m_interval = other.m_interval;
    m_closed = other.m_closed;
  }

  return *this;
}

template <typename T, template<typename> class Vector>
bool
QcPath<T, Vector>::operator==(const QcPath & other) const
{
  if (m_closed != other.m_closed)
    return false;

  // interval ?

  const VertexListType & other_vertexes = other.m_vertexes;
  int _number_of_vertexes = number_of_vertexes();
  if (other_vertexes.size() != _number_of_vertexes)
    return false;

  for (int i = 0; i < _number_of_vertexes; i++)
    if (m_vertexes[i] != other_vertexes[i])
      return false;
  return true;
}

template <typename T, template<typename> class Vector>
void
QcPath<T, Vector>::clear()
{
  m_vertexes.clear();
  m_edges.clear();
  m_interval = IntervalType();
  m_closed = false;
}

template <typename T, template<typename> class Vector>
void
QcPath<T, Vector>::add_vertex(const VertexType & vertex)
{
  IntervalType vertex_interval = vertex.to_interval();
  int _number_of_vertexes = number_of_vertexes();
  if (! _number_of_vertexes)
    m_interval = vertex_interval;
  else {
    m_interval |= vertex_interval;
    m_edges << EdgeType(m_vertexes.constLast(), vertex);
  }
  m_vertexes << vertex;
}

template <typename T, template<typename> class Vector>
int
QcPath<T, Vector>::number_of_edges() const
{
  int _number_of_edges = m_edges.size();
  if (m_closed)
    _number_of_edges++;
  return _number_of_edges;
}

/*
QcPath<T, Vector>::EdgeType
QcPath<T, Vector>::edge(int start_index) const
{
  int _number_of_vertexes = number_of_vertexes();
  int last_index = _number_of_vertexes - 1;
  if (start_index < last_index) // i > 0 &&
    return EdgeType(m_vertexes[start_index+1], m_vertexes[start_index]);
  else if (start_index == last_index && m_closed)
    return EdgeType(m_vertexes[0], m_vertexes[last_index]);
  else
    return EdgeType();
}
*/

template <typename T, template<typename> class Vector>
typename QcPath<T, Vector>::EdgeListType
QcPath<T, Vector>::edges() const
{
  // Fixme: overhead ???
  if (m_closed) {
     EdgeListType edges = m_edges;
     edges << closing_edge();
     return edges;
  } else
    return m_edges;
}

template <typename T, template<typename> class Vector>
T
QcPath<T, Vector>::length() const
{
  T _length = .0;
  int _number_of_vertexes = number_of_vertexes();
  if (_number_of_vertexes < 2)
    return _length;

  for (const auto & edge : m_edges)
    _length += edge.length();
  if (m_closed)
    _length += closing_edge().length();

  return _length;
}

template <typename T, template<typename> class Vector>
typename QcPath<T, Vector>::VertexType
QcPath<T, Vector>::barycenter() const
{
  VertexType _barycenter;

  for (const auto & vertex : m_vertexes)
    _barycenter += vertex;

  return _barycenter / (T) number_of_vertexes();
}

template <typename T, template<typename> class Vector>
int
QcPath<T, Vector>::nearest_vertex_index(const VertexType & point, T & distance) const
{
  int vertex_index = 0;
  int vertex_index_min = -1;
  T distance_min = std::numeric_limits<T>::max();
  for (const auto & vertex : m_vertexes) {
    T distance = (vertex - point).magnitude_square();
    if (distance < distance_min || vertex_index_min == -1) {
      distance_min = distance;
      vertex_index_min = vertex_index;
    }
    vertex_index++;
  }

  distance = sqrt(distance_min);
  return vertex_index_min;
}

template <typename T, template<typename> class Vector>
const typename QcPath<T, Vector>::VertexType &
QcPath<T, Vector>::nearest_vertex(const VertexType & point, T & distance) const
{
  return m_vertexes[nearest_vertex_index(point, distance)];
}

template <typename T, template<typename> class Vector>
const typename QcPath<T, Vector>::EdgeType &
QcPath<T, Vector>::nearest_edge(const VertexType & point, T & distance, T & abscissa) const
{
  int edge_index = 0;
  int edge_min = -1;
  T abscissa_min;
  T distance_min = std::numeric_limits<T>::max();
  EdgeListType _edges = edges();
  for (const auto & edge : _edges) {
    QcLine<T> line(edge.p1(), edge.vector());
    EdgeType edge_point(edge.p1(), point);
    T _distance, _abscissa;
    line.distance_and_abscissa_to_line(edge_point.vector(), _distance, _abscissa);
    if (distance < distance_min || edge_min == -1) {
      distance_min = _distance;
      edge_min = edge_index;
      abscissa_min = _abscissa;
    }
    edge_index++;
  }

  bool inside_edge = 0. <= abscissa_min && abscissa_min <= 1.;
  if (!inside_edge) {
    bool before = abscissa_min < 0;
    int edge_index_last = _edges.size() -1;
    edge_index = edge_min + (before ? -1 : 1);
    if (edge_index < 0)
      edge_index = edge_index_last;
    else if (edge_index > edge_index_last)
      edge_index = 0;
    const EdgeType & edge2 = _edges[edge_index];
    QcLine<T> line2(edge2.p1(), edge2.vector());
    EdgeType edge_point2(edge2.p1(), point);
    T distance2, abscissa2;
    line2.distance_and_abscissa_to_line(edge_point2.vector(), distance2, abscissa2);
    if ((before && (abscissa2 - 1.) < abs(abscissa_min))
        || (abs(abscissa2) < (abscissa_min - 1.))) {
      edge_min = edge_index;
      distance_min = distance2;
      abscissa_min = abscissa2;
    }
  }

  distance = distance_min;
  abscissa = abscissa_min;
  return _edges[edge_min];
}

template <typename T, template<typename> class Vector>
bool
QcPath<T, Vector>::is_self_intersecting() const
{
  EdgeListType _edges = edges();
  int number_of_edges = _edges.size();
  for (int i = 0; i < number_of_edges; i++)
    for (int j = i + 1; j < number_of_edges; j++)
      if (_edges[i].intersect(_edges[j]))
        return true;
  return false;
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
