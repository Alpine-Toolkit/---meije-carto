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

#include "tile_matrix_index.h"

/**************************************************************************************************/

QcTileMatrixIndex::QcTileMatrixIndex(int x, int y)
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

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug debug, const QcTileMatrixIndex & tile_index)
{
  QDebugStateSaver saver(debug); // Fixme: ???

  debug.nospace() << "QcTileMatrixIndex(";
  debug << tile_index.x();
  debug << ", ";
  debug << tile_index.y();
  debug << ')';

  return debug;
}
#endif

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
