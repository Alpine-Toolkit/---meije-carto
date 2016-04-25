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

/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

/**************************************************************************************************/

#include "tile_spec.h"

#include <QtCore/QDebug>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

QcTileSpec::QcTileSpec()
  : QcTileMatrixIndex(),
    m_plugin(), m_map_id(-1), m_level(-1)
{}

QcTileSpec::QcTileSpec(const QString & plugin, int map_id, int level, int x, int y)
  : QcTileMatrixIndex(x, y),
    m_plugin(plugin), m_map_id(map_id), m_level(level)
{}

QcTileSpec::QcTileSpec(const QcTileSpec & other)
  : QcTileMatrixIndex(other),
    m_plugin(other.m_plugin), m_map_id(other.m_map_id), m_level(other.m_level)
{}

QcTileSpec::~QcTileSpec()
{}

QcTileSpec &
QcTileSpec::operator=(const QcTileSpec & other)
{
  if (this != &other) {
    QcTileMatrixIndex::operator=(other);
    m_plugin = other.m_plugin;
    m_map_id = other.m_map_id;
    m_level = other.m_level;
  }

  return *this;
}

bool
QcTileSpec::operator==(const QcTileSpec & rhs) const
{
  return (QcTileMatrixIndex::operator==(rhs)
	  && m_plugin == rhs.m_plugin
	  && m_map_id == rhs.m_map_id
	  && m_level == rhs.m_level);
}

/*
bool QcTileSpec::operator<(const QcTileSpec & rhs) const
{
  if (plugin_ < rhs.plugin_)
    return true;
  if (plugin_ > rhs.plugin_)
    return false;

  if (mapId_ < rhs.mapId_)
    return true;
  if (mapId_ > rhs.mapId_)
    return false;

  if (zoom_ < rhs.zoom_)
    return true;
  if (zoom_ > rhs.zoom_)
    return false;

  if (x_ < rhs.x_)
    return true;
  if (x_ > rhs.x_)
    return false;

  if (y_ < rhs.y_)
    return true;
  if (y_ > rhs.y_)
    return false;

  return (version_ < rhs.version_);
}
*/

unsigned int
qHash(const QcTileSpec & tile_spec)
{
  unsigned int result = (qHash(tile_spec.plugin()) * 13) % 31;
  result += ((tile_spec.map_id() * 17) % 31) << 5;
  result += ((tile_spec.level() * 19) % 31) << 10;
  result += ((tile_spec.x() * 23) % 31) << 15;
  result += ((tile_spec.y() * 29) % 31) << 20;
  return result;
}

QDebug
operator<<(QDebug debug, const QcTileSpec & tile_spec)
{
  // Fixme: litteral, arg OR s <<
  QString s = tile_spec.plugin();
  s += QLatin1Literal("-");
  s += QString::number(tile_spec.map_id());
  s += QLatin1Literal("-");
  s += QString::number(tile_spec.level());
  s += QLatin1Literal("-");
  s += QString::number(tile_spec.x());
  s += QLatin1Literal("-");
  s += QString::number(tile_spec.y());
  debug << s;
  return debug;
}

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
