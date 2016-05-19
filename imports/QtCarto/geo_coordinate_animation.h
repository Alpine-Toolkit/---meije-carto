// -*- mode: c++ -*-

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

// cf. qtlocation/src/imports/positioning/qquickgeocoordinateanimation_p.h

/**************************************************************************************************/

#ifndef __GEO_COORDINATE_ANIMATION_H__
#define __GEO_COORDINATE_ANIMATION_H__

/**************************************************************************************************/

// #include <QGeoCoordinate>
#include "coordinate/geo_coordinate.h"

// Fixme: Use QtQuick Private API !!!
#include <private/qquickanimation_p.h>
#include <private/qquickanimation_p_p.h>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcGeoCoordinateAnimationPrivate;

class QcGeoCoordinateAnimation : public QQuickPropertyAnimation
{
  Q_OBJECT
  Q_DECLARE_PRIVATE(QcGeoCoordinateAnimation)
  Q_PROPERTY(QcGeoCoordinateWGS84 from READ from WRITE setFrom)
  Q_PROPERTY(QcGeoCoordinateWGS84 to READ to WRITE setTo)
  Q_PROPERTY(Direction direction READ direction WRITE setDirection NOTIFY directionChanged)

public:
  enum Direction {
    Shortest,
    West,
    East
  };
  Q_ENUM(Direction)

  QcGeoCoordinateAnimation(QObject * parent = 0);
  ~QcGeoCoordinateAnimation();

  QcGeoCoordinateWGS84 from() const;
  void setFrom(const QcGeoCoordinateWGS84 &);

  QcGeoCoordinateWGS84 to() const;
  void setTo(const QcGeoCoordinateWGS84 &);

  Direction direction() const;
  void setDirection(Direction direction);

signals:
  void directionChanged();
};

class QcGeoCoordinateAnimationPrivate : public QQuickPropertyAnimationPrivate
{
  Q_DECLARE_PUBLIC(QcGeoCoordinateAnimation)

public:
  QcGeoCoordinateAnimationPrivate();
  QcGeoCoordinateAnimation::Direction m_direction;
};

// QVariant coordinate_interpolator(const QcGeoCoordinateWGS84 & from, const QcGeoCoordinateWGS84 & to, qreal progress);

// QC_END_NAMESPACE

QML_DECLARE_TYPE(QcGeoCoordinateAnimation)

#endif // __GEO_COORDINATE_ANIMATION_H__

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
