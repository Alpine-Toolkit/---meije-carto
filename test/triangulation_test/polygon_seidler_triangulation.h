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

#ifndef __POLYGON_SEIDLER_TRIANGULATION_H__
#define __POLYGON_SEIDLER_TRIANGULATION_H__

/**************************************************************************************************/

// #include <cmath>

// #include <QtCore/QMetaType>
// #include <QVector>
// #include <QList>

// #include "qtcarto_global.h"
// #include "geometry/segment.h"
// #include "geometry/vector.h"
// #include "math/interval.h"
// #include "math/qc_math.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

constexpr int QSIZE = 800; // maximum table sizes
constexpr int TRSIZE = 400; // max# trapezoids
constexpr int SEGSIZE = 100; // max# of segments

/**************************************************************************************************/

typedef struct {
  double x, y;
} point_t, vector_t;

typedef struct {
  point_t v0, v1;
  int is_inserted;
  int root0, root1;
} segment_t;

typedef struct {
  int lseg, rseg;
  point_t hi, lo;
  int u0, u1;
  int d0, d1;
  int sink;
  int usave, uside;
  int state;
} trap_t;

typedef struct {
  int node_type;
  int segment_number;
  point_t yval;
  int trnum;
  int parent;
  int left, right;
} node_t;

typedef struct {
  int vnum;
  int next;
  int prev;
} monotone_chain_t;

typedef struct {
  point_t pt;
  int vnext[4]; // next vertices for the 4 chains
  int vpos[4]; // position of v in the 4 chains
  int nextfree;
} vertex_chain_t;

/**************************************************************************************************/

// QC_EXPORT
class QcSeidlerPolygonTriangulation
{
public:
  QcSeidlerPolygonTriangulation(int n, double vertices[][2], int triangles[][3]);
  // ~QcSeidlerPolygonTriangulation();

private:
  int generate_random_ordering(int n);
  int choose_segment();
  int inserted(int segment_number, int whichpt);
  int newnode();
  int new_trapezoid();
  int init_query_structure(int segment_number);
  int is_left_of(int segment_number, point_t * v);
  int is_collinear(int segment_number, point_t * v, int is_swapped);
  int locate_endpoint(point_t * v, point_t * vo, int r);
  int merge_trapezoids(int segment_number, int tfirst, int tlast, int side);
  int add_segment(int segment_number);
  int find_new_roots(int segment_number);
  void construct_trapezoids(int nseg, segment_t seg[]);
  int inside_polygon(trap_t * t);
  int newmon();
  int new_chain_element();
  int get_vertex_positions(int v0, int v1, int * ip, int * iq);
  int make_new_monotone_poly(int mcur, int v0, int v1);
  int traverse_polygon(int mcur, int trnum, int from, int dir);
  int monotonate_trapezoids(int n);
  int triangulate_single_polygon(int posmax, int side, int op[][3]);
  void triangulate_monotone_polygons(int nmonpoly, int op[][3]);
  int initialise(int nseg);

private:
  int m_number_of_segments;

  int m_chain_index;
  int m_choose_index;
  int m_mon_index;
  int m_op_index;
  int m_q_index;
  int m_trapezoid_index;

  int m_mon[SEGSIZE]; // contains position of any vertex in the monotone chain for the polygon
  int m_permute[SEGSIZE];
  int m_visited[TRSIZE];

  node_t m_query[QSIZE]; // Query structure
  segment_t m_segment[SEGSIZE]; // Segment table
  trap_t m_trapezoid[TRSIZE]; // Trapezoid structure

  // Table to hold all the m_monotone polygons . Each monotone polygon is a circularly linked list
  monotone_chain_t m_monotone_chain[TRSIZE];

  /* chain init. information. This is used to decide which m_monotone
   * polygon to split if there are several other polygons touching at
   * the same vertex */
  vertex_chain_t m_vertex[SEGSIZE];
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __POLYGON_SEIDLER_TRIANGULATION_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
