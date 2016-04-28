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

#ifndef __SEGMENT_HXX__
#define __SEGMENT_HXX__

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

/*
  Travelling from p0 to p1 to p2.
  @return -1 for counter-clockwise or if p0 is on the line segment between p1 and p2
  1 for clockwise or if p1 is on the line segment between p0 and p2
  0 if p2 in on the line segment between p0 and p1
*/
template <typename T>
int
triangle_orientation(const QcVector<T> & p0, const QcVector<T> & p1, const QcVector<T> & p2)
{
  T dx1 = p1.x() - p0.x();
  T dy1 = p1.y() - p0.y();
  T dx2 = p2.x() - p0.x();
  T dy2 = p2.y() - p0.y();

  // second slope is greater than the first one --> counter-clockwise
  if (dx1 * dy2 > dx2 * dy1) {
    return 1;
  }
  // first slope is greater than the second one --> clockwise
  else if (dx1 * dy2 < dx2 * dy1) {
    return -1;
  }
  // both slopes are equal --> collinear line segments
  else {
    // p0 is between p1 and p2
    if (dx1 * dx2 < 0 || dy1 * dy2 < 0) {
      return -1;
    }
    // p2 is between p0 and p1, as the length is compared
    // square roots are avoided to increase performance
    else if (dx1 * dx1 + dy1 * dy1 >= dx2 * dx2 + dy2 * dy2) {
      return 0;
    }
    // p1 is between p0 and p2
    else {
      return 1;
    }
  }
}

/*
  Checks if the line segments intersect.
  @return 1 if there is an intersection
  0 otherwise
*/
template <typename T>
unsigned int
QcSegment<T>::intersect(const Segment & line2)
{
  const Segment & line1 = *this;

  // triangle_orientation returns 0 if two points are identical, except from the situation
  // when p0 and p1 are identical and different from p2
  int ccw11 = triangle_orientation(line1.p1(), line1.p2(), line2.p1());
  int ccw12 = triangle_orientation(line1.p1(), line1.p2(), line2.p2());
  int ccw21 = triangle_orientation(line2.p1(), line2.p2(), line1.p1());
  int ccw22 = triangle_orientation(line2.p1(), line2.p2(), line1.p2());

  return (((ccw11 * ccw12 < 0) && (ccw21 * ccw22 < 0))
          // one ccw value is zero to detect an intersection
          || (ccw11 * ccw12 * ccw21 * ccw22 == 0)) ? 1 : 0;
}

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __SEGMENT_HXX__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
