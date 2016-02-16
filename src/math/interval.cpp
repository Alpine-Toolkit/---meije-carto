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

#include "interval.h"

/**************************************************************************************************/

template<>
double
QcIntervalDouble::wrap(double x) const
{
  if (contains(x))
    return x;
  else
    return fmod(x - m_inf, length()) + m_inf;
}

template<>
QcIntervalInt
QcIntervalInt::cut(const QcIntervalInt & other, QcIntervalInt & left, QcIntervalInt & right, bool & exchanged) const
{
  QcIntervalInt _left;
  QcIntervalInt _right;
  if (*this < other) {
    _left = *this;
    _right = other;
    exchanged = false;
  } else {
    _left = other;
    _right = *this;
    exchanged = true;
  }

  QcIntervalInt intersection = _left & _right;
  if (intersection.is_empty()) {
    left = _left;
    right = _right;
  } else {
    int inf, sup;

    inf = _left.inf();
    sup = intersection.inf() -1;
    if (inf <= sup)
      left = QcIntervalInt(inf, sup);
    else
      left = QcIntervalInt();

    inf = intersection.sup() +1;
    sup = _right.sup();
    if (inf <= sup)
      right = QcIntervalInt(inf, sup);
    else
      right = QcIntervalInt();
  }

  return intersection;
}

template<>
QcIntervalDouble
QcIntervalDouble::cut(const QcIntervalDouble & other, QcIntervalDouble & left, QcIntervalDouble & right, bool & exchanged) const
{
  QcIntervalDouble _left;
  QcIntervalDouble _right;
  if (*this < other) {
    _left = *this;
    _right = other;
    exchanged = false;
  } else {
    _left = other;
    _right = *this;
    exchanged = true;
  }

  QcIntervalDouble intersection = _left & _right;
  if (intersection.is_empty()) {
    left = _left;
    right = _right;
  } else {
    int inf, sup;

    inf = _left.inf();
    sup = intersection.inf();
    if (inf <= sup)
      left = QcIntervalDouble(inf, sup);
    else
      left = QcIntervalDouble();

    inf = intersection.sup();
    sup = _right.sup();
    if (inf <= sup)
      right = QcIntervalDouble(inf, sup);
    else
      right = QcIntervalDouble();
  }

  return intersection;
}

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug debug, const QcIntervalInt & interval)
{
  QDebugStateSaver saver(debug); // Fixme: ???

  debug.nospace() << "QcIntervalInt(";
  debug << interval.inf();
  debug << ", ";
  debug << interval.sup();
  debug << ')';

  return debug;
}
#endif

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug debug, const QcIntervalDouble & interval)
{
  QDebugStateSaver saver(debug); // Fixme: ???

  debug.nospace() << "QcIntervalDouble(";
  debug << interval.inf();
  debug << ", ";
  debug << interval.sup();
  debug << ')';

  return debug;
}
#endif

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug debug, const QcInterval2DInt & interval)
{
  QDebugStateSaver saver(debug); // Fixme: ???

  debug.nospace() << "QcInterval2DInt(";
  debug << interval.x().inf();
  debug << ", ";
  debug << interval.x().sup();
  debug << ", ";
  debug << interval.y().inf();
  debug << ", ";
  debug << interval.y().sup();
  debug << ')';

  return debug;
}
#endif

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug debug, const QcInterval2DDouble & interval)
{
  QDebugStateSaver saver(debug); // Fixme: ???

  debug.nospace() << "QcInterval2DDouble(";
  debug << interval.x().inf();
  debug << ", ";
  debug << interval.x().sup();
  debug << ", ";
  debug << interval.y().inf();
  debug << ", ";
  debug << interval.y().sup();
  debug << ')';

  return debug;
}
#endif

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
