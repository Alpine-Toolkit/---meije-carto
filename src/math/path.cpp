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

#include <QVector>

#include "segment.h"
#include "line.h"

/**************************************************************************************************/

QcPath::QcPath()
  : m_vertexes(),
    m_interval(),
    m_closed(false)
{
}

/*
QcPath::QcPath(size_t number_of_vertexes)
  : m_vertexes(number_of_vertexes), m_edges(0)
{
  if (number_of_vertexes > 1)
    m_edges.reserve(number_of_vertexes -1);
}
*/

QcPath::QcPath(const VertexListType & vertexes, bool closed)
  : m_vertexes(vertexes),
    m_interval(),
    m_closed(closed)
{
  size_t number_of_vertexes = m_vertexes.size();

  if (number_of_vertexes) {
    m_interval = m_vertexes[0].to_interval();
    for (size_t i = 1; i < number_of_vertexes; i++)
      m_interval |= m_vertexes[i].to_interval();
  }
}

QcPath::QcPath(const QVector<Type> & coordinates, bool closed)
  : m_vertexes(),
    m_interval(),
    m_closed(closed)
{
  size_t number_of_coordinates = coordinates.size();
  if (number_of_coordinates % 2 == 1)
    throw std::invalid_argument("Odd number of coordinates");

  for (size_t i = 0; i < number_of_coordinates; i += 2) {
    add_vertex(VertexType(coordinates[i], coordinates[i+1]));
  }
}

QcPath::QcPath(const QcPath & path)
  : QcPath(path.m_vertexes, path.m_closed)
{}

QcPath::~QcPath()
{}

QcPath &
QcPath::operator=(const QcPath & other)
{
  if (this != &other) {
    m_vertexes = other.m_vertexes;
    m_interval = other.m_interval;
    m_closed = other.m_closed;
  }

  return *this;
}

bool
QcPath::operator==(const QcPath & other) const
{
  if (m_closed != other.m_closed)
    return false;

  // interval ?

  const VertexListType & other_vertexes = other.m_vertexes;
  int number_of_vertexes = m_vertexes.size();
  if (other_vertexes.size() != number_of_vertexes)
    return false;

  for (int i = 0; i < number_of_vertexes; i++)
    if (m_vertexes[i] != other_vertexes[i])
      return false;
  return true;
}

void
QcPath::clear()
{
  m_vertexes.clear();
  m_interval = IntervalType();
  m_closed = false;
}

void
QcPath::add_vertex(const VertexType & vertex)
{
  IntervalType vertex_interval = vertex.to_interval();
  size_t number_of_vertexes = m_vertexes.size();
  if (! number_of_vertexes)
    m_interval = vertex_interval;
  else
    m_interval |= vertex_interval;
  m_vertexes.push_back(vertex);
}

QcPath::EdgeType
QcPath::edge(int start_index) const
{
  int number_of_vertexes = m_vertexes.size();
  int last_index = number_of_vertexes - 1;
  if (start_index < last_index) // i > 0 &&
    return EdgeType(m_vertexes[start_index+1], m_vertexes[start_index]);
  else if (start_index == last_index && m_closed)
    return EdgeType(m_vertexes[0], m_vertexes[last_index]);
  else
    return EdgeType();
}

QcPath::Type
QcPath::length() const
{
  Type _length = .0;
  int number_of_vertexes = m_vertexes.size();
  if (number_of_vertexes < 2)
    return _length;

  int i = 0;
  for (; i < (number_of_vertexes -1); i++)
    _length += EdgeType(m_vertexes[i+1], m_vertexes[i]).length();
  if (m_closed)
    _length += EdgeType(m_vertexes[0], m_vertexes[i]).length();

  return _length;
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
