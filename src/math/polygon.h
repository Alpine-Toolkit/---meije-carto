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

#ifndef __POLYGON_H__
#define __POLYGON_H__

/**************************************************************************************************/

#include <cmath>

#include <QVector>
#include <QList>

#include "qtcarto_global.h"
#include "math/path.h"
#include "math/qc_math.h"
#include "map/tile_matrix_set.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcTiledPolygon;
class QcTiledPolygonRun;
class QcTiledPolygonDiff;

typedef QList<QcTiledPolygonRun> QcTiledPolygonRunList;

class QC_EXPORT QcPolygon : public QcPath
{
 public:
  QcPolygon();
  // QcPolygon(size_t number_of_vertexes);
  QcPolygon(const VertexListType & vertexes);
  QcPolygon(const QVector<Type> & coordinates);
  QcPolygon(const QcPolygon & polygon);
  ~QcPolygon();

  bool contains(const QcVectorDouble & point) const;

  QcTiledPolygon intersec_with_grid(double grid_step) const;

  QcPolygon rotate_counter_clockwise(double angle) const;

  Type area() const;
};

class QC_EXPORT QcTiledPolygonRun
{
 public:
  QcTiledPolygonRun(int y, const QcIntervalInt & interval);

  inline int y() const { return m_y; }

  inline const QcIntervalInt & interval() const { return m_interval; }

  bool operator==(const QcTiledPolygonRun & other) const;

  inline bool intersect(const QcTiledPolygonRun & other) const {
    return m_y == other.m_y && m_interval.intersect(other.m_interval);
  }

  bool cut(const QcTiledPolygonRun & other,
	   QcIntervalInt & intersection, QcIntervalInt & left,  QcIntervalInt & right,
	   bool & exchanged) const;

  QVector<QcTileMatrixIndex> tile_indexes() const;

 private:
  int m_y;
  QcIntervalInt m_interval;
};

class QC_EXPORT QcTiledPolygon
{
 public:
  QcTiledPolygon(const QcPolygon & polygon, double grid_step);

  inline const QcPolygon & polygon() const { return m_polygon; }

  inline double grid_step() const { return m_grid_step; }

  inline const QList<QcTiledPolygonRun> runs() const { return m_runs; }

  QcTiledPolygonDiff diff(const QcTiledPolygon & polygon);

 private:
  const QcPolygon & m_polygon;
  double m_grid_step;
  QcTiledPolygonRunList m_runs;
};

class QC_EXPORT QcTiledPolygonDiff
{
 public:
  inline const QcTiledPolygonRunList & new_area() const { return m_new_area; }
  inline const QcTiledPolygonRunList & old_area() const { return m_old_area; }
  inline const QcTiledPolygonRunList & same_area() const { return m_same_area; }

  void add_new_area(const QcTiledPolygonRun & run) {
    m_new_area.push_back(run);
  }

  void add_old_area(const QcTiledPolygonRun & run) {
    m_old_area.push_back(run);
  }

  void add_same_area(const QcTiledPolygonRun & run) {
    m_same_area.push_back(run);
  }

 private:
  QcTiledPolygonRunList m_new_area;
  QcTiledPolygonRunList m_old_area;
  QcTiledPolygonRunList m_same_area;
};

/**************************************************************************************************/

// QT_END_NAMESPACE

/**************************************************************************************************/

#endif /* __POLYGON_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
