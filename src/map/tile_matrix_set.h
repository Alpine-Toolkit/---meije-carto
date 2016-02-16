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

#ifndef TILE_MATRIX_SET_H
#define TILE_MATRIX_SET_H

/**************************************************************************************************/

#include <vector>

#include <QtCore/QMetaType>
#include <QDebug>
#include <QString>

#include "qtcarto_global.h"
#include "map/geo_coordinate.h"
#include "map/earth.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

// class QDebug;
// class QDataStream;

/**************************************************************************************************/

class QC_EXPORT QcTileMatrixIndex
{
 public:
  QcTileMatrixIndex();
  QcTileMatrixIndex(size_t x, size_t y);
  QcTileMatrixIndex(const QcTileMatrixIndex & other);
  ~QcTileMatrixIndex();

  QcTileMatrixIndex &operator=(const QcTileMatrixIndex & other);

  bool operator==(const QcTileMatrixIndex & other) const;
  inline bool operator!=(const QcTileMatrixIndex & other) const {
    return !operator==(other);
  }

  inline void set_x(size_t x) {
    m_x = x;
  }
  inline size_t x() const {
    return m_x;
  }

  inline void set_y(size_t y) {
    m_y = y;
  }
  inline size_t y() const {
    return m_y;
  }

 private:
  size_t m_x; // column
  size_t m_y; // row
};

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug debug, const QcTileMatrixIndex & tile_index);
#endif

/**************************************************************************************************/

class QcTileMatrix;
class QcTileMatrixSet;

class QC_EXPORT QcTileMatrixSetIterator
{
 public:
  QcTileMatrixSetIterator (const QcTileMatrixSet * tile_matrix_set, size_t position);

  inline bool operator!= (const QcTileMatrixSetIterator & other) const {
    return m_position != other.m_position;
  }

  inline const QcTileMatrixSetIterator & operator++ () {
    ++m_position;
    return *this;
  }

  const QcTileMatrix & operator* () const;

 private:
  size_t m_position;
  const QcTileMatrixSet * m_tile_matrix_set;
};

// Fixme: subclass QVector ?
class QC_EXPORT QcTileMatrixSet
{
 public:
  // Fixme: ???
  static constexpr char const * projection = "epsg:3857";
  static constexpr double x_offset = -HALF_EQUATORIAL_PERIMETER;
  static constexpr double y_offset =  HALF_EQUATORIAL_PERIMETER;

 public:
  // Fixme: std::size_t ???
  QcTileMatrixSet(QString name, size_t number_of_level, size_t tile_size);

  inline QString name() const {
    return m_name;
  }

  inline size_t number_of_levels() const {
    return m_number_of_levels;
  }

  inline size_t tile_size() const {
    return m_tile_size;
  }

  inline double root_resolution() const {
    return m_root_resolution;
  }

  // Return the resolution of the given level [m/px].
  inline double level_resolution(size_t level) const {
    return m_root_resolution / double(1 << level);
  }

  // Return the smallest available resolution.
  inline double smallest_resolution() const {
    return level_resolution(m_number_of_levels -1);
  }

  // Return the closest level for the given resolution.
  size_t closest_level(double resolution) const;

  inline const QcTileMatrix & operator[](size_t level) const {
    return m_tile_matrix_set[level];
  };

  QcTileMatrixSetIterator begin() const {
    return QcTileMatrixSetIterator(this,  0);
  }

  QcTileMatrixSetIterator end() const {
    return QcTileMatrixSetIterator(this,  m_number_of_levels);
  }

  // public:
  //  typedef std::vector<QcTileMatrix>::const_iterator tile_matrix_set_iterator;
  // inline std::vector<QcTileMatrix>::const_iterator level_iterator() const {
  //   return m_tile_matrix_set.begin();
  // }

 private:
  QString m_name;
  size_t m_number_of_levels;
  size_t m_tile_size;
  double m_root_resolution;
  std::vector<QcTileMatrix> m_tile_matrix_set;
};

/**************************************************************************************************/

class QC_EXPORT QcTileMatrix
{
 public:
  QcTileMatrix(QcTileMatrixSet & tile_matrix_set, size_t level);

  inline const QcTileMatrixSet & tile_matrix_set() const {
    return m_tile_matrix_set;
  }

  inline const size_t level() const {
    return m_level;
  }

  inline const size_t mosaic_size() const {
    return m_mosaic_size;
  }

  inline double resolution() const {
    return m_resolution;
  }

  inline double tile_length_m() const {
    return m_tile_length_m;
  }

  inline size_t tile_size() const {
    return m_tile_matrix_set.tile_size();
  }

  QcTileMatrixIndex mercator_to_matrix_index(const QcGeoCoordinateMercator & coordinate) const;
  QcTileMatrixIndex mercator_to_matrix_index(const QcGeoCoordinateNormalisedMercator & coordinate) const;

 private:
  QcTileMatrixSet & m_tile_matrix_set;
  size_t m_level;
  size_t m_mosaic_size;
  double m_resolution;
  double m_tile_length_m;
};

// QT_END_NAMESPACE

/**************************************************************************************************/

#endif // TILE_MATRIX_SET_H

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
