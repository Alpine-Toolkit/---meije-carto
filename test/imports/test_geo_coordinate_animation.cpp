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

#include <cmath>
#include <iostream>

#include <QtTest/QtTest>
#include <QtDebug>

/**************************************************************************************************/

 #include "src/coordinate/wgs84.h"
 #include "src/coordinate/mercator.h"

// #include "imports/QtCarto/geo_coordinate_animation.h"

/**************************************************************************************************/

QcWgsCoordinate
coordinate_shortest_interpolator(const QcWgsCoordinate & _from, const QcWgsCoordinate & _to, qreal progress)
{
  QcNormalisedWebMercatorCoordinate from = _from.normalised_web_mercator();
  QcNormalisedWebMercatorCoordinate to = _to.normalised_web_mercator();

  double to_x = to.x();
  double to_y = to.y();
  double from_x = from.x();
  double from_y = from.y();
  double delta_x = to_x - from_x;
  double delta_y = to_y - from_y;

  /*
   * a) Two cases : |f - t| <= 1/2 or > 1/2
   * if > 1/2 then it is shorter to cross the date line
   *   b) Two cases
   *    |--f   t<-| (3)
   *    |->t   f--| (4)
   */

  double x;
  double delta_x_abs = qAbs(delta_x);
  if (delta_x_abs > 0.5) { // case b
    double dx = 1 - delta_x_abs;
    if (from_x < to_x) { // case 3 <-|-
      x = from_x - dx * progress;
      if (x < 0.0) // we crossed date line
        x += 1.0;
    } else { // case 4 -|->
      x = from_x + dx * progress;
      if (x > 1.0) // we crossed date line
        x -= 1.0;
    }
  } else
    x = from_x + delta_x * progress;

  double y = from_y + delta_y * progress;

  QcWgsCoordinate coordinate = QcNormalisedWebMercatorCoordinate(x, y).wgs84();
  // qInfo() << progress << coordinate;
  return coordinate;
}

QcWgsCoordinate
coordinate_west_interpolator(const QcWgsCoordinate & from, const QcWgsCoordinate & to, qreal progress)
{
  if (progress == .0)
    return from;
  else if (progress == 1.)
    return to;
  else if (from == to)
    return from;

  double to_x = to.x();
  double to_y = to.y();
  double from_x = from.x();
  double from_y = from.y();
  double delta_x = to_x - from_x;
  double delta_y = to_y - from_y;

  // <-|-
  if (from_x < to_x)
    delta_x = 360. - delta_x;
  double delta_x_abs = qAbs(delta_x);
  double x = from_x - delta_x_abs * progress;
  if (x < -180.) // we crossed date line
    x += 360.;

  double y = from_y + delta_y * progress;

  QcWgsCoordinate coordinate(x, y);
  // qInfo() << progress << coordinate;
  return coordinate;
}

QcWgsCoordinate
coordinate_east_interpolator(const QcWgsCoordinate & from, const QcWgsCoordinate & to, qreal progress)
{
  if (progress == .0)
    return from;
  else if (progress == 1.)
    return to;
  else if (from == to)
    return from;

  double to_x = to.x();
  double to_y = to.y();
  double from_x = from.x();
  double from_y = from.y();
  double delta_x = to_x - from_x;
  double delta_y = to_y - from_y;

  // -|->
  if (to_x < from_x)
    delta_x = 360. + delta_x;
  double delta_x_abs = qAbs(delta_x);
  double x = from_x + delta_x_abs * progress;
  if (x > 180.) // we crossed date line
    x -= 360.;

  double y = from_y + delta_y * progress;

  QcWgsCoordinate coordinate(x, y);
  return coordinate;
}

/***************************************************************************************************/

class TestQcGeoCoordinateAnimation: public QObject
{
  Q_OBJECT

private slots:
  void constructor();
};

void TestQcGeoCoordinateAnimation::constructor()
{
  const double longitude_max = 180.; // Fixme: use projection
  const double longitude_step = 10.;
  double progess_step = .1;

  for (double from_longitude = -longitude_max; from_longitude <= longitude_max; from_longitude += longitude_step)
    for (double to_longitude = -longitude_max; to_longitude <= longitude_max; to_longitude += longitude_step) {
      if ((from_longitude != to_longitude) and
          (from_longitude != -longitude_max and to_longitude !=  longitude_max) and
          (from_longitude !=  longitude_max and to_longitude != -longitude_max)
          ) {
        qInfo() << "=========================";
        QcWgsCoordinate from(from_longitude, .0);
        QcWgsCoordinate to(to_longitude, .0);

        QcWgsCoordinate previous = from;
        double previous_delta = .0;
        for (double progress = .0; progress <= 1; progress += progess_step) {
          QcWgsCoordinate coordinate = coordinate_east_interpolator(from, to, progress);
          qInfo() << "east from" << from_longitude << "to" << to_longitude << "progress" << progress << '=' << coordinate;
          if (progress == .0)
            QVERIFY(coordinate == from);
          else if (progress == 1.)
            QVERIFY(coordinate == to);
          double delta = .0;
          if (.0 < progress) {
            delta = coordinate.longitude() - previous.longitude();
            double coordinate_sign = sign(coordinate.longitude());
            double previous_sign = sign(previous.longitude());
            if (previous_sign > coordinate_sign)
              delta += 360.;
            // qInfo() << delta;
          }
          if (previous_delta)
            QVERIFY(qFuzzyCompare(delta, previous_delta));
          previous = coordinate;
          previous_delta = delta;
        }

        qInfo() << "-------------------------";
        previous = from;
        previous_delta = .0;
        for (double progress = .0; progress <= 1; progress += progess_step) {
          QcWgsCoordinate coordinate = coordinate_west_interpolator(from, to, progress);
          qInfo() << "west from" << from_longitude << "to" << to_longitude << "progress" << progress << '=' << coordinate;
          if (progress == .0)
            QVERIFY(coordinate == from);
          else if (progress == 1.)
            QVERIFY(coordinate == to);
          double delta = .0;
          if (.0 < progress) {
            delta = coordinate.longitude() - previous.longitude();
            double coordinate_sign = sign(coordinate.longitude());
            double previous_sign = sign(previous.longitude());
            if (previous_sign < coordinate_sign)
              delta -= 360.;
            // qInfo() << delta;
          }
          if (previous_delta)
            QVERIFY(qFuzzyCompare(delta, previous_delta));
          previous = coordinate;
          previous_delta = delta;
        }
      }
    }
}

/***************************************************************************************************/

QTEST_MAIN(TestQcGeoCoordinateAnimation)
#include "test_geo_coordinate_animation.moc"

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
