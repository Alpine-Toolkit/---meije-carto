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

#include "tile_matrix_set.h"

/**************************************************************************************************/

QcTileMatrixIndex::QcTileMatrixIndex(size_t x, size_t y)
  : m_x(x), m_y(y)
{}

QcTileMatrixIndex::QcTileMatrixIndex()
  : QcTileMatrixIndex(0, 0)
{}

QcTileMatrixIndex::QcTileMatrixIndex(const QcTileMatrixIndex & other)
  : QcTileMatrixIndex(other.m_x, other.m_y)
{
}

QcTileMatrixIndex::~QcTileMatrixIndex()
{}

QcTileMatrixIndex &
QcTileMatrixIndex::operator=(const QcTileMatrixIndex &other)
{
  if (this != &other) {
    m_x = other.m_x;
    m_y = other.m_y;
  }

  return *this;
}

bool
QcTileMatrixIndex::operator==(const QcTileMatrixIndex &other) const
{
  return (m_x == other.m_x) && (m_y == other.m_y);
}

/**************************************************************************************************/

QcTileMatrixSetIterator::QcTileMatrixSetIterator (const QcTileMatrixSet * tile_matrix_set, size_t position)
  : m_position(position), m_tile_matrix_set(tile_matrix_set)
{}

const QcTileMatrix &
QcTileMatrixSetIterator::operator* () const {
  return (*m_tile_matrix_set)[m_position];
}

/**************************************************************************************************/

QcTileMatrixSet::QcTileMatrixSet(QString name, size_t number_of_levels, size_t tile_size)
  : m_name(name),
    m_number_of_levels(number_of_levels),
    m_tile_size(tile_size),
    m_root_resolution(EQUATORIAL_PERIMETER / tile_size)
{
  for (size_t level = 0; level < number_of_levels; level++)
    m_tile_matrix_set.emplace_back(*this, level);
}

size_t
QcTileMatrixSet::closest_level(double resolution) const
{
  return rint(log(m_root_resolution / resolution) / log(2));
}

/**************************************************************************************************/

QcTileMatrix::QcTileMatrix(QcTileMatrixSet & tile_matrix_set, size_t level)
  : m_tile_matrix_set(tile_matrix_set),
    m_level(level),
    m_mosaic_size(2^level),
    m_resolution(tile_matrix_set.level_resolution(level)),
    m_tile_length_m(0)
{
  m_tile_length_m = tile_size() * m_resolution;
}

QcTileMatrixIndex
QcTileMatrix::mercator_to_matrix_index(const QcGeoCoordinateMercator & coordinate) const
{
  double xm = coordinate.x() - QcTileMatrixSet::x_offset;
  double ym = QcTileMatrixSet::y_offset - coordinate.y();

  size_t x = int(xm / m_tile_length_m);
  size_t y = int(ym / m_tile_length_m);
  return QcTileMatrixIndex(x, y);

  // if (x < m_mosaic_size and y < m_mosaic_size)
  // else raise
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
