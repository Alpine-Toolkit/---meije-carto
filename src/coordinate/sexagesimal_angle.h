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

#ifndef __SEXAGESIMAL_ANGLE_H__
#define __SEXAGESIMAL_ANGLE_H__

/**************************************************************************************************/

#include "qtcarto_global.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QC_EXPORT QcGeoSexagesimalAngle
{
 public:
  static double to_decimal(int degrees, int minutes=0, double seconds=0);
  static void to_sexagesimal(double angle, int & degrees, int & minutes, double & seconds);

  inline static bool is_valid_degrees(int degrees) {
    // Fixme: -0 sign, 360 ? modulo ?
    return -360 <= degrees && degrees <= 360;
  }

  inline static bool is_valid_minutes(int degrees) {
    return 0 <= degrees && degrees <= 60;
  }

 public:
  QcGeoSexagesimalAngle(int degrees, int minutes=0, double seconds=0);
  QcGeoSexagesimalAngle(double degrees);
  QcGeoSexagesimalAngle(const QcGeoSexagesimalAngle & other);
  ~QcGeoSexagesimalAngle();

  QcGeoSexagesimalAngle & operator=(const QcGeoSexagesimalAngle & other);

  bool operator==(const QcGeoSexagesimalAngle & other) const;
  inline bool operator!=(const QcGeoSexagesimalAngle & other) const {
    return !operator==(other);
  }

  inline int degrees() const { return m_degrees; }
  inline void set_degrees(int degrees) {
    if (is_valid_degrees(degrees))
      m_degrees = degrees;
  }
  void set_degrees(double degrees);

  inline int minutes() const { return m_degrees; }
  inline void set_minutes(int minutes) {
    if (is_valid_minutes(minutes))
      m_minutes = minutes;
  }

  inline double seconds() const { return m_degrees; }
  inline void set_seconds(double seconds) { m_seconds = seconds; }

  double decimal() const;

 private:
  int m_degrees;
  int m_minutes; // Fixme: unsigned
  double m_seconds;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __SEXAGESIMAL_ANGLE_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
