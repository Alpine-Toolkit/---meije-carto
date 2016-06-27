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

#ifndef __PATH_PROPERTY_H__
#define __PATH_PROPERTY_H__

/**************************************************************************************************/

#include <QObject>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcPathProperty : public QObject
{
  Q_OBJECT
  // FIXME: CONSTANT
  Q_PROPERTY(double length READ length WRITE set_length NOTIFY length_changed)
  Q_PROPERTY(double area READ area WRITE set_area NOTIFY area_changed)

public:
  QcPathProperty(QObject * parent = nullptr);
  QcPathProperty(const QcPathProperty & other, QObject * parent = nullptr);
  ~QcPathProperty();

  QcPathProperty & operator=(const QcPathProperty & other);

  double length() const { return m_length; }
  void set_length(double length);

  double area() const { return m_area; }
  void set_area(double area);

signals:
  void length_changed();
  void area_changed();

private:
  double m_length;
  double m_area;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

Q_DECLARE_METATYPE(QcPathProperty)

/**************************************************************************************************/

#endif /* __PATH_PROPERTY_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
