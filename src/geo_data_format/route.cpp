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

#include "route.h"

/**************************************************************************************************/

QcRouteMetaData::QcRouteMetaData()
  : m_name(),
    m_comment(),
    m_description(),
    m_source(),
    m_link(),
    m_number(),
    m_type()
{}

QcRouteMetaData::QcRouteMetaData(const QcRouteMetaData & other)
  : m_name(other.m_name),
    m_comment(other.m_comment),
    m_description(other.m_description),
    m_source(other.m_source),
    m_link(other.m_link),
    m_number(other.m_number),
    m_type(other.m_type)
{}

QcRouteMetaData::~QcRouteMetaData()
{}

QcRouteMetaData &
QcRouteMetaData::operator=(const QcRouteMetaData & other)
{
  if (this != &other) {
    m_name = other.m_name;
    m_comment = other.m_comment;
    m_description = other.m_description;
    m_source = other.m_source;
    m_link = other.m_link;
    m_number = other.m_number;
    m_type = other.m_type;
  }

  return *this;
}

/**************************************************************************************************/

QcRoute::QcRoute()
  : QcRouteMetaData(),
    m_waypoints()
{}

QcRoute::QcRoute(const QcRoute & other)
  : QcRouteMetaData(other),
    m_waypoints(other.m_waypoints)
{}

QcRoute::~QcRoute()
{}

QcRoute &
QcRoute::operator=(const QcRoute & other)
{
  if (this != &other) {
    QcRouteMetaData::operator=(other);
    m_waypoints = other.m_waypoints;
  }

  return *this;
}

void
QcRoute::add_waypoint(const QcWayPoint & waypoint)
{
  m_waypoints.append(waypoint);
}

/**************************************************************************************************/

QcTrack::QcTrack()
  : QcRouteMetaData(),
    m_segments()
{}

QcTrack::QcTrack(const QcTrack & other)
  : QcRouteMetaData(other),
    m_segments(other.m_segments)
{}

QcTrack::~QcTrack()
{}

QcTrack &
QcTrack::operator=(const QcTrack & other)
{
  if (this != &other) {
    QcRouteMetaData::operator=(other);
    m_segments = other.m_segments;
  }

  return *this;
}

void
QcTrack::add_segment(const QcWayPointList & segment)
{
  m_segments.append(segment);
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
