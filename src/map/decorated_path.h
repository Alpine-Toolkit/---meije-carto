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

#ifndef __DECORATED_PATH_H__
#define __DECORATED_PATH_H__

/**************************************************************************************************/

#include "qtcarto_global.h"
#include "enum_flag.h"
#include "geometry/path.h"

#include <QList>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

// Fixme: vertex must be const
//  how to extend QcVectorDouble ? Union (const vertex, attribute)
//  how to get vertex and attribute as one object ?

class QcDecoratedPathDouble : public QcPathDouble
{
public:
  enum class AttributeType {
    Normal = 1,
    Selected = 2,
    Touched = 4
  };

public:
  QcDecoratedPathDouble();
  QcDecoratedPathDouble(const VertexListType & vertexes, bool closed = false);
  // QcDecoratedPathDouble(const QcVectorDouble & coordinates, bool closed = false);
  QcDecoratedPathDouble(const QcDecoratedPathDouble & path);
  QcDecoratedPathDouble(const QcPathDouble & path);
  ~QcDecoratedPathDouble();

  QcDecoratedPathDouble & operator=(const QcDecoratedPathDouble & other);
  QcDecoratedPathDouble & operator=(const QcPathDouble & other);

  void clear();
  void add_vertex(const VertexType & vertex);

  AttributeType attribute_at(int i) const { return m_attributes[i]; }
  void set_attribute_at(int i, AttributeType value) { m_attributes[i] = value; }

private:
  void set_attributes(AttributeType value);

public:
  QList<AttributeType> m_attributes;
};

ENUM_FLAGS(QcDecoratedPathDouble::AttributeType, unsigned int)

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __DECORATED_PATH_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
