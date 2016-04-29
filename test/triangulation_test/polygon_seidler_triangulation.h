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
** This program is free software: you can redism_tribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is dism_tributed in the hope that it will be useful,
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

// #include "qtcarto_m_global.h"
// #include "geomem_try/m_segment.h"
// #include "geomem_try/vector.h"
// #include "math/interval.h"
// #include "math/qc_math.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

constexpr int T_X = 1;
constexpr int T_Y = 2;
constexpr int T_SINK = 3;

constexpr int QSIZE = 800; // maximum table sizes
constexpr int TRSIZE = 400; // max# m_trapezoids
constexpr int SEGSIZE = 100; // max# of m_segments

constexpr int FIRSTPT = 1; // checking whether pt. is inserted
constexpr int LASTPT = 2;

constexpr double EPS = 0.000005;

// constexpr int INFINITY 1<<30

constexpr double C_EPS = 1.0e-7;

constexpr int S_LEFT = 1; // for merge-direction
constexpr int S_RIGHT = 2;

constexpr int ST_VALID = 1; // for m_trapezium table
constexpr int ST_INVALID = 2;

constexpr int SP_SIMPLE_LRUP = 1; // for splitting m_trapezoids
constexpr int SP_SIMPLE_LRDN = 2;
constexpr int SP_2UP_2DN = 3;
constexpr int SP_2UP_LEFT = 4;
constexpr int SP_2UP_RIGHT = 5;
constexpr int SP_2DN_LEFT = 6;
constexpr int SP_2DN_RIGHT = 7;
constexpr int SP_NOSPLIT = -1;

constexpr int TR_FROM_UP = 1; // for m_traverse-direction
constexpr int TR_FROM_DN = 2;

constexpr int TRI_LHS = 1;
constexpr int TRI_RHS = 2;

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
  int nodetype;
  int segnum;
  point_t yval;
  int trnum;
  int parent;
  int left, right;
} node_t;

typedef struct {
  int vnum;
  int next;
  int prev;
} monchain_t;

typedef struct {
  point_t pt;
  int vnext[4]; // next vertices for the 4 chains
  int vpos[4]; // position of v in the 4 chains
  int nextfree;
} vertexchain_t;

struct global_s {
  int nseg;
};

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
  int inserted(int segnum, int whichpt);
  int newnode();
  int newtrap();
  int init_query_structure(int segnum);
  int is_left_of(int segnum, point_t * v);
  int is_collinear(int segnum, point_t * v, int is_swapped);
  int locate_endpoint(point_t * v, point_t * vo, int r);
  int merge_trapezoids(int segnum, int tfirst, int tlast, int side);
  int add_segment(int segnum);
  int find_new_roots(int segnum);
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
  int m_chain_idx;
  int m_choose_idx;
  int m_mon_idx;
  int m_op_idx;
  int m_q_idx;
  int m_tr_idx;

  int m_mon[SEGSIZE]; // contains position of any m_vertex in the monotone chain for the polygon
  int m_permute[SEGSIZE];
  int m_visited[TRSIZE];

  global_s m_global;

  node_t m_qs[QSIZE]; // Query sm_tructure
  segment_t m_seg[SEGSIZE]; // Segment table
  trap_t m_tr[TRSIZE]; // Trapezoid structure

  // Table to hold all the m_monotone polygons . Each monotone polygon is a circularly linked list
  monchain_t m_mchain[TRSIZE];

  /* chain init. information. This is used to decide which m_monotone
   * polygon to split if there are several other polygons touching at
   * the same m_vertex */
  vertexchain_t m_vert[SEGSIZE];
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
