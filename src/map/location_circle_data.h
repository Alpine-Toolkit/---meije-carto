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

#ifndef __LOCATION_CIRCLE_DATA_H__
#define __LOCATION_CIRCLE_DATA_H__

/**************************************************************************************************/

#include <QObject>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcLocationCircleData : public QObject
{
  Q_OBJECT
  Q_PROPERTY(double bearing READ bearing WRITE set_bearing NOTIFY bearingChanged)
  Q_PROPERTY(double horizontal_precision READ horizontal_precision WRITE set_horizontal_precision NOTIFY horizontal_precisionChanged)

public:
  QcLocationCircleData(QObject * parent = nullptr);
  QcLocationCircleData(const QcLocationCircleData & other, QObject * parent = nullptr);
  ~QcLocationCircleData();

  QcLocationCircleData & operator=(const QcLocationCircleData & other);

  double bearing() const { return m_bearing; }
  void set_bearing(double bearing);

  double horizontal_precision() const { return m_horizontal_precision; }
  void set_horizontal_precision(double horizontal_precision);

signals:
  void bearingChanged();
  void horizontal_precisionChanged();

private:
  double m_bearing;
  double m_horizontal_precision;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

Q_DECLARE_METATYPE(QcLocationCircleData)

/**************************************************************************************************/

#endif /* __LOCATION_CIRCLE_DATA_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
