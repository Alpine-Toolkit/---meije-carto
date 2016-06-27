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

#include "path_property.h"

#include <QtDebug>

/**************************************************************************************************/

QcPathProperty::QcPathProperty(QObject * parent)
  : QObject(parent),
    m_length(),
    m_area()
{}

QcPathProperty::QcPathProperty(const QcPathProperty & other, QObject * parent)
  : QObject(parent),
    m_length(other.m_length),
    m_area(other.m_area)
{}

QcPathProperty::~QcPathProperty()
{}

QcPathProperty &
QcPathProperty::operator=(const QcPathProperty & other)
{
  if (this != &other) {
    m_length = other.m_length;
    m_area = other.m_area;
  }

  return *this;
}

void
QcPathProperty::set_length(double length)
{
  if (!qFuzzyCompare(length, m_length)) {
    m_length = length;
    emit length_changed();
  };
}

void
QcPathProperty::set_area(double area)
{
  if (!qFuzzyCompare(area, m_area)) {
    m_area = area;
    emit area_changed();
  }
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
