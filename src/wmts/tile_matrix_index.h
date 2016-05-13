// -*- mode: c++ -*-

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

#ifndef TILE_MATRIX_INDEX_H
#define TILE_MATRIX_INDEX_H

/**************************************************************************************************/

#include <QDebug>

#include "qtcarto_global.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QC_EXPORT QcTileMatrixIndex
{
 public:
  QcTileMatrixIndex();
  QcTileMatrixIndex(int x, int y);
  QcTileMatrixIndex(const QcTileMatrixIndex & other);
  ~QcTileMatrixIndex();

  QcTileMatrixIndex &operator=(const QcTileMatrixIndex & other);

  bool operator==(const QcTileMatrixIndex & other) const;
  inline bool operator!=(const QcTileMatrixIndex & other) const {
    return !operator==(other);
  }

  inline void set_x(int x) {
    m_x = x;
  }
  inline int x() const {
    return m_x;
  }

  inline void set_y(int y) {
    m_y = y;
  }
  inline int y() const {
    return m_y;
  }

 private:
  int m_x; // column
  int m_y; // row
};

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug debug, const QcTileMatrixIndex & tile_index);
#endif

/**************************************************************************************************/

#endif // TILE_MATRIX_INDEX_H

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
