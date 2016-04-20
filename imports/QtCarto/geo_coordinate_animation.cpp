/****************************************************************************
 **
 ** Copyright (C) 2016 The Qt Company Ltd.
 ** Contact: https://www.qt.io/licensing/
 **
 ** This file is part of the QtPositioning module of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and The Qt Company. For licensing terms
 ** and conditions see https://www.qt.io/terms-conditions. For further
 ** information use the contact form at https://www.qt.io/contact-us.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 3 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL3 included in the
 ** packaging of this file. Please review the following information to
 ** ensure the GNU Lesser General Public License version 3 requirements
 ** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 2.0 or (at your option) the GNU General
 ** Public license version 3 or any later version approved by the KDE Free
 ** Qt Foundation. The licenses are as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file. Please review the following
 ** information to ensure the GNU General Public License requirements will
 ** be met: https://www.gnu.org/licenses/gpl-2.0.html and
 ** https://www.gnu.org/licenses/gpl-3.0.html.
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

/**************************************************************************************************/

#include "map/geo_coordinate.h"
#include "geo_coordinate_animation.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/*!
  \qmltype CoordinateAnimation
  \instantiates QcGeoCoordinateAnimation
  \inherits PropertyAnimation
  \inqmlmodule QtPositioning
  \since 5.3

  \brief A PropertyAnimation for geo coordinate properties.

  A specialized \l{PropertyAnimation} that defines an animation
  between two \l{coordinate}{coordinates}.

  By default, a \l{latitude} of the \l{coordinate} is animated in the direction of shortest
  (geodesic) distance between those coordinates. Since CoordinateAnimation uses Mercator
  map projection, the \l{latitude} animation is always between -90 and 90 degrees.
  The \l{longitude} animation path is not limited and can go over 180 degrees
  in both west and east directions.

  The \l{direction} property can be set to specify the direction in which the \l{longitude}
  animation should occur.

  \sa {Animation and Transitions in Qt Quick}
*/

/**************************************************************************************************/

/*
QVariant
coordinate_interpolator(const QGeoCoordinate & from, const QGeoCoordinate & to, qreal progress)
{
  if (from == to) { // ???
    if (progress < 0.5)
      return QVariant::fromValue(from);
    else
      return QVariant::fromValue(to);
  }

  QGeoCoordinate result = QGeoProjection::coordinateInterpolation(from, to, progress);

  return QVariant::fromValue(result);
}
*/

/**************************************************************************************************/

QVariant
coordinate_shortest_interpolator(const QGeoCoordinate & _from, const QGeoCoordinate & _to, qreal progress)
{
  QcGeoCoordinateNormalisedWebMercator from = QcGeoCoordinateWGS84(_from.longitude(), _from.latitude()).normalised_web_mercator();
  QcGeoCoordinateNormalisedWebMercator to = QcGeoCoordinateWGS84(_to.longitude(), _to.latitude()).normalised_web_mercator();

  double to_x = to.x();
  double to_y = to.y();
  double from_x = from.x();
  double from_y = from.y();
  double delta_x = to_x - from_x;

  double x;
  if (0.5 < qAbs(delta_x)) {
    // handle dateline crossing
    double ex = to_x; // ex and sx unused ???
    double sx = from_x;
    if (ex < sx)
      sx -= 1.0;
    else if (sx < ex)
      ex -= 1.0;
    x = from_x + delta_x * progress;
    if (x < 0.0)
      x += 1.0;
  } else {
    x = from_x + delta_x * progress;
  }

  double y = from_y + (to_y - from_y) * progress;

  QcGeoCoordinateNormalisedWebMercator normalised(x, y);
  QcGeoCoordinateWGS84 wgs84 = normalised.wgs84();
  QGeoCoordinate coordinate(wgs84.latitude(), wgs84.longitude());
  return QVariant::fromValue(coordinate);
}

QVariant
coordinate_west_interpolator(const QGeoCoordinate & _from, const QGeoCoordinate & _to, qreal progress)
{
  QcGeoCoordinateNormalisedWebMercator from = QcGeoCoordinateWGS84(_from.longitude(), _from.latitude()).normalised_web_mercator();
  QcGeoCoordinateNormalisedWebMercator to = QcGeoCoordinateWGS84(_to.longitude(), _to.latitude()).normalised_web_mercator();

  double to_x = to.x();
  double to_y = to.y();
  double from_x = from.x();
  double from_y = from.y();
  double delta_x = to_x - from_x;

  while (delta_x < 0.0) {
    to_x += 1.0;
    delta_x += 1.0;
  }

  double x = from_x + delta_x * progress;
  double y = from_y + (to_y - from_y) * progress;

  while (x > 1.0)
    x -= 1.0;

  QcGeoCoordinateNormalisedWebMercator normalised(x, y);
  QcGeoCoordinateWGS84 wgs84 = normalised.wgs84();
  QGeoCoordinate coordinate(wgs84.latitude(), wgs84.longitude());
  return QVariant::fromValue(coordinate);
}

QVariant
coordinate_east_interpolator(const QGeoCoordinate & _from, const QGeoCoordinate & _to, qreal progress)
{
  QcGeoCoordinateNormalisedWebMercator from = QcGeoCoordinateWGS84(_from.longitude(), _from.latitude()).normalised_web_mercator();
  QcGeoCoordinateNormalisedWebMercator to = QcGeoCoordinateWGS84(_to.longitude(), _to.latitude()).normalised_web_mercator();

  double to_x = to.x();
  double to_y = to.y();
  double from_x = from.x();
  double from_y = from.y();
  double delta_x = to_x - from_x;

  while (delta_x > 0.0) {
    to_x -= 1.0;
    delta_x -= 1.0;
  }

  double x = from_x + delta_x * progress;
  double y = from_y + (to_y - from_y) * progress;

  while (x < 0.0)
    x += 1.0;

  QcGeoCoordinateNormalisedWebMercator normalised(x, y);
  QcGeoCoordinateWGS84 wgs84 = normalised.wgs84();
  QGeoCoordinate coordinate(wgs84.latitude(), wgs84.longitude());
  return QVariant::fromValue(coordinate);
}

/**************************************************************************************************/

QcGeoCoordinateAnimationPrivate::QcGeoCoordinateAnimationPrivate()
  : m_direction(QcGeoCoordinateAnimation::Shortest)
{
}

/**************************************************************************************************/

QcGeoCoordinateAnimation::QcGeoCoordinateAnimation(QObject * parent)
  : QQuickPropertyAnimation(*(new QcGeoCoordinateAnimationPrivate), parent) // ctor prototype ??? when deleted ???
{
  Q_D(QcGeoCoordinateAnimation);
  d->interpolatorType = qMetaTypeId<QGeoCoordinate>();
  d->defaultToInterpolatorType = true;
  d->interpolator = QVariantAnimationPrivate::getInterpolator(d->interpolatorType);
}

QcGeoCoordinateAnimation::~QcGeoCoordinateAnimation()
{
}

/*!
  \qmlproperty coordinate CoordinateAnimation::from
  This property holds the coordinate where the animation should begin.
*/
QGeoCoordinate
QcGeoCoordinateAnimation::from() const
{
  Q_D(const QcGeoCoordinateAnimation);
  return d->from.value<QGeoCoordinate>();
}

void QcGeoCoordinateAnimation::setFrom(const QGeoCoordinate & from)
{
  QQuickPropertyAnimation::setFrom(QVariant::fromValue(from));
}

/*!
  \qmlproperty coordinate CoordinateAnimation::to
  This property holds the coordinate where the animation should end.
*/
QGeoCoordinate
QcGeoCoordinateAnimation::to() const
{
  Q_D(const QcGeoCoordinateAnimation);
  return d->to.value<QGeoCoordinate>();
}

void
QcGeoCoordinateAnimation::setTo(const QGeoCoordinate & to)
{
  QQuickPropertyAnimation::setTo(QVariant::fromValue(to));
}

/*!
  \qmlproperty enumeration CoordinateAnimation::direction
  This property holds the direction of the \l{longitude} animation of the \l{coordinate}.

  Possible values are:

  \list
  \li CoordinateAnimation.Shortest (default) - the longitude animation goes in the direction
  that produces the shortest animation path.
  \li CoordinateAnimation.West - the longitude animation always goes into western direction
  and may cross the date line.
  \li CoordinateAnimation.East - the longitude animation always goes into eastern direction
  and may cross the date line.
  \endlist
  \since 5.5
*/

QcGeoCoordinateAnimation::Direction
QcGeoCoordinateAnimation::direction() const
{
  Q_D(const QcGeoCoordinateAnimation);
  return d->m_direction;
}

void
QcGeoCoordinateAnimation::setDirection(QcGeoCoordinateAnimation::Direction direction)
{
  Q_D(QcGeoCoordinateAnimation);
  if (d->m_direction == direction)
    return;

  d->m_direction = direction;
  switch (direction) {
  case West:
    d->interpolator = reinterpret_cast<QVariantAnimation::Interpolator>(&coordinate_west_interpolator);
    break;
  case East:
    d->interpolator = reinterpret_cast<QVariantAnimation::Interpolator>(&coordinate_east_interpolator);
    break;
  case Shortest:
  default:
    d->interpolator = reinterpret_cast<QVariantAnimation::Interpolator>(&coordinate_shortest_interpolator);
    break;
  }
  emit directionChanged();
}

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
