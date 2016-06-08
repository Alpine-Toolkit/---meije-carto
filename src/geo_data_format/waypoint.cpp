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

#include "waypoint.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcWayPoint::QcWayPoint()
  : m_coordinate(),
    m_time(),
    m_magnetic_variation(),
    m_geoid_height(),
    m_name(),
    m_comment(),
    m_description(),
    m_source(),
    m_link(),
    m_symbol(),
    m_type(),
    m_fix_type(),
    m_number_of_satellites(),
    m_hdop(),
    m_vdop(),
    m_pdop(),
    m_age_of_dgps_data(),
    m_dgps_id()
{}

QcWayPoint::QcWayPoint(const QcWayPoint & other)
  : m_coordinate(other.m_coordinate),
    m_time(other.m_time),
    m_magnetic_variation(other.m_magnetic_variation),
    m_geoid_height(other.m_geoid_height),
    m_name(other.m_name),
    m_comment(other.m_comment),
    m_description(other.m_description),
    m_source(other.m_source),
    m_link(other.m_link),
    m_symbol(other.m_symbol),
    m_type(other.m_type),
    m_fix_type(other.m_fix_type),
    m_number_of_satellites(other.m_number_of_satellites),
    m_hdop(other.m_hdop),
    m_vdop(other.m_vdop),
    m_pdop(other.m_pdop),
    m_age_of_dgps_data(other.m_age_of_dgps_data),
    m_dgps_id(other.m_dgps_id)
{}

QcWayPoint::~QcWayPoint()
{}

QcWayPoint &
QcWayPoint::operator=(const QcWayPoint & other)
{
  if (this != &other) {
    m_coordinate = other.m_coordinate;
    m_time = other.m_time;
    m_magnetic_variation = other.m_magnetic_variation;
    m_geoid_height = other.m_geoid_height;
    m_name = other.m_name;
    m_comment = other.m_comment;
    m_description = other.m_description;
    m_source = other.m_source;
    m_link = other.m_link;
    m_symbol = other.m_symbol;
    m_type = other.m_type;
    m_fix_type = other.m_fix_type;
    m_number_of_satellites = other.m_number_of_satellites;
    m_hdop = other.m_hdop;
    m_vdop = other.m_vdop;
    m_pdop = other.m_pdop;
    m_age_of_dgps_data = other.m_age_of_dgps_data;
    m_dgps_id = other.m_dgps_id;
  }

   return *this;
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
