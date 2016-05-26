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

#ifndef __ENUM_FLAG_H__
#define __ENUM_FLAG_H__

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

#define ENUM_FLAGS(T, INT_T) \
  inline T operator & (T x, T y) { return static_cast<T>(static_cast<INT_T>(x) & static_cast<INT_T>(y)); }; \
  inline T operator | (T x, T y) { return static_cast<T>(static_cast<INT_T>(x) | static_cast<INT_T>(y)); }; \
  inline T operator ^ (T x, T y) { return static_cast<T>(static_cast<INT_T>(x) ^ static_cast<INT_T>(y)); }; \
  inline T operator ~ (T x)      { return static_cast<T>(~static_cast<INT_T>(x)); }; \
  inline T & operator &= (T & x, T y) { x = x & y; return x; };         \
  inline T & operator |= (T & x, T y) { x = x | y; return x; };         \
  inline T & operator ^= (T & x, T y) { x = x ^ y; return x; };         \
  inline bool __flag_to_bool (T x) { return static_cast<INT_T>(x) != 0; } \
  inline bool test_bit (T x, T y) { return __flag_to_bool(x & y); }

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __ENUM_FLAG_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
