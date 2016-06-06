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

#include "debug_tools.h"

#include "wgs84.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QString
format_interval(const QcInterval2DDouble & interval, const QcProjection * projection)
{
  const QcIntervalDouble & x = interval.x();
  const QcIntervalDouble & y = interval.y();
  QcGeoCoordinate inf(projection, x.inf(), y.inf());
  QcGeoCoordinate sup(projection, x.sup(), y.sup());
  QcGeoCoordinate wgs84_inf = inf.transform(&QcWgsCoordinate::cls_projection); // Fixme: radians !
  QcGeoCoordinate wgs84_sup = sup.transform(&QcWgsCoordinate::cls_projection);

  QString message =
    '[' + QString::number(static_cast<int>(x.inf())) + QStringLiteral(", ") + QString::number(static_cast<int>(x.sup())) + QStringLiteral("]") +
    QStringLiteral(" x ") +
    '[' + QString::number(static_cast<int>(y.inf())) + QStringLiteral(", ") + QString::number(static_cast<int>(y.sup())) + QStringLiteral("]") +
    QStringLiteral("   ")  +
    '[' + QString::number(wgs84_inf.x()) + QStringLiteral(", ") + QString::number(wgs84_sup.x()) + QStringLiteral("]");
  return message;
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
