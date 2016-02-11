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
#include "math/qc_math.h"

// #include <QtCore/QMetaType>
#include <QVector>
#include <QList>

#include "qtcarto_global.h"
#include "interval.h"
#include "vector.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcTiledPolygon;

class QC_EXPORT QcPolygon
{
 public:
  QcPolygon();
  // QcPolygon(size_t number_of_vertexes);
  QcPolygon(const QList<QcVectorDouble> & vertexes);
  QcPolygon(const QVector<double> & coordinates);
  QcPolygon(const QcPolygon & polygon);
  ~QcPolygon();

  void add_vertex(const QcVectorDouble & vertex);

  inline const QList<QcVectorDouble> & vertexes() const {
    return m_vertexes;
  }

  inline const QcInterval2DDouble & interval() const {
    return m_interval;
  }

  bool contains(const QcVectorDouble & point) const;

  QcTiledPolygon intersec_with_grid(double grid_step) const;

  QcPolygon rotate_counter_clockwise(double angle) const;

 private:
  // Fixme: QVector
  QList<QcVectorDouble> m_vertexes;
  QcInterval2DDouble m_interval;
};

class QC_EXPORT QcTiledPolygonRun
{
 public:
  QcTiledPolygonRun(int y, const QcIntervalInt & interval);

  inline int y() const {
    return m_y;
  }

  inline const QcIntervalInt & interval() const {
    return m_interval;
  }

  bool operator==(const QcTiledPolygonRun & other) const;

 private:
  int m_y;
  QcIntervalInt m_interval;
};

class QC_EXPORT QcTiledPolygon
{
 public:
  QcTiledPolygon(const QcPolygon & polygon, double grid_step);

  inline const QcPolygon & polygon() const {
    return m_polygon;
  }

  inline double grid_step() const {
    return m_grid_step;
  }

  inline const QList<QcTiledPolygonRun> runs() const {
    return m_runs;
  }

 private:
  const QcPolygon & m_polygon;
  double m_grid_step;
  QList<QcTiledPolygonRun> m_runs;
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
