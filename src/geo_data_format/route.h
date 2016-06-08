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

#ifndef __QcRoute_H__
#define __QcRoute_H__

/**************************************************************************************************/

#include <QString>
#include <QList>

#include "waypoint.h"

/**************************************************************************************************/

/* <rte>
 *     <name> xsd:string </name> [0..1]
 *     <cmt> xsd:string </cmt> [0..1]
 *     <desc> xsd:string </desc> [0..1]
 *     <src> xsd:string </src> [0..1]
 *     <link> linkType </link> [0..*]
 *     <number> xsd:nonNegativeInteger </number> [0..1]
 *     <type> xsd:string </type> [0..1]
 *     <extensions> extensionsType </extensions> [0..1]
 *     <rtept> wptType </rtept> [0..*]
 * </rte>
 *
 * <trk>
 *     <name> xsd:string </name> [0..1]
 *     <cmt> xsd:string </cmt> [0..1]
 *     <desc> xsd:string </desc> [0..1]
 *     <src> xsd:string </src> [0..1]
 *     <link> linkType </link> [0..*]
 *     <number> xsd:nonNegativeInteger </number> [0..1]
 *     <type> xsd:string </type> [0..1]
 *     <extensions> extensionsType </extensions> [0..1]
 *     <trkseg>
 *         <trkpt> wptType </trkpt> [0..*]
 *         <extensions> extensionsType </extensions> [0..1]
 *     </trkseg> [0..*]
 * </trk>
 */

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcRouteMetaData
{
public:
  QcRouteMetaData(); // QObject * parent = 0
  QcRouteMetaData(const QcRouteMetaData & other);
  ~QcRouteMetaData();

  QcRouteMetaData & operator=(const QcRouteMetaData & other);

  const QString & name() const { return m_name; }
  void set_name(const QString & name) { m_name = name; }

  const QString & comment() const { return m_comment; }
  void set_comment(const QString & comment) { m_comment = comment; }

  const QString & description() const { return m_description; }
  void set_description(const QString & description) { m_description = description; }

  const QString & source() const { return m_source; }
  void set_source(const QString & source) { m_source = source; }

  const QString & link() const { return m_link; }
  void set_link(const QString & link) { m_link = link; }

  unsigned int number() const { return m_number; }
  void set_number(unsigned int number) { m_number = number; }

  const QString & type() const { return m_type; }
  void set_type(const QString & type) { m_type = type; }

private:
  QString m_name;
  QString m_comment;
  QString m_description;
  QString m_source;
  QString m_link;
  unsigned int m_number;
  QString m_type;
};

/**************************************************************************************************/

typedef QList<QcWayPoint> QcWayPointList;

class QcRoute : public QcRouteMetaData
{
public:
  QcRoute(); // QObject * parent = 0
  QcRoute(const QcRoute & other);
  ~QcRoute();

  QcRoute & operator=(const QcRoute & other);

  const QcWayPointList & waypoints() const { return m_waypoints; }
  void add_waypoint(const QcWayPoint & waypoint);

private:
  QcWayPointList m_waypoints;
};

/**************************************************************************************************/

class QcTrack : public QcRouteMetaData
{
public:
  QcTrack(); // QObject * parent = 0
  QcTrack(const QcTrack & other);
  ~QcTrack();

  QcTrack & operator=(const QcTrack & other);

  const QList<QcWayPointList> & segments() const { return m_segments; }
  void add_segment(const QcWayPointList & segment);

private:
  QList<QcWayPointList> m_segments;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

#endif /* __QcRoute_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
