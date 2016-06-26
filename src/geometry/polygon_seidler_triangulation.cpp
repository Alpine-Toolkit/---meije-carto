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

#include "polygon_seidler_triangulation.h"

/**************************************************************************************************/

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring> // memset

/**************************************************************************************************/

#define DEBUG 1
// #define CLOCK
// #define STANDALONE
// #define SIMPLE

/**************************************************************************************************/

constexpr int T_X = 1;
constexpr int T_Y = 2;
constexpr int T_SINK = 3;

constexpr int FIRSTPT = 1; // checking whether pt. is inserted
constexpr int LASTPT = 2;

constexpr double EPS = 0.000005;

// constexpr int INFINITY 1<<30

constexpr double C_EPS = 1.0e-7;

constexpr int S_LEFT = 1; // for merge-direction
constexpr int S_RIGHT = 2;

constexpr int ST_VALID = 1; // for trapezium table
constexpr int ST_INVALID = 2;

constexpr int SP_SIMPLE_LRUP = 1; // for splitting trapezoids
constexpr int SP_SIMPLE_LRDN = 2;
constexpr int SP_2UP_2DN = 3;
constexpr int SP_2UP_LEFT = 4;
constexpr int SP_2UP_RIGHT = 5;
constexpr int SP_2DN_LEFT = 6;
constexpr int SP_2DN_RIGHT = 7;
constexpr int SP_NOSPLIT = -1;

constexpr int TR_FROM_UP = 1; // for traverse-direction
constexpr int TR_FROM_DN = 2;

constexpr int TRI_LHS = 1;
constexpr int TRI_RHS = 2;

/**************************************************************************************************/

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define CROSS(v0, v1, v2) (((v1).x - (v0).x)*((v2).y - (v0).y) -        \
                           ((v1).y - (v0).y)*((v2).x - (v0).x))

#define DOT(v0, v1) ((v0).x * (v1).x + (v0).y * (v1).y)

#define MODULO_NEXT(v0, n) (((v0) - 1) % (n) + 1)

#define FP_EQUAL(s, t) (fabs(s - t) <= C_EPS)

#define CROSS_SINE(v0, v1) ((v0).x * (v1).y - (v1).x * (v0).y)
#define LENGTH(v0) (sqrt((v0).x * (v0).x + (v0).y * (v0).y))

/**************************************************************************************************/

// Get log*n for given n
int
math_logstar_n(int n)
{
  int i;
  double v;

  for (i = 0, v = (double) n; v >= 1; i++)
    v = log2(v);

  return (i - 1);
}

int
math_N(int n, int h)
{
  int i;
  double v;

  for (i = 0, v = (int) n; i < h; i++)
    v = log2(v);

  return (int) ceil((double) 1.0 * n / v);
}

/**************************************************************************************************/

/* Return the maximum of the two points into the yval structure */
int
_max(point_t * yval, point_t * v0, point_t * v1)
{
  if (v0->y > v1->y + C_EPS)
    *yval = *v0;
  else if (FP_EQUAL(v0->y, v1->y)) {
    if (v0->x > v1->x + C_EPS)
      *yval = *v0;
    else
      *yval = *v1;
  } else
    *yval = *v1;

  return 0;
}

/* Return the minimum of the two points into the yval structure */
int
_min(point_t * yval, point_t * v0, point_t * v1)
{
  if (v0->y < v1->y - C_EPS)
    *yval = *v0;
  else if (FP_EQUAL(v0->y, v1->y)) {
    if (v0->x < v1->x)
      *yval = *v0;
    else
      *yval = *v1;
  } else
    *yval = *v1;

  return 0;
}

int
_greater_than(point_t * v0, point_t * v1)
{
  if (v0->y > v1->y + C_EPS)
    return true;
  else if (v0->y < v1->y - C_EPS)
    return false;
  else
    return (v0->x > v1->x);
}

int
_equal_to(point_t * v0, point_t * v1)
{
  return (FP_EQUAL(v0->y, v1->y) && FP_EQUAL(v0->x, v1->x));
}

int
_greater_than_equal_to(point_t * v0, point_t * v1)
{
  if (v0->y > v1->y + C_EPS)
    return true;
  else if (v0->y < v1->y - C_EPS)
    return false;
  else
    return (v0->x >= v1->x);
}

int
_less_than(point_t * v0, point_t * v1)
{
  if (v0->y < v1->y - C_EPS)
    return true;
  else if (v0->y > v1->y + C_EPS)
    return false;
  else
    return (v0->x < v1->x);
}

double
get_angle(point_t * vp0, point_t * vpnext, point_t * vp1)
{
  point_t v0, v1;

  v0.x = vpnext->x - vp0->x;
  v0.y = vpnext->y - vp0->y;

  v1.x = vp1->x - vp0->x;
  v1.y = vp1->y - vp0->y;

  if (CROSS_SINE(v0, v1) >= 0) /* sine is positive */
    return DOT(v0, v1) / LENGTH(v0) / LENGTH(v1);
  else
    return (-1.0 * DOT(v0, v1) / LENGTH(v0) / LENGTH(v1) - 2);
}

/**************************************************************************************************/

/* Generate a random permutation of the segments 1..n
 */
int
QcSeidlerPolygonTriangulation::generate_random_ordering(int n)
{
  register int i;
  int m, st[SEGSIZE], *p;

  m_choose_index = 1;

  for (i = 0; i <= n; i++)
    st[i] = i;

  p = st;
  for (i = 1; i <= n; i++, p++) {
    m = lrand48() % (n + 1 - i) + 1;
    m_permute[i] = p[m];
    if (m != 1)
      p[m] = p[1];
  }
  return 0;
}

/* Return the next segment in the generated random ordering of all the segments in S */
int
QcSeidlerPolygonTriangulation::choose_segment()
{
#ifdef DEBUG
  fprintf(stderr, "choose_segment: %d\n", m_permute[m_choose_index]);
#endif

  return m_permute[m_choose_index++];
}

int
QcSeidlerPolygonTriangulation::inserted(int segment_number, int whichpt)
{
  int n1, n2;

  n1 = segment_number % m_number_of_segments + 1; /* next seg. */
  n2 = (segment_number - 1 + m_number_of_segments - 1) % m_number_of_segments + 1; /* prev. */

  if (whichpt == FIRSTPT)
    return m_segment[n2].is_inserted;
  else
    return m_segment[n1].is_inserted;
}

/**************************************************************************************************/

/* Return a new node to be added into the query tree */
int
QcSeidlerPolygonTriangulation::newnode()
{
  if (m_q_index < QSIZE)
    return m_q_index++;
  else {
    fprintf(stderr, "newnode: Query-table overflow\n");
    return -1;
  }
}

/* Return a free trapezoid */
int
QcSeidlerPolygonTriangulation::new_trapezoid()
{
   if (m_trapezoid_index < TRSIZE) {
    m_trapezoid[m_trapezoid_index].lseg = -1;
    m_trapezoid[m_trapezoid_index].rseg = -1;
    m_trapezoid[m_trapezoid_index].state = ST_VALID;
    return m_trapezoid_index++;
  } else {
    fprintf(stderr, "new_trapezoid: Trapezoid-table overflow\n");
    return -1;
  }
}

/* Initilialise the query structure (Q) and the trapezoid table (T)
 * when the first segment is added to start the trapezoidation
 */
int
QcSeidlerPolygonTriangulation::init_query_structure(int segment_number)
{
  int i1, i2, i3, i4, i5, i6, i7, root;
  int t1, t2, t3, t4;
  segment_t *s = &m_segment[segment_number];

  memset((void *) m_trapezoid, 0, sizeof(m_trapezoid));
  memset((void *) m_query, 0, sizeof(m_query));

  i1 = newnode();
  m_query[i1].node_type = T_Y;
  _max(&m_query[i1].yval, &s->v0, &s->v1); /* root */
  root = i1;

  m_query[i1].right = i2 = newnode();
  m_query[i2].node_type = T_SINK;
  m_query[i2].parent = i1;

  m_query[i1].left = i3 = newnode();
  m_query[i3].node_type = T_Y;
  _min(&m_query[i3].yval, &s->v0, &s->v1); /* root */
  m_query[i3].parent = i1;

  m_query[i3].left = i4 = newnode();
  m_query[i4].node_type = T_SINK;
  m_query[i4].parent = i3;

  m_query[i3].right = i5 = newnode();
  m_query[i5].node_type = T_X;
  m_query[i5].segment_number = segment_number;
  m_query[i5].parent = i3;

  m_query[i5].left = i6 = newnode();
  m_query[i6].node_type = T_SINK;
  m_query[i6].parent = i5;

  m_query[i5].right = i7 = newnode();
  m_query[i7].node_type = T_SINK;
  m_query[i7].parent = i5;

  t1 = new_trapezoid(); /* middle left */
  t2 = new_trapezoid(); /* middle right */
  t3 = new_trapezoid(); /* bottom-most */
  t4 = new_trapezoid(); /* topmost */

  m_trapezoid[t1].hi = m_trapezoid[t2].hi = m_trapezoid[t4].lo = m_query[i1].yval;
  m_trapezoid[t1].lo = m_trapezoid[t2].lo = m_trapezoid[t3].hi = m_query[i3].yval;
  m_trapezoid[t4].hi.y = (double) (INFINITY);
  m_trapezoid[t4].hi.x = (double) (INFINITY);
  m_trapezoid[t3].lo.y = (double) -1 * (INFINITY);
  m_trapezoid[t3].lo.x = (double) -1 * (INFINITY);
  m_trapezoid[t1].rseg = m_trapezoid[t2].lseg = segment_number;
  m_trapezoid[t1].u0 = m_trapezoid[t2].u0 = t4;
  m_trapezoid[t1].d0 = m_trapezoid[t2].d0 = t3;
  m_trapezoid[t4].d0 = m_trapezoid[t3].u0 = t1;
  m_trapezoid[t4].d1 = m_trapezoid[t3].u1 = t2;

  m_trapezoid[t1].sink = i6;
  m_trapezoid[t2].sink = i7;
  m_trapezoid[t3].sink = i4;
  m_trapezoid[t4].sink = i2;

  m_trapezoid[t1].state = m_trapezoid[t2].state = ST_VALID;
  m_trapezoid[t3].state = m_trapezoid[t4].state = ST_VALID;

  m_query[i2].trnum = t4;
  m_query[i4].trnum = t3;
  m_query[i6].trnum = t1;
  m_query[i7].trnum = t2;

  s->is_inserted = true;
  return root;
}

/* Retun true if the vertex v is to the left of line segment no.
 * segment_number
 */
int
QcSeidlerPolygonTriangulation::is_left_of(int segment_number, point_t * v)
{
  segment_t *s = &m_segment[segment_number];
  double area;

  if (_greater_than(&s->v1, &s->v0)) { /* seg. going upwards */
    if (FP_EQUAL(s->v1.y, v->y)) {
      if (v->x < s->v1.x)
        area = 1.0;
      else
        area = -1.0;
    } else if (FP_EQUAL(s->v0.y, v->y)) {
      if (v->x < s->v0.x)
        area = 1.0;
      else
        area = -1.0;
    } else
      area = CROSS(s->v0, s->v1, (*v));
  } else { /* v0 > v1 */
    if (FP_EQUAL(s->v1.y, v->y)) {
      if (v->x < s->v1.x)
        area = 1.0;
      else
        area = -1.0;
    } else if (FP_EQUAL(s->v0.y, v->y)) {
      if (v->x < s->v0.x)
        area = 1.0;
      else
        area = -1.0;
    } else
      area = CROSS(s->v1, s->v0, (*v));
  }

  if (area > 0.0)
    return true;
  else
    return false;
}

int
QcSeidlerPolygonTriangulation::is_collinear(int segment_number, point_t * v, int is_swapped)
{
  int n;

  /* First check if the endpoint is already inserted */
  if (!is_swapped)
    n = MODULO_NEXT(segment_number + 1, m_number_of_segments);
  else if ((n = segment_number - 1) == 0)
    n = 1;

  return m_segment[n].is_inserted;
}

/* This is query routine which determines which trapezoid does the
 * point v lie in. The return value is the trapezoid number
 */
int
QcSeidlerPolygonTriangulation::locate_endpoint(point_t * v, point_t * vo, int r)
{
  node_t *rptr = &m_query[r];

  switch (rptr->node_type) {
  case T_SINK:
    return rptr->trnum;

  case T_Y:
    if (_greater_than(v, &rptr->yval)) /* above */
      return locate_endpoint(v, vo, rptr->right);
    else if (_equal_to(v, &rptr->yval)) { /* the point is already */
      /* inserted. */
      if (_greater_than(vo, &rptr->yval)) /* above */
        return locate_endpoint(v, vo, rptr->right);
      else
        return locate_endpoint(v, vo, rptr->left); /* below */
    } else
      return locate_endpoint(v, vo, rptr->left); /* below */

  case T_X:
    if (_equal_to(v, &m_segment[rptr->segment_number].v0) || _equal_to(v, &m_segment[rptr->segment_number].v1)) {
      if (FP_EQUAL(v->y, vo->y)) {  /* horizontal segment */

        if (vo->x < v->x)
          return locate_endpoint(v, vo, rptr->left); /* left */
        else
          return locate_endpoint(v, vo, rptr->right); /* right */
      }

      else if (is_left_of(rptr->segment_number, vo))
        return locate_endpoint(v, vo, rptr->left); /* left */
      else
        return locate_endpoint(v, vo, rptr->right); /* right */
    }
    else if (is_left_of(rptr->segment_number, v))
      return locate_endpoint(v, vo, rptr->left); /* left */
    else
      return locate_endpoint(v, vo, rptr->right); /* right */

  default:
    fprintf(stderr, "Haggu !!!!!\n");
    return 0;
  }
}

/* Thread in the segment into the existing trapezoidation. The
 * limiting trapezoids are given by tfirst and tlast (which are the
 * trapezoids containing the two endpoints of the segment
 */
int
QcSeidlerPolygonTriangulation::merge_trapezoids(int segment_number, int tfirst, int tlast, int side)
{
  int t, tnext, cond;
  int ptnext;

  /* First merge polys on the LHS */
  t = tfirst;
  while ((t > 0) && _greater_than_equal_to(&m_trapezoid[t].lo, &m_trapezoid[tlast].lo)) {
    if (side == S_LEFT)
      cond = ((((tnext = m_trapezoid[t].d0) > 0) && (m_trapezoid[tnext].rseg == segment_number)) ||
              (((tnext = m_trapezoid[t].d1) > 0) && (m_trapezoid[tnext].rseg == segment_number)));
    else
      cond = ((((tnext = m_trapezoid[t].d0) > 0) && (m_trapezoid[tnext].lseg == segment_number)) ||
              (((tnext = m_trapezoid[t].d1) > 0) && (m_trapezoid[tnext].lseg == segment_number)));

    if (cond) {
      if ((m_trapezoid[t].lseg == m_trapezoid[tnext].lseg) && (m_trapezoid[t].rseg == m_trapezoid[tnext].rseg)) { /* good neighbours */
        /* merge them */
        /* Use the upper node as the new node i.e. t */

        ptnext = m_query[m_trapezoid[tnext].sink].parent;

        if (m_query[ptnext].left == m_trapezoid[tnext].sink)
          m_query[ptnext].left = m_trapezoid[t].sink;
        else
          m_query[ptnext].right = m_trapezoid[t].sink; /* redirect parent */

        /* Change the upper neighbours of the lower trapezoids */

        if ((m_trapezoid[t].d0 = m_trapezoid[tnext].d0) > 0) {
          if (m_trapezoid[m_trapezoid[t].d0].u0 == tnext)
            m_trapezoid[m_trapezoid[t].d0].u0 = t;
          else if (m_trapezoid[m_trapezoid[t].d0].u1 == tnext)
            m_trapezoid[m_trapezoid[t].d0].u1 = t;
        }

        if ((m_trapezoid[t].d1 = m_trapezoid[tnext].d1) > 0) {
          if (m_trapezoid[m_trapezoid[t].d1].u0 == tnext)
            m_trapezoid[m_trapezoid[t].d1].u0 = t;
          else if (m_trapezoid[m_trapezoid[t].d1].u1 == tnext)
            m_trapezoid[m_trapezoid[t].d1].u1 = t;
        }

        m_trapezoid[t].lo = m_trapezoid[tnext].lo;
        m_trapezoid[tnext].state = ST_INVALID; /* invalidate the lower */
        /* trapezium */
      } else /* not good neighbours */
        t = tnext;
    } else /* do not satisfy the outer if */
      t = tnext;

  } /* end-while */

  return 0;
}

/* Add in the new segment into the trapezoidation and update Q and T structures
 */
int
QcSeidlerPolygonTriangulation::add_segment(int segment_number)
{
  segment_t s;
  int tu, tl, sk, tfirst, tlast, tnext;
  int tfirstr, tlastr, tfirstl, tlastl;
  int i1, i2, t, tn;
  point_t vper;
  point_t tpt;
  int tritop = 0;
  int tribot = 0, is_swapped = 0;
  int tmp_trim_segment;

  s = m_segment[segment_number];
  if (_greater_than(&s.v1, &s.v0)) { /* Get higher vertex in v0 */
    int tmp;
    tpt = s.v0;
    s.v0 = s.v1;
    s.v1 = tpt;
    tmp = s.root0;
    s.root0 = s.root1;
    s.root1 = tmp;
    is_swapped = true;
  }

  if ((is_swapped) ? !inserted(segment_number, LASTPT) : !inserted(segment_number, FIRSTPT)) { /* insert v0 in the tree */
    int tmp_d;

    tu = locate_endpoint(&s.v0, &s.v1, s.root0);
    tl = new_trapezoid(); /* tl is the new lower trapezoid */
    m_trapezoid[tl].state = ST_VALID;
    m_trapezoid[tl] = m_trapezoid[tu];
    m_trapezoid[tu].lo.y = m_trapezoid[tl].hi.y = s.v0.y;
    m_trapezoid[tu].lo.x = m_trapezoid[tl].hi.x = s.v0.x;
    m_trapezoid[tu].d0 = tl;
    m_trapezoid[tu].d1 = 0;
    m_trapezoid[tl].u0 = tu;
    m_trapezoid[tl].u1 = 0;

    if (((tmp_d = m_trapezoid[tl].d0) > 0) && (m_trapezoid[tmp_d].u0 == tu))
      m_trapezoid[tmp_d].u0 = tl;
    if (((tmp_d = m_trapezoid[tl].d0) > 0) && (m_trapezoid[tmp_d].u1 == tu))
      m_trapezoid[tmp_d].u1 = tl;

    if (((tmp_d = m_trapezoid[tl].d1) > 0) && (m_trapezoid[tmp_d].u0 == tu))
      m_trapezoid[tmp_d].u0 = tl;
    if (((tmp_d = m_trapezoid[tl].d1) > 0) && (m_trapezoid[tmp_d].u1 == tu))
      m_trapezoid[tmp_d].u1 = tl;

    /* Now update the query structure and obtain the sinks for the */
    /* two trapezoids */

    i1 = newnode(); /* Upper trapezoid sink */
    i2 = newnode(); /* Lower trapezoid sink */
    sk = m_trapezoid[tu].sink;

    m_query[sk].node_type = T_Y;
    m_query[sk].yval = s.v0;
    m_query[sk].segment_number = segment_number; /* not really reqd ... maybe later */
    m_query[sk].left = i2;
    m_query[sk].right = i1;

    m_query[i1].node_type = T_SINK;
    m_query[i1].trnum = tu;
    m_query[i1].parent = sk;

    m_query[i2].node_type = T_SINK;
    m_query[i2].trnum = tl;
    m_query[i2].parent = sk;

    m_trapezoid[tu].sink = i1;
    m_trapezoid[tl].sink = i2;
    tfirst = tl;
  }
  else { /* v0 already present */
    /* Get the topmost intersecting trapezoid */
    vper.x = s.v0.x + EPS * (s.v1.x - s.v0.x);
    vper.y = s.v0.y + EPS * (s.v1.y - s.v0.y);
    tfirst = locate_endpoint(&s.v0, &s.v1, s.root0);
    tritop = 1;
  }

  if ((is_swapped) ? !inserted(segment_number, FIRSTPT) : !inserted(segment_number, LASTPT)) { /* insert v1 in the tree */
    int tmp_d;

    tu = locate_endpoint(&s.v1, &s.v0, s.root1);

    tl = new_trapezoid(); /* tl is the new lower trapezoid */
    m_trapezoid[tl].state = ST_VALID;
    m_trapezoid[tl] = m_trapezoid[tu];
    m_trapezoid[tu].lo.y = m_trapezoid[tl].hi.y = s.v1.y;
    m_trapezoid[tu].lo.x = m_trapezoid[tl].hi.x = s.v1.x;
    m_trapezoid[tu].d0 = tl;
    m_trapezoid[tu].d1 = 0;
    m_trapezoid[tl].u0 = tu;
    m_trapezoid[tl].u1 = 0;

    if (((tmp_d = m_trapezoid[tl].d0) > 0) && (m_trapezoid[tmp_d].u0 == tu))
      m_trapezoid[tmp_d].u0 = tl;
    if (((tmp_d = m_trapezoid[tl].d0) > 0) && (m_trapezoid[tmp_d].u1 == tu))
      m_trapezoid[tmp_d].u1 = tl;

    if (((tmp_d = m_trapezoid[tl].d1) > 0) && (m_trapezoid[tmp_d].u0 == tu))
      m_trapezoid[tmp_d].u0 = tl;
    if (((tmp_d = m_trapezoid[tl].d1) > 0) && (m_trapezoid[tmp_d].u1 == tu))
      m_trapezoid[tmp_d].u1 = tl;

    /* Now update the query structure and obtain the sinks for the */
    /* two trapezoids */

    i1 = newnode(); /* Upper trapezoid sink */
    i2 = newnode(); /* Lower trapezoid sink */
    sk = m_trapezoid[tu].sink;

    m_query[sk].node_type = T_Y;
    m_query[sk].yval = s.v1;
    m_query[sk].segment_number = segment_number; /* not really reqd ... maybe later */
    m_query[sk].left = i2;
    m_query[sk].right = i1;

    m_query[i1].node_type = T_SINK;
    m_query[i1].trnum = tu;
    m_query[i1].parent = sk;

    m_query[i2].node_type = T_SINK;
    m_query[i2].trnum = tl;
    m_query[i2].parent = sk;

    m_trapezoid[tu].sink = i1;
    m_trapezoid[tl].sink = i2;
    tlast = tu;
  } else { /* v1 already present */
    /* Get the lowermost intersecting trapezoid */
    vper.x = s.v1.x + EPS * (s.v0.x - s.v1.x);
    vper.y = s.v1.y + EPS * (s.v0.y - s.v1.y);
    tlast = locate_endpoint(&s.v1, &s.v0, s.root1);
    tribot = 1;
  }

  /* Thread the segment into the query tree creating a new X-node */
  /* First, split all the trapezoids which are intersected by s into */
  /* two */

  t = tfirst; /* topmost trapezoid */

  while ((t > 0) && _greater_than_equal_to(&m_trapezoid[t].lo, &m_trapezoid[tlast].lo)) {
    /* traverse from top to bot */
    int t_sav, tn_sav;
    sk = m_trapezoid[t].sink;
    i1 = newnode(); /* left trapezoid sink */
    i2 = newnode(); /* right trapezoid sink */

    m_query[sk].node_type = T_X;
    m_query[sk].segment_number = segment_number;
    m_query[sk].left = i1;
    m_query[sk].right = i2;

    m_query[i1].node_type = T_SINK; /* left trapezoid (use existing one) */
    m_query[i1].trnum = t;
    m_query[i1].parent = sk;

    m_query[i2].node_type = T_SINK; /* right trapezoid (allocate new) */
    m_query[i2].trnum = tn = new_trapezoid();
    m_trapezoid[tn].state = ST_VALID;
    m_query[i2].parent = sk;

    if (t == tfirst)
      tfirstr = tn;
    if (_equal_to(&m_trapezoid[t].lo, &m_trapezoid[tlast].lo))
      tlastr = tn;

    m_trapezoid[tn] = m_trapezoid[t];
    m_trapezoid[t].sink = i1;
    m_trapezoid[tn].sink = i2;
    t_sav = t;
    tn_sav = tn;

    /* error */

    if ((m_trapezoid[t].d0 <= 0) && (m_trapezoid[t].d1 <= 0)) { /* case cannot arise */
      fprintf(stderr, "add_segment: error\n");
      break;
    }

    /* only one trapezoid below. partition t into two and make the */
    /* two resulting trapezoids t and tn as the upper neighbours of */
    /* the sole lower trapezoid */

    else if ((m_trapezoid[t].d0 > 0) && (m_trapezoid[t].d1 <= 0)) {
      /* Only one trapezoid below */
      if ((m_trapezoid[t].u0 > 0) && (m_trapezoid[t].u1 > 0)) {
        /* continuation of a chain from abv. */
        if (m_trapezoid[t].usave > 0) { /* three upper neighbours */
          if (m_trapezoid[t].uside == S_LEFT) {
            m_trapezoid[tn].u0 = m_trapezoid[t].u1;
            m_trapezoid[t].u1 = -1;
            m_trapezoid[tn].u1 = m_trapezoid[t].usave;

            m_trapezoid[m_trapezoid[t].u0].d0 = t;
            m_trapezoid[m_trapezoid[tn].u0].d0 = tn;
            m_trapezoid[m_trapezoid[tn].u1].d0 = tn;
          } else { /* intersects in the right */
            m_trapezoid[tn].u1 = -1;
            m_trapezoid[tn].u0 = m_trapezoid[t].u1;
            m_trapezoid[t].u1 = m_trapezoid[t].u0;
            m_trapezoid[t].u0 = m_trapezoid[t].usave;

            m_trapezoid[m_trapezoid[t].u0].d0 = t;
            m_trapezoid[m_trapezoid[t].u1].d0 = t;
            m_trapezoid[m_trapezoid[tn].u0].d0 = tn;
          }

          m_trapezoid[t].usave = m_trapezoid[tn].usave = 0;
        } else { /* No usave.... simple case */
          m_trapezoid[tn].u0 = m_trapezoid[t].u1;
          m_trapezoid[t].u1 = m_trapezoid[tn].u1 = -1;
          m_trapezoid[m_trapezoid[tn].u0].d0 = tn;
        }
      } else { /* fresh m_segment. or upward cusp */
        int tmp_u = m_trapezoid[t].u0;
        int td0, td1;
        if (((td0 = m_trapezoid[tmp_u].d0) > 0) && ((td1 = m_trapezoid[tmp_u].d1) > 0)) { /* upward cusp */
          if ((m_trapezoid[td0].rseg > 0) && !is_left_of(m_trapezoid[td0].rseg, &s.v1)) {
            m_trapezoid[t].u0 = m_trapezoid[t].u1 = m_trapezoid[tn].u1 = -1;
            m_trapezoid[m_trapezoid[tn].u0].d1 = tn;
          } else { /* cusp going leftwards */
            m_trapezoid[tn].u0 = m_trapezoid[tn].u1 = m_trapezoid[t].u1 = -1;
            m_trapezoid[m_trapezoid[t].u0].d0 = t;
          }
        } else { /* fresh segment */
          m_trapezoid[m_trapezoid[t].u0].d0 = t;
          m_trapezoid[m_trapezoid[t].u0].d1 = tn;
        }
      }

      if (FP_EQUAL(m_trapezoid[t].lo.y, m_trapezoid[tlast].lo.y) &&
          FP_EQUAL(m_trapezoid[t].lo.x, m_trapezoid[tlast].lo.x) && tribot) { /* bottom forms a triangle */

        if (is_swapped) {
          tmp_trim_segment = segment_number - 1;
          if (tmp_trim_segment == 0)
            tmp_trim_segment = m_number_of_segments;
        } else
          tmp_trim_segment = MODULO_NEXT(segment_number + 1, m_number_of_segments);

        if ((tmp_trim_segment > 0) && is_left_of(tmp_trim_segment, &s.v0)) {
          /* L-R downward cusp */
          m_trapezoid[m_trapezoid[t].d0].u0 = t;
          m_trapezoid[tn].d0 = m_trapezoid[tn].d1 = -1;
        } else {
          /* R-L downward cusp */
          m_trapezoid[m_trapezoid[tn].d0].u1 = tn;
          m_trapezoid[t].d0 = m_trapezoid[t].d1 = -1;
        }
      } else {
        if ((m_trapezoid[m_trapezoid[t].d0].u0 > 0) && (m_trapezoid[m_trapezoid[t].d0].u1 > 0)) {
          if (m_trapezoid[m_trapezoid[t].d0].u0 == t) { /* passes thru LHS */
            m_trapezoid[m_trapezoid[t].d0].usave = m_trapezoid[m_trapezoid[t].d0].u1;
            m_trapezoid[m_trapezoid[t].d0].uside = S_LEFT;
          } else {
            m_trapezoid[m_trapezoid[t].d0].usave = m_trapezoid[m_trapezoid[t].d0].u0;
            m_trapezoid[m_trapezoid[t].d0].uside = S_RIGHT;
          }
        }
        m_trapezoid[m_trapezoid[t].d0].u0 = t;
        m_trapezoid[m_trapezoid[t].d0].u1 = tn;
      }

      t = m_trapezoid[t].d0;
    } else if ((m_trapezoid[t].d0 <= 0) && (m_trapezoid[t].d1 > 0)) { /* Only one trapezoid below */
      if ((m_trapezoid[t].u0 > 0) && (m_trapezoid[t].u1 > 0)) { /* continuation of a chain from abv. */
        if (m_trapezoid[t].usave > 0) { /* three upper neighbours */
          if (m_trapezoid[t].uside == S_LEFT) {
            m_trapezoid[tn].u0 = m_trapezoid[t].u1;
            m_trapezoid[t].u1 = -1;
            m_trapezoid[tn].u1 = m_trapezoid[t].usave;

            m_trapezoid[m_trapezoid[t].u0].d0 = t;
            m_trapezoid[m_trapezoid[tn].u0].d0 = tn;
            m_trapezoid[m_trapezoid[tn].u1].d0 = tn;
          } else { /* intersects in the right */
            m_trapezoid[tn].u1 = -1;
            m_trapezoid[tn].u0 = m_trapezoid[t].u1;
            m_trapezoid[t].u1 = m_trapezoid[t].u0;
            m_trapezoid[t].u0 = m_trapezoid[t].usave;

            m_trapezoid[m_trapezoid[t].u0].d0 = t;
            m_trapezoid[m_trapezoid[t].u1].d0 = t;
            m_trapezoid[m_trapezoid[tn].u0].d0 = tn;
          }

          m_trapezoid[t].usave = m_trapezoid[tn].usave = 0;
        } else { /* No usave.... simple case */
          m_trapezoid[tn].u0 = m_trapezoid[t].u1;
          m_trapezoid[t].u1 = m_trapezoid[tn].u1 = -1;
          m_trapezoid[m_trapezoid[tn].u0].d0 = tn;
        }
      } else { /* fresh m_segment. or upward cusp */
        int tmp_u = m_trapezoid[t].u0;
        int td0, td1;
        if (((td0 = m_trapezoid[tmp_u].d0) > 0) && ((td1 = m_trapezoid[tmp_u].d1) > 0)) { /* upward cusp */
          if ((m_trapezoid[td0].rseg > 0) && !is_left_of(m_trapezoid[td0].rseg, &s.v1)) {
            m_trapezoid[t].u0 = m_trapezoid[t].u1 = m_trapezoid[tn].u1 = -1;
            m_trapezoid[m_trapezoid[tn].u0].d1 = tn;
          } else {
            m_trapezoid[tn].u0 = m_trapezoid[tn].u1 = m_trapezoid[t].u1 = -1;
            m_trapezoid[m_trapezoid[t].u0].d0 = t;
          }
        } else { /* fresh segment */
          m_trapezoid[m_trapezoid[t].u0].d0 = t;
          m_trapezoid[m_trapezoid[t].u0].d1 = tn;
        }
      }

      if (FP_EQUAL(m_trapezoid[t].lo.y, m_trapezoid[tlast].lo.y) &&
          FP_EQUAL(m_trapezoid[t].lo.x, m_trapezoid[tlast].lo.x) && tribot) { /* bottom forms a triangle */
        int tmpseg;
        if (is_swapped) {
          tmpseg = segment_number - 1;
          if (tmpseg == 0)
            tmpseg = m_number_of_segments;
        } else
          tmpseg = MODULO_NEXT(segment_number + 1, m_number_of_segments);

        if ((tmpseg > 0) && is_left_of(tmpseg, &s.v0)) {
          /* L-R downward cusp */
          m_trapezoid[m_trapezoid[t].d1].u0 = t;
          m_trapezoid[tn].d0 = m_trapezoid[tn].d1 = -1;
        } else {
          /* R-L downward cusp */
          m_trapezoid[m_trapezoid[tn].d1].u1 = tn;
          m_trapezoid[t].d0 = m_trapezoid[t].d1 = -1;
        }
      } else {
        if ((m_trapezoid[m_trapezoid[t].d1].u0 > 0) && (m_trapezoid[m_trapezoid[t].d1].u1 > 0)) {
          if (m_trapezoid[m_trapezoid[t].d1].u0 == t) { /* passes thru LHS */
            m_trapezoid[m_trapezoid[t].d1].usave = m_trapezoid[m_trapezoid[t].d1].u1;
            m_trapezoid[m_trapezoid[t].d1].uside = S_LEFT;
          } else {
            m_trapezoid[m_trapezoid[t].d1].usave = m_trapezoid[m_trapezoid[t].d1].u0;
            m_trapezoid[m_trapezoid[t].d1].uside = S_RIGHT;
          }
        }
        m_trapezoid[m_trapezoid[t].d1].u0 = t;
        m_trapezoid[m_trapezoid[t].d1].u1 = tn;
      }

      t = m_trapezoid[t].d1;
    } else {
      /* two trapezoids below. Find out which one is intersected by */
      /* this segment and proceed down that one */
      double y0, yt;
      point_t tmppt;
      int i_d0;

      i_d0 = false;
      if (FP_EQUAL(m_trapezoid[t].lo.y, s.v0.y)) {
        if (m_trapezoid[t].lo.x > s.v0.x)
          i_d0 = true;
      } else {
        tmppt.y = y0 = m_trapezoid[t].lo.y;
        yt = (y0 - s.v0.y) / (s.v1.y - s.v0.y);
        tmppt.x = s.v0.x + yt * (s.v1.x - s.v0.x);

        if (_less_than(&tmppt, &m_trapezoid[t].lo))
          i_d0 = true;
      }

      /* check continuity from the top so that the lower-neighbour */
      /* values are properly filled for the upper trapezoid */

      if ((m_trapezoid[t].u0 > 0) && (m_trapezoid[t].u1 > 0)) { /* continuation of a chain from abv. */
        if (m_trapezoid[t].usave > 0) { /* three upper neighbours */
          if (m_trapezoid[t].uside == S_LEFT) {
            m_trapezoid[tn].u0 = m_trapezoid[t].u1;
            m_trapezoid[t].u1 = -1;
            m_trapezoid[tn].u1 = m_trapezoid[t].usave;

            m_trapezoid[m_trapezoid[t].u0].d0 = t;
            m_trapezoid[m_trapezoid[tn].u0].d0 = tn;
            m_trapezoid[m_trapezoid[tn].u1].d0 = tn;
          } else { /* intersects in the right */
            m_trapezoid[tn].u1 = -1;
            m_trapezoid[tn].u0 = m_trapezoid[t].u1;
            m_trapezoid[t].u1 = m_trapezoid[t].u0;
            m_trapezoid[t].u0 = m_trapezoid[t].usave;

            m_trapezoid[m_trapezoid[t].u0].d0 = t;
            m_trapezoid[m_trapezoid[t].u1].d0 = t;
            m_trapezoid[m_trapezoid[tn].u0].d0 = tn;
          }

          m_trapezoid[t].usave = m_trapezoid[tn].usave = 0;
        } else { /* No usave.... simple case */
          m_trapezoid[tn].u0 = m_trapezoid[t].u1;
          m_trapezoid[tn].u1 = -1;
          m_trapezoid[t].u1 = -1;
          m_trapezoid[m_trapezoid[tn].u0].d0 = tn;
        }
      } else { /* fresh m_segment. or upward cusp */
        int tmp_u = m_trapezoid[t].u0;
        int td0, td1;
        if (((td0 = m_trapezoid[tmp_u].d0) > 0) && ((td1 = m_trapezoid[tmp_u].d1) > 0)) { /* upward cusp */
          if ((m_trapezoid[td0].rseg > 0) && !is_left_of(m_trapezoid[td0].rseg, &s.v1)) {
            m_trapezoid[t].u0 = m_trapezoid[t].u1 = m_trapezoid[tn].u1 = -1;
            m_trapezoid[m_trapezoid[tn].u0].d1 = tn;
          } else {
            m_trapezoid[tn].u0 = m_trapezoid[tn].u1 = m_trapezoid[t].u1 = -1;
            m_trapezoid[m_trapezoid[t].u0].d0 = t;
          }
        } else { /* fresh segment */
          m_trapezoid[m_trapezoid[t].u0].d0 = t;
          m_trapezoid[m_trapezoid[t].u0].d1 = tn;
        }
      }

      if (FP_EQUAL(m_trapezoid[t].lo.y, m_trapezoid[tlast].lo.y) &&
          FP_EQUAL(m_trapezoid[t].lo.x, m_trapezoid[tlast].lo.x) && tribot) {
        /* this case arises only at the lowest trapezoid.. i.e.
           tlast, if the lower endpoint of the segment is
           already inserted in the structure */

        m_trapezoid[m_trapezoid[t].d0].u0 = t;
        m_trapezoid[m_trapezoid[t].d0].u1 = -1;
        m_trapezoid[m_trapezoid[t].d1].u0 = tn;
        m_trapezoid[m_trapezoid[t].d1].u1 = -1;

        m_trapezoid[tn].d0 = m_trapezoid[t].d1;
        m_trapezoid[t].d1 = m_trapezoid[tn].d1 = -1;

        tnext = m_trapezoid[t].d1;
      } else if (i_d0) {
        /* intersecting d0 */
        m_trapezoid[m_trapezoid[t].d0].u0 = t;
        m_trapezoid[m_trapezoid[t].d0].u1 = tn;
        m_trapezoid[m_trapezoid[t].d1].u0 = tn;
        m_trapezoid[m_trapezoid[t].d1].u1 = -1;

        /* new code to determine the bottom neighbours of the */
        /* newly partitioned trapezoid */

        m_trapezoid[t].d1 = -1;

        tnext = m_trapezoid[t].d0;
      } else { /* intersecting d1 */
        m_trapezoid[m_trapezoid[t].d0].u0 = t;
        m_trapezoid[m_trapezoid[t].d0].u1 = -1;
        m_trapezoid[m_trapezoid[t].d1].u0 = t;
        m_trapezoid[m_trapezoid[t].d1].u1 = tn;

        /* new code to determine the bottom neighbours of the */
        /* newly partitioned trapezoid */

        m_trapezoid[tn].d0 = m_trapezoid[t].d1;
        m_trapezoid[tn].d1 = -1;

        tnext = m_trapezoid[t].d1;
      }

      t = tnext;
    }

    m_trapezoid[t_sav].rseg = m_trapezoid[tn_sav].lseg = segment_number;
  } /* end-while */

  /* Now combine those trapezoids which share common segments. We can */
  /* use the pointers to the parent to connect these together. This */
  /* works only because all these new trapezoids have been formed */
  /* due to splitting by the segment, and hence have only one parent */

  tfirstl = tfirst;
  tlastl = tlast;
  merge_trapezoids(segment_number, tfirstl, tlastl, S_LEFT);
  merge_trapezoids(segment_number, tfirstr, tlastr, S_RIGHT);

  m_segment[segment_number].is_inserted = true;
  return 0;
}

/* Update the roots stored for each of the endpoints of the segment.
 * This is done to speed up the location-query for the endpoint when
 * the segment is inserted into the trapezoidation subsequently
 */
int
QcSeidlerPolygonTriangulation::find_new_roots(int segment_number)
{
  segment_t *s = &m_segment[segment_number];
  point_t vper;

  if (s->is_inserted)
    return 0;

  vper.x = s->v0.x + EPS * (s->v1.x - s->v0.x);
  vper.y = s->v0.y + EPS * (s->v1.y - s->v0.y);
  s->root0 = locate_endpoint(&s->v0, &s->v1, s->root0);
  s->root0 = m_trapezoid[s->root0].sink;

  vper.x = s->v1.x + EPS * (s->v0.x - s->v1.x);
  vper.y = s->v1.y + EPS * (s->v0.y - s->v1.y);
  s->root1 = locate_endpoint(&s->v1, &s->v0, s->root1);
  s->root1 = m_trapezoid[s->root1].sink;
  return 0;
}

/* Main routine to perform trapezoidation */
void
QcSeidlerPolygonTriangulation::construct_trapezoids(int nseg, segment_t seg[])
{
  register int i;
  int root, h;

  m_q_index = m_trapezoid_index = 1;
  /* Add the first segment and get the query structure and trapezoid */
  /* list initialised */
  root = init_query_structure(choose_segment());

#ifdef SIMPLE /* no randomization */

  for (i = 1; i <= nseg; i++)
    m_segment[i].root0 = seg[i].root1 = root;

  for (i = 2; i <= nseg; i++)
    add_segment(choose_segment());

#else

  for (i = 1; i <= nseg; i++)
    m_segment[i].root0 = seg[i].root1 = root;

  for (h = 1; h <= math_logstar_n(nseg); h++)
    {
      for (i = math_N(nseg, h - 1) + 1; i <= math_N(nseg, h); i++)
        add_segment(choose_segment());

      /* Find a new root for each of the segment endpoints */
      for (i = 1; i <= nseg; i++)
        find_new_roots(i);
    }

  for (i = math_N(nseg, math_logstar_n(nseg)) + 1; i <= nseg; i++)
    add_segment(choose_segment());

#endif
}

/**************************************************************************************************/

/* Function returns true if the trapezoid lies inside the polygon */
int
QcSeidlerPolygonTriangulation::inside_polygon(trap_t * t)
{
  int rseg = t->rseg;

  if (t->state == ST_INVALID)
    return 0;

  if ((t->lseg <= 0) || (t->rseg <= 0))
    return 0;

  if (((t->u0 <= 0) && (t->u1 <= 0)) || ((t->d0 <= 0) && (t->d1 <= 0))) /* triangle */
    return (_greater_than(&m_segment[rseg].v1, &m_segment[rseg].v0));

  return 0;
}

/* return a new m_mon structure from the table */
int
QcSeidlerPolygonTriangulation::newmon()
{
  return ++m_mon_index;
}

/* return a new chain element from the table */
int
QcSeidlerPolygonTriangulation::new_chain_element()
{
  return ++m_chain_index;
}

/* (v0, v1) is the new diagonal to be added to the polygon. Find which
 * chain to use and return the positions of v0 and v1 in p and q */
int
QcSeidlerPolygonTriangulation::get_vertex_positions(int v0, int v1, int * ip, int * iq)
{
  vertex_chain_t *vp0, *vp1;
  register int i;
  double angle, temp;
  int tp, tq;

  vp0 = &m_vertex[v0];
  vp1 = &m_vertex[v1];

  /* p is identified as follows. Scan from (v0, v1) rightwards till
   * you hit the first segment starting from v0. That chain is the
   * chain of our interest */

  angle = -4.0;
  for (i = 0; i < 4; i++)
    {
      if (vp0->vnext[i] <= 0)
        continue;
      if ((temp = get_angle(&vp0->pt, &(m_vertex[vp0->vnext[i]].pt), &vp1->pt)) > angle)
        {
          angle = temp;
          tp = i;
        }
    }

  *ip = tp;

  /* Do similar actions for q */

  angle = -4.0;
  for (i = 0; i < 4; i++)
    {
      if (vp1->vnext[i] <= 0)
        continue;
      if ((temp = get_angle(&vp1->pt, &(m_vertex[vp1->vnext[i]].pt), &vp0->pt)) > angle)
        {
          angle = temp;
          tq = i;
        }
    }

  *iq = tq;

  return 0;
}

/* v0 and v1 are specified in anti-clockwise order with respect to the
 * current monotone polygon mcur. Split the current polygon into two
 * polygons using the diagonal (v0, v1)
 */
int
QcSeidlerPolygonTriangulation::make_new_monotone_poly(int mcur, int v0, int v1)
{
  int p, q, ip, iq;
  int mnew = newmon();
  int i, j, nf0, nf1;
  vertex_chain_t *vp0, *vp1;

  vp0 = &m_vertex[v0];
  vp1 = &m_vertex[v1];

  get_vertex_positions(v0, v1, &ip, &iq);

  p = vp0->vpos[ip];
  q = vp1->vpos[iq];

  /* At this stage, we have got the positions of v0 and v1 in the
   * desired chain. Now modify the linked lists */

  i = new_chain_element(); /* for the new list */
  j = new_chain_element();

  m_monotone_chain[i].vnum = v0;
  m_monotone_chain[j].vnum = v1;

  m_monotone_chain[i].next = m_monotone_chain[p].next;
  m_monotone_chain[m_monotone_chain[p].next].prev = i;
  m_monotone_chain[i].prev = j;
  m_monotone_chain[j].next = i;
  m_monotone_chain[j].prev = m_monotone_chain[q].prev;
  m_monotone_chain[m_monotone_chain[q].prev].next = j;

  m_monotone_chain[p].next = q;
  m_monotone_chain[q].prev = p;

  nf0 = vp0->nextfree;
  nf1 = vp1->nextfree;

  vp0->vnext[ip] = v1;

  vp0->vpos[nf0] = i;
  vp0->vnext[nf0] = m_monotone_chain[m_monotone_chain[i].next].vnum;
  vp1->vpos[nf1] = j;
  vp1->vnext[nf1] = v0;

  vp0->nextfree++;
  vp1->nextfree++;

#ifdef DEBUG
  fprintf(stderr, "make_poly: mcur = %d, (v0, v1) = (%d, %d)\n", mcur, v0, v1);
  fprintf(stderr, "next posns = (p, q) = (%d, %d)\n", p, q);
#endif

  m_mon[mcur] = p;
  m_mon[mnew] = i;
  return mnew;
}

/* recursively visit all the trapezoids */
int
QcSeidlerPolygonTriangulation::traverse_polygon(int mcur, int trnum, int from, int dir)
{
  trap_t *t = &m_trapezoid[trnum];
  int mnew;
  int v0, v1;
  int retval = 0;
  int do_switch = false;

  if ((trnum <= 0) || m_visited[trnum])
    return 0;

  m_visited[trnum] = true;

  /* We have much more information available here. */
  /* rseg: goes upwards   */
  /* lseg: goes downwards */

  /* Initially assume that dir = TR_FROM_DN(from the left) */
  /* Switch v0 and v1 if necessary afterwards */

  /* special cases for triangles with cusps at the opposite ends. */
  /* take care of this first */
  if ((t->u0 <= 0) && (t->u1 <= 0)) {
    if ((t->d0 > 0) && (t->d1 > 0)) { /* downward opening triangle */
      v0 = m_trapezoid[t->d1].lseg;
      v1 = t->lseg;
      if (from == t->d1) {
        do_switch = true;
        mnew = make_new_monotone_poly(mcur, v1, v0);
        traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
        traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP);
      } else {
        mnew = make_new_monotone_poly(mcur, v0, v1);
        traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
        traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP);
      }
    } else {
      retval = SP_NOSPLIT; /* Just traverse all neighbours */
      traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
      traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
      traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
      traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
    }
  } else if ((t->d0 <= 0) && (t->d1 <= 0)) {
    if ((t->u0 > 0) && (t->u1 > 0)) { /* upward opening triangle */
      v0 = t->rseg;
      v1 = m_trapezoid[t->u0].rseg;
      if (from == t->u1) {
        do_switch = true;
        mnew = make_new_monotone_poly(mcur, v1, v0);
        traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
        traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN);
      } else {
        mnew = make_new_monotone_poly(mcur, v0, v1);
        traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
        traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN);
      }
    } else {
      retval = SP_NOSPLIT; /* Just traverse all neighbours */
      traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
      traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
      traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
      traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
    }
  } else if ((t->u0 > 0) && (t->u1 > 0)) {
    if ((t->d0 > 0) && (t->d1 > 0)) {  /* downward + upward cusps */
      v0 = m_trapezoid[t->d1].lseg;
      v1 = m_trapezoid[t->u0].rseg;
      retval = SP_2UP_2DN;
      if (((dir == TR_FROM_DN) && (t->d1 == from)) || ((dir == TR_FROM_UP) && (t->u1 == from))) {
        do_switch = true;
        mnew = make_new_monotone_poly(mcur, v1, v0);
        traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
        traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
        traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN);
        traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP);
      } else {
        mnew = make_new_monotone_poly(mcur, v0, v1);
        traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
        traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
        traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN);
        traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP);
      }
    } else { /* only downward cusp */
      if (_equal_to(&t->lo, &m_segment[t->lseg].v1)) {
        v0 = m_trapezoid[t->u0].rseg;
        v1 = MODULO_NEXT(t->lseg + 1, m_number_of_segments);
        retval = SP_2UP_LEFT;
        if ((dir == TR_FROM_UP) && (t->u0 == from)) {
          do_switch = true;
          mnew = make_new_monotone_poly(mcur, v1, v0);
          traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
          traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP);
          traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN);
          traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP);
        } else {
          mnew = make_new_monotone_poly(mcur, v0, v1);
          traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
          traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
          traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
          traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN);
        }
      } else {
        v0 = t->rseg;
        v1 = m_trapezoid[t->u0].rseg;
        retval = SP_2UP_RIGHT;
        if ((dir == TR_FROM_UP) && (t->u1 == from)) {
          do_switch = true;
          mnew = make_new_monotone_poly(mcur, v1, v0);
          traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
          traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP);
          traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP);
          traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN);
        } else {
          mnew = make_new_monotone_poly(mcur, v0, v1);
          traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
          traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
          traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
          traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN);
        }
      }
    }
  } else {
    if ((t->d0 > 0) && (t->d1 > 0)) { /* only upward cusp */
      if (_equal_to(&t->hi, &m_segment[t->lseg].v0)) {
        v0 = m_trapezoid[t->d1].lseg;
        v1 = t->lseg;
        retval = SP_2DN_LEFT;
        if (!((dir == TR_FROM_DN) && (t->d0 == from))) {
          do_switch = true;
          mnew = make_new_monotone_poly(mcur, v1, v0);
          traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
          traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
          traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
          traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP);
        } else {
          mnew = make_new_monotone_poly(mcur, v0, v1);
          traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
          traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN);
          traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN);
          traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP);
        }
      } else {
        v0 = m_trapezoid[t->d1].lseg;
        v1 = MODULO_NEXT(t->rseg + 1, m_number_of_segments);
        retval = SP_2DN_RIGHT;
        if ((dir == TR_FROM_DN) && (t->d1 == from)) {
          do_switch = true;
          mnew = make_new_monotone_poly(mcur, v1, v0);
          traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
          traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN);
          traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN);
          traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP);
        } else {
          mnew = make_new_monotone_poly(mcur, v0, v1);
          traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
          traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
          traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
          traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP);
        }
      }
    } else { /* no cusp */
      if (_equal_to(&t->hi, &m_segment[t->lseg].v0) && _equal_to(&t->lo, &m_segment[t->rseg].v0)) {
        v0 = t->rseg;
        v1 = t->lseg;
        retval = SP_SIMPLE_LRDN;
        if (dir == TR_FROM_UP) {
          do_switch = true;
          mnew = make_new_monotone_poly(mcur, v1, v0);
          traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
          traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
          traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP);
          traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP);
        } else {
          mnew = make_new_monotone_poly(mcur, v0, v1);
          traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
          traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
          traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN);
          traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN);
        }
      } else if (_equal_to(&t->hi, &m_segment[t->rseg].v1) && _equal_to(&t->lo, &m_segment[t->lseg].v1)) {
        v0 = MODULO_NEXT(t->rseg + 1, m_number_of_segments);
        v1 = MODULO_NEXT(t->lseg + 1, m_number_of_segments);
        retval = SP_SIMPLE_LRUP;
        if (dir == TR_FROM_UP) {
          do_switch = true;
          mnew = make_new_monotone_poly(mcur, v1, v0);
          traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
          traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
          traverse_polygon(mnew, t->d1, trnum, TR_FROM_UP);
          traverse_polygon(mnew, t->d0, trnum, TR_FROM_UP);
        } else {
          mnew = make_new_monotone_poly(mcur, v0, v1);
          traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
          traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
          traverse_polygon(mnew, t->u0, trnum, TR_FROM_DN);
          traverse_polygon(mnew, t->u1, trnum, TR_FROM_DN);
        }
      } else { /* no split possible */
        retval = SP_NOSPLIT;
        traverse_polygon(mcur, t->u0, trnum, TR_FROM_DN);
        traverse_polygon(mcur, t->d0, trnum, TR_FROM_UP);
        traverse_polygon(mcur, t->u1, trnum, TR_FROM_DN);
        traverse_polygon(mcur, t->d1, trnum, TR_FROM_UP);
      }
    }
  }

  return retval;
}

/* Main routine to get monotone polygons from the trapezoidation of the polygon.
 */
int
QcSeidlerPolygonTriangulation::monotonate_trapezoids(int n)
{
  register int i;
  int tr_start;

  memset((void *) m_vertex, 0, sizeof(m_vertex));
  memset((void *) m_visited, 0, sizeof(m_visited));
  memset((void *) m_monotone_chain, 0, sizeof(m_monotone_chain));
  memset((void *) m_mon, 0, sizeof(m_mon));

  /* First locate a trapezoid which lies inside the polygon */
  /* and which is triangular */
  for (i = 0; i < TRSIZE; i++)
    if (inside_polygon(&m_trapezoid[i]))
      break;
  tr_start = i;

  /* Initialise the m_mon data-structure and start spanning all the */
  /* trapezoids within the polygon */

  for (i = 1; i <= n; i++) {
    m_monotone_chain[i].prev = i - 1;
    m_monotone_chain[i].next = i + 1;
    m_monotone_chain[i].vnum = i;
    m_vertex[i].pt = m_segment[i].v0;
    m_vertex[i].vnext[0] = i + 1; /* next vertex */
    m_vertex[i].vpos[0] = i; /* locn. of next vertex */
    m_vertex[i].nextfree = 1;
  }
  m_monotone_chain[1].prev = n;
  m_monotone_chain[n].next = 1;
  m_vertex[n].vnext[0] = 1;
  m_vertex[n].vpos[0] = n;
  m_chain_index = n;
  m_mon_index = 0;
  m_mon[0] = 1; /* position of any vertex in the first */
  /* chain  */

  /* traverse the polygon */
  if (m_trapezoid[tr_start].u0 > 0)
    traverse_polygon(0, tr_start, m_trapezoid[tr_start].u0, TR_FROM_UP);
  else if (m_trapezoid[tr_start].d0 > 0)
    traverse_polygon(0, tr_start, m_trapezoid[tr_start].d0, TR_FROM_DN);

  /* return the number of polygons created */
  return newmon();
}

/* A greedy corner-cutting algorithm to triangulate a y-monotone polygon in O(n) time.
 * Joseph O-Rourke, Computational Geometry in C.
 */
int
QcSeidlerPolygonTriangulation::triangulate_single_polygon(int posmax, int side, int op[][3])
{
  register int v;
  int rc[SEGSIZE], ri = 0; /* reflex chain */
  int endv, tmp, vpos;

  if (side == TRI_RHS) { /* RHS segment is a single segment */
    rc[0] = m_monotone_chain[posmax].vnum;
    tmp = m_monotone_chain[posmax].next;
    rc[1] = m_monotone_chain[tmp].vnum;
    ri = 1;

    vpos = m_monotone_chain[tmp].next;
    v = m_monotone_chain[vpos].vnum;

    if ((endv = m_monotone_chain[m_monotone_chain[posmax].prev].vnum) == 0)
      endv = m_number_of_segments;
  } else { /* LHS is a single segment */
    tmp = m_monotone_chain[posmax].next;
    rc[0] = m_monotone_chain[tmp].vnum;
    tmp = m_monotone_chain[tmp].next;
    rc[1] = m_monotone_chain[tmp].vnum;
    ri = 1;

    vpos = m_monotone_chain[tmp].next;
    v = m_monotone_chain[vpos].vnum;

    endv = m_monotone_chain[posmax].vnum;
  }

  while ((v != endv) || (ri > 1)) {
    if (ri > 0) { /* reflex chain is non-empty */
      if (CROSS(m_vertex[v].pt, m_vertex[rc[ri - 1]].pt, m_vertex[rc[ri]].pt) > 0) { /* convex corner: cut if off */
        op[m_op_index][0] = rc[ri - 1];
        op[m_op_index][1] = rc[ri];
        op[m_op_index][2] = v;
        m_op_index++;
        ri--;
      } else { /* non-convex */
        /* add v to the chain */
        ri++;
        rc[ri] = v;
        vpos = m_monotone_chain[vpos].next;
        v = m_monotone_chain[vpos].vnum;
      }
    } else { /* reflex-chain empty: add v to the */
      /* reflex chain and advance it  */
      rc[++ri] = v;
      vpos = m_monotone_chain[vpos].next;
      v = m_monotone_chain[vpos].vnum;
    }
  } /* end-while */

  /* reached the bottom vertex. Add in the triangle formed */
  op[m_op_index][0] = rc[ri - 1];
  op[m_op_index][1] = rc[ri];
  op[m_op_index][2] = v;
  m_op_index++;
  ri--;

  return 0;
}

void
QcSeidlerPolygonTriangulation::triangulate_monotone_polygons(int nmonpoly, int op[][3])
{
  register int i;
  point_t ymax, ymin;
  int p, vfirst, posmax, posmin, v;
  int vcount;

#ifdef DEBUG
  for (i = 0; i < nmonpoly; i++) {
    fprintf(stderr, "\n\nPolygon %d: ", i);
    vfirst = m_monotone_chain[m_mon[i]].vnum;
    p = m_monotone_chain[m_mon[i]].next;
    fprintf(stderr, "%d ", m_monotone_chain[m_mon[i]].vnum);
    while (m_monotone_chain[p].vnum != vfirst)
      {
        fprintf(stderr, "%d ", m_monotone_chain[p].vnum);
        p = m_monotone_chain[p].next;
      }
  }
  fprintf(stderr, "\n");
#endif

  m_op_index = 0;
  for (i = 0; i < nmonpoly; i++) {
    vcount = 1;
    vfirst = m_monotone_chain[m_mon[i]].vnum;
    ymax = ymin = m_vertex[vfirst].pt;
    posmax = posmin = m_mon[i];
    p = m_monotone_chain[m_mon[i]].next;
    while ((v = m_monotone_chain[p].vnum) != vfirst) {
      if (_greater_than(&m_vertex[v].pt, &ymax)) {
        ymax = m_vertex[v].pt;
        posmax = p;
      }
      if (_less_than(&m_vertex[v].pt, &ymin)) {
        ymin = m_vertex[v].pt;
        posmin = p;
      }
      p = m_monotone_chain[p].next;
      vcount++;
    }

    if (vcount == 3) { /* already a triangle */
      op[m_op_index][0] = m_monotone_chain[p].vnum;
      op[m_op_index][1] = m_monotone_chain[m_monotone_chain[p].next].vnum;
      op[m_op_index][2] = m_monotone_chain[m_monotone_chain[p].prev].vnum;
      m_op_index++;
    } else { /* triangulate the polygon */
      v = m_monotone_chain[m_monotone_chain[posmax].next].vnum;
      if (_equal_to(&m_vertex[v].pt, &ymin)) { /* LHS is a single line */
        triangulate_single_polygon(posmax, TRI_LHS, op);
      } else
        triangulate_single_polygon(posmax, TRI_RHS, op);
    }
  }

#ifdef DEBUG
  for (i = 0; i < (m_number_of_segments - 2); i++)
    fprintf(stderr, "tri #%d: (%d, %d, %d)\n", i, op[i][0], op[i][1], op[i][2]);
#endif
}

/**************************************************************************************************/

/* The points constituting the polygon are specified in anticlockwise
 * order. If there are n points in the polygon, it would be the
 * points p0, p1....p(n) (where p0 and pn are the same point). The
 * output is contained in the array "triangles".
 *
 * Every triangle is output in anticlockwise order and the 3
 * integers are the indices of the points. Thus, the triangle (i, j, k)
 * refers to the triangle formed by the points (pi, pj, pk). Before
 * using this routine, please check-out that you do not conflict with
 * the m_global variables defined in basic.h.
 *
 * n:         number of points in polygon (p0 = pn)
 * vertices:  the vertices p0, p1..., p(n) of the polygon
 * triangles: output array containing the triangles
 */
QcSeidlerPolygonTriangulation::QcSeidlerPolygonTriangulation(int number_of_segments, double vertices[][2], int triangles[][3])
{
  m_number_of_segments = number_of_segments;

  memset((void *) m_segment, 0, sizeof(m_segment));
  for (int i = 1; i <= number_of_segments; i++) {
    m_segment[i].is_inserted = false;

    m_segment[i].v0.x = vertices[i][0]; /* x-cood */
    m_segment[i].v0.y = vertices[i][1]; /* y-cood */
    if (i == 1) {
      m_segment[number_of_segments].v1.x = m_segment[i].v0.x;
      m_segment[number_of_segments].v1.y = m_segment[i].v0.y;
    } else {
      m_segment[i - 1].v1.x = m_segment[i].v0.x;
      m_segment[i - 1].v1.y = m_segment[i].v0.y;
    }
  }

  // for (int i = 1; i <= number_of_segments; i++)
  //   m_segment[i].is_inserted = false;
  generate_random_ordering(number_of_segments);

  construct_trapezoids(number_of_segments, m_segment);
  int number_of_monotone_polygons = monotonate_trapezoids(number_of_segments);
  triangulate_monotone_polygons(number_of_monotone_polygons, triangles);
}

/* This function returns true or false depending upon whether the
 * vertex is inside the polygon or not. The polygon must already have
 * been triangulated before this routine is called.
 * This routine will always detect all the points belonging to the
 * set (polygon-area - polygon-boundary). The return value for points
 * on the boundary is not consistent!!!
 */
/*
int
is_point_inside_polygon(double vertex[2])
{
  point_t v;
  int trnum, rseg;
  trap_t *t;

  v.x = vertex[0];
  v.y = vertex[1];

  trnum = locate_endpoint(&v, &v, 1);
  t = &m_trapezoid[trnum];

  if (t->state == ST_INVALID)
    return false;

  if ((t->lseg <= 0) || (t->rseg <= 0))
    return false;
  rseg = t->rseg;
  return _greater_than_equal_to(&m_segment[rseg].v1, &seg[rseg].v0);
}
*/

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
