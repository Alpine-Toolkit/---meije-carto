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

#ifndef __PATH_H__
#define __PATH_H__

/**************************************************************************************************/

#include <cmath>

// #include <QtCore/QMetaType>
#include <QVector>
#include <QList>

#include "qtcarto_global.h"
#include "geometry/segment.h"
#include "geometry/vector.h"
#include "math/interval.h"
#include "math/qc_math.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

template <typename T, template<typename T> class Vector = QcVector>
class QC_EXPORT QcPath
{
 public:
  typedef T Type;
  typedef Vector<T> VertexType;
  typedef QList<VertexType> VertexListType;
  typedef QcSegment<T> EdgeType; // segment is 2D
  typedef QList<EdgeType> EdgeListType;
  typedef typename VertexType::IntervalType IntervalType;

 public:
  QcPath();
  // QcPath(int number_of_vertexes);
  QcPath(const VertexListType & vertexes, bool closed = false);
  QcPath(const QVector<T> & coordinates, bool closed = false);
  QcPath(const QcPath & path);
  ~QcPath();

  QcPath & operator=(const QcPath & other);

  bool operator==(const QcPath & other) const;
  inline bool operator!=(const QcPath & other) const {
    return !operator==(other);
  }

  void clear();
  void add_vertex(const VertexType & vertex);

  inline const VertexType & vertex_at(int i) const { return m_vertexes[i]; }
  // inline const EdgeType & edge(int i) const { return m_edges[i]; }

  inline int number_of_vertexes() const { return m_vertexes.size(); }
  inline const VertexListType & vertexes() const { return m_vertexes; }
  inline const IntervalType & interval() const { return m_interval; } // Fixme: any vertexes

  inline bool closed() const { return m_closed; };
  inline void set_closed(bool value) { m_closed = value; };

  T length() const;

  int number_of_edges() const;
  // EdgeType edge(int start_index) const;
  EdgeListType edges() const;
  EdgeType closing_edge() const { return EdgeType(m_vertexes.constLast(), m_vertexes.constFirst()); }

  VertexType barycenter() const;
  VertexType nearest_vertex(const VertexType & point, T & distance) const;
  EdgeType nearest_edge(const VertexType & point, T & distance, T & abscissa) const;

  bool is_self_intersecting() const;

 private:
  // Fixme: QVector
  VertexListType m_vertexes;
  EdgeListType m_edges;
  IntervalType m_interval;
  bool m_closed;
};

typedef QcPath<double> QcPathDouble;
template<> QcPathDouble::QcPath(const QVector<double> & coordinates, bool closed);

typedef QcPath<double, QcVector3D> QcPath3DDouble;
template<> QcPath3DDouble::QcPath(const QVector<double> & coordinates, bool closed);

/**************************************************************************************************/

#ifndef QC_MANUAL_INSTANTIATION
#include "path.hxx"
#endif

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __PATH_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
