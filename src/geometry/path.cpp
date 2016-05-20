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

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
