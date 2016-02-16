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

#include <QDebug>

#include "map/mosaic_painter.h"

/**************************************************************************************************/

QcMosaicPainter::QcMosaicPainter(const QcViewport & viewport)
  : m_viewport(viewport)
{
  QObject::connect(&viewport, &QcViewport::viewport_changed,
		   this, &QcMosaicPainter::viewport_changed);
}

QcMosaicPainter::~QcMosaicPainter()
{}

void
QcMosaicPainter::viewport_changed()
{
  qInfo() << "QcMosaicPainter:.viewport_changed";
}

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
