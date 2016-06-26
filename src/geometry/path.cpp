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

#include "polygon_seidler_triangulation.h"

/**************************************************************************************************/

template<>
QcPathDouble::QcPath(const QVector<double> & coordinates, bool closed)
  : m_vertexes(),
    m_edges(),
    m_interval(),
    m_closed(closed)
{
  int number_of_coordinates = coordinates.size();
  int dimension = QcVectorDouble::dimension();
  if (number_of_coordinates % dimension == 1)
    throw std::invalid_argument("Odd number of coordinates");

  for (int i = 0; i < number_of_coordinates; i += dimension)
    add_vertex(VertexType(coordinates[i], coordinates[i+1]));
}

template<>
QcPath3DDouble::QcPath(const QVector<double> & coordinates, bool closed)
  : m_vertexes(),
    m_edges(),
    m_interval(),
    m_closed(closed)
{
  int number_of_coordinates = coordinates.size();
  int dimension = QcVector3DDouble::dimension();
  if (number_of_coordinates % dimension == 1)
    throw std::invalid_argument("Odd number of coordinates");

  for (int i = 0; i < number_of_coordinates; i += dimension)
    add_vertex(VertexType(coordinates[i], coordinates[i+1], coordinates[i+2]));
}

/*
QcPolygonTriangulation
QcPath::triangulate() const
{
  return QcPolygonTriangulation(*this);
}
*/

/**************************************************************************************************/

QcTriangleVertex::QcTriangleVertex()
  : p1(), p2(), p3()
{}

QcTriangleVertex::QcTriangleVertex(const QcVectorDouble & p1,
                                   const QcVectorDouble & p2,
                                   const QcVectorDouble & p3)
  : p1(p1), p2(p2), p3(p3)
{}

QcTriangleVertex::QcTriangleVertex(const QcTriangleVertex & other)
  : p1(other.p1),
    p2(other.p2),
    p3(other.p3)
{}

QcTriangleVertex::~QcTriangleVertex()
{}

QcTriangleVertex &
QcTriangleVertex::operator=(const QcTriangleVertex & other)
{
  if (this != &other) {
    p1 = other.p1;
    p2 = other.p2;
    p3 = other.p3;
  }

  return *this;
}

QcPolygonTriangulation::QcPolygonTriangulation(const QcPathDouble & path)
  : m_path(path)
{
  int number_of_vertexes = path.number_of_vertexes();
  int number_of_segments = number_of_vertexes -2; // Fixme: true ?

  double vertexes[number_of_vertexes][2];
  int i = 0;
  for (const auto & vertex : path.vertexes()) {
    qInfo() << vertex;
    vertexes[i][0] = vertex.x();
    vertexes[i][1] = vertex.y();
    i++;
  }

  int triangles[number_of_segments][3];
  for (int i = 0; i < number_of_segments; i++) {
    for (int j = 0; j < 3; j++)
      triangles[i][j] = 0;
  }

  QcSeidlerPolygonTriangulation triangulation(number_of_vertexes, vertexes, triangles);

  for (int i = 0; i < number_of_segments; i++) {
    qInfo() << triangles[i][0] << triangles[i][1] << triangles[i][2];
    m_triangles << QcTriangleIndex(triangles[i][0] -1,
                                   triangles[i][1] -1,
                                   triangles[i][2] -1);
  }
}

QList<QcTriangleVertex>
QcPolygonTriangulation::triangle_vertexes() const
{
  QList<QcTriangleVertex> triangle_vertexes;
  for (const auto & triangle: m_triangles) {
    qInfo() << triangle.p1 << triangle.p2 << triangle.p3;
    qInfo() << m_path.vertex_at(triangle.p1);
    qInfo() << m_path.vertex_at(triangle.p2);
    qInfo() << m_path.vertex_at(triangle.p3);
    triangle_vertexes << QcTriangleVertex(m_path.vertex_at(triangle.p1),
                                          m_path.vertex_at(triangle.p2),
                                          m_path.vertex_at(triangle.p3));
  }
  return triangle_vertexes;
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
