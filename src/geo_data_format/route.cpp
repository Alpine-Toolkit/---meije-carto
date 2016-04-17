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

const QString &
QcRouteMetaData::name() const
{
  return m_name;
}

void
QcRouteMetaData::set_name(const QString & name)
{
  m_name = name;
}

const QString &
QcRouteMetaData::comment() const
{
  return m_comment;
}

void
QcRouteMetaData::set_comment(const QString & comment)
{
  m_comment = comment;
}

const QString &
QcRouteMetaData::description() const
{
  return m_description;
}

void
QcRouteMetaData::set_description(const QString & description)
{
  m_description = description;
}

const QString &
QcRouteMetaData::source() const
{
  return m_source;
}

void
QcRouteMetaData::set_source(const QString & source)
{
  m_source = source;
}

const QString &
QcRouteMetaData::link() const
{
  return m_link;
}

void
QcRouteMetaData::set_link(const QString & link)
{
  m_link = link;
}

unsigned int
QcRouteMetaData::number() const
{
  return m_number;
}

void
QcRouteMetaData::set_number(unsigned int number)
{
  m_number = number;
}

const QString &
QcRouteMetaData::type() const
{
  return m_type;
}

void
QcRouteMetaData::set_type(const QString & type)
{
  m_type = type;
}

/**************************************************************************************************/

QcRoute::QcRoute()
  : QcRouteMetaData(),
    m_points()
{}

QcRoute::QcRoute(const QcRoute & other)
  : QcRouteMetaData(other),
    m_points(other.m_points)
{}

QcRoute::~QcRoute()
{}

QcRoute &
QcRoute::operator=(const QcRoute & other)
{
  if (this != &other) {
    QcRouteMetaData::operator=(other);
    m_points = other.m_points;
  }

  return *this;
}

const QcWayPointList &
QcRoute::points() const
{
  return m_points;
}

void
QcRoute::add_point(const QcWayPoint & point)
{
  m_points.append(point);
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

// const QList<QcWayPoint>
// QcTrack::segments() const
// {
//   return m_segments;
// }

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
