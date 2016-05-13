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

#include "tile_matrix_set.h"

#include <cmath>

/**************************************************************************************************/

QcTileMatrixSetIterator::QcTileMatrixSetIterator (const QcTileMatrixSet * tile_matrix_set, int position)
  : m_position(position), m_tile_matrix_set(tile_matrix_set)
{}

const QcTileMatrix &
QcTileMatrixSetIterator::operator* () const {
  return (*m_tile_matrix_set)[m_position];
}

/**************************************************************************************************/

double
QcTileMatrixSet::resolution_for_level(double map_size, int tile_size, int zoom_level)
{
  return map_size / double(tile_size * (1 << zoom_level)); // unit is m/px
}

QcTileMatrixSet::QcTileMatrixSet(QString name, int number_of_levels, int tile_size)
  : m_name(name),
    m_number_of_levels(number_of_levels),
    m_tile_size(tile_size),
    m_root_resolution(EQUATORIAL_PERIMETER / tile_size)
{
  for (int level = 0; level < number_of_levels; level++)
    m_tile_matrix_set.emplace_back(*this, level);
}

int
QcTileMatrixSet::closest_level(double resolution) const
{
  return rint(log(m_root_resolution / resolution) / log(2));
}

/**************************************************************************************************/

QcTileMatrix::QcTileMatrix(QcTileMatrixSet & tile_matrix_set, int level)
  : m_tile_matrix_set(tile_matrix_set),
    m_level(level),
    m_mosaic_size(1 << level),
    m_resolution(tile_matrix_set.level_resolution(level)),
    m_tile_length_m(0)
{
  m_tile_length_m = tile_size() * m_resolution;
}

QcTileMatrixIndex
QcTileMatrix::mercator_to_matrix_index(const QcGeoCoordinateWebMercator & coordinate) const
{
  // return mercator_to_matrix_index(coordinate.normalised_mercator());

  double xm = coordinate.x() - QcTileMatrixSet::x_offset;
  double ym = QcTileMatrixSet::y_offset - coordinate.y();

  int x = int(xm / m_tile_length_m);
  int y = int(ym / m_tile_length_m);

  if (x < m_mosaic_size and y < m_mosaic_size)
    return QcTileMatrixIndex(x, y);
  else
    throw std::invalid_argument("Invalid coordinate");
}

QcTileMatrixIndex
QcTileMatrix::mercator_to_matrix_index(const QcGeoCoordinateNormalisedWebMercator & coordinate) const
{
  int x = int(coordinate.x() * m_mosaic_size);
  int y = int(coordinate.x() * m_mosaic_size);

  if (x < m_mosaic_size and y < m_mosaic_size)
    return QcTileMatrixIndex(x, y);
  else
    throw std::invalid_argument("Invalid coordinate");
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
