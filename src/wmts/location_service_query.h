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

#ifndef __LOCATION_SERVICE_QUERY_H__
#define __LOCATION_SERVICE_QUERY_H__

/**************************************************************************************************/

#include "coordinate/mercator.h"
#include "coordinate/wgs84.h"
#include "math/interval.h"

#include <QMap>
#include <QObject>
#include <QString>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcLocationServiceQueryBase
{
  Q_GADGET
  Q_PROPERTY(int maximum_responses READ maximum_responses WRITE set_maximum_responses)
  Q_PROPERTY(QString request_id READ request_id WRITE set_request_id)
  // Q_PROPERTY(QString srs_name READ srs_name WRITE set_srs_name)

public:
  QcLocationServiceQueryBase();
  QcLocationServiceQueryBase(const QcLocationServiceQueryBase & other);
  ~QcLocationServiceQueryBase();

  QcLocationServiceQueryBase & operator=(const QcLocationServiceQueryBase & other);

  int maximum_responses() const { return m_maximum_responses; }
  void set_maximum_responses(int maximum_responses) { m_maximum_responses = maximum_responses; }

  const QString & request_id() const { return m_request_id; }
  void set_request_id(const QString & request_id) { m_request_id = request_id; }

  // const QString & srs_name() const { return m_srs_name; }
  // void set_srs_name(const QString & srs_name);

private:
  int m_maximum_responses;
  QString m_request_id;
  // QString m_srs_name;
};

/**************************************************************************************************/

class QcLocationServiceQuery : public QcLocationServiceQueryBase
{
  Q_GADGET
  Q_PROPERTY(QString country_code READ country_code WRITE set_country_code)
  Q_PROPERTY(QString free_form_address READ free_form_address WRITE set_free_form_address)
  Q_PROPERTY(QString postale_code READ postale_code WRITE set_postale_code)
  Q_PROPERTY(QString srs_name READ srs_name WRITE set_srs_name)
  Q_PROPERTY(QString street READ street WRITE set_street)
  // Q_PROPERTY(QMap<QString, QString> places READ places WRITE set_places)
  // Q_PROPERTY(QcInterval2DDouble bounding_box READ bounding_box WRITE set_bounding_box)

public:
  QcLocationServiceQuery();
  QcLocationServiceQuery(const QcLocationServiceQuery & other);
  ~QcLocationServiceQuery();

  QcLocationServiceQuery & operator=(const QcLocationServiceQuery & other);

  const QString & srs_name() const { return m_srs_name; }
  void set_srs_name(const QString & srs_name);

  const QString & country_code() const { return m_country_code; }
  void set_country_code(const QString & country_code);

  const QString & free_form_address() const { return m_free_form_address; }
  void set_free_form_address(const QString & free_form_address);

  const QString & street() const { return m_street; }
  void set_street(const QString & street);

  const QMap<QString, QString> & place() const { return m_place; }
  void add_place(const QString & type, const QString & place);

  const QString & postale_code() const { return m_postale_code; }
  void set_postale_code(const QString & postale_code);

  const QcInterval2DDouble & bounding_box() const { return m_bounding_box; }
  void set_bounding_box(const QcInterval2DDouble & bounding_box);

private:
  QString m_srs_name;
  QString m_country_code;
  QString m_free_form_address;
  QString m_street;
  QString m_postale_code;
  QMap<QString, QString> m_place;
  QcInterval2DDouble m_bounding_box;
};

/**************************************************************************************************/

class QcLocationServiceReverseQuery : public QcLocationServiceQueryBase
{
  Q_GADGET
  Q_PROPERTY(QcWgsCoordinate coordinate READ coordinate WRITE set_coordinate)
  Q_PROPERTY(int radius READ radius WRITE set_radius)

public:
  QcLocationServiceReverseQuery();
  QcLocationServiceReverseQuery(const QcLocationServiceReverseQuery & other);
  ~QcLocationServiceReverseQuery();

  QcLocationServiceReverseQuery & operator=(const QcLocationServiceReverseQuery & other);

  const QcWgsCoordinate & coordinate() const { return m_coordinate; }
  void set_coordinate(const QcWgsCoordinate & coordinate);

  int radius() const { return m_radius; }
  void set_radius(int radius);

private:
  QcWgsCoordinate m_coordinate;
  int m_radius;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __LOCATION_SERVICE_QUERY_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
