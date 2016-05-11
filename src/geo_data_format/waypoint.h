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

#ifndef __WAYPOINT_H__
#define __WAYPOINT_H__

/**************************************************************************************************/

#include <QDateTime>
#include <QString>
#include <QList>

#include "coordinate/geo_coordinate.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/*
 * wptType
 * <...
 * lat="latitudeType [1]"
 * lon="longitudeType [1]">
 * <ele> xsd:decimal </ele> [0..1]
 * <time> xsd:dateTime </time> [0..1]
 * <magvar> degreesType </magvar> [0..1]
 * <geoidheight> xsd:decimal </geoidheight> [0..1]
 * <name> xsd:string </name> [0..1]
 * <cmt> xsd:string </cmt> [0..1]
 * <desc> xsd:string </desc> [0..1]
 * <src> xsd:string </src> [0..1]
 * <link> linkType </link> [0..*]
 * <sym> xsd:string </sym> [0..1]
 * <type> xsd:string </type> [0..1]
 * <fix> fixType </fix> [0..1]
 * <sat> xsd:nonNegativeInteger </sat> [0..1]
 * <hdop> xsd:decimal </hdop> [0..1]
 * <vdop> xsd:decimal </vdop> [0..1]
 * <pdop> xsd:decimal </pdop> [0..1]
 * <ageofdgpsdata> xsd:decimal </ageofdgpsdata> [0..1]
 * <dgpsid> dgpsStationType </dgpsid> [0..1]
 * <extensions> extensionsType </extensions> [0..1]
 * </...>
 */

class QC_EXPORT QcWayPoint
{
 public:
  QcWayPoint();
  QcWayPoint(const QcWayPoint & other);
  ~QcWayPoint();

  QcWayPoint & operator=(const QcWayPoint & other);

  // bool operator==(const QcWayPoint & rhs) const;

  QcGeoElevationCoordinateWGS84 coordinate() const;
  void set_coordinate(const QcGeoElevationCoordinateWGS84 & coordinate);

  QDateTime time() const;
  void set_time(const QDateTime & time);

  qreal magnetic_variation() const;
  void set_magnetic_variation(qreal magnetic_variation);

  qreal geoid_height() const;
  void set_geoid_height(qreal geoid_height);

  const QString & name() const;
  void set_name(const QString & name);

  const QString & comment() const;
  void set_comment(const QString & comment);

  const QString & description() const;
  void set_description(const QString & description);

  const QString & source() const;
  void set_source(const QString & source);

  const QList<QString> link() const;
  void add_link(const QString & link);

  const QString & symbol() const;
  void set_symbol(const QString & symbol);

  const QString & type() const;
  void set_type(const QString & type);

  const QString & fix_type() const;
  void set_fix_type(const QString & fix_type);

  int number_of_satellites() const;
  void set_number_of_satellites(int number_of_satellites);

  qreal hdop() const;
  void set_hdop(qreal hdop);

  qreal vdop() const;
  void set_vdop(qreal vdop);

  qreal pdop() const;
  void set_pdop(qreal pdop);

  qreal age_of_dgps_data() const;
  void set_age_of_dgps_data(qreal age_of_dgps_data);

  int dgps_id() const;
  void set_dgps_id(int dgps_id);

 private:
  QcGeoElevationCoordinateWGS84 m_coordinate;
  QDateTime m_time;
  qreal m_magnetic_variation;
  qreal m_geoid_height;
  QString m_name;
  QString m_comment;
  QString m_description;
  QString m_source;
  QList<QString> m_link;
  QString m_symbol;
  QString m_type;
  QString m_fix_type;
  int m_number_of_satellites;
  qreal m_hdop;
  qreal m_vdop;
  qreal m_pdop;
  qreal m_age_of_dgps_data;
  unsigned int m_dgps_id;
  // You can add extend GPX by adding your own elements from another schema here.
};

// QC_END_NAMESPACE

#endif /* __WAYPOINT_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
