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

#ifndef __TILE_IMAGE_H__
#define __TILE_IMAGE_H__

/**************************************************************************************************/

#include <QString>
#include <QByteArray>

#include "map/wmts/tile_spec.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

QString tile_spec_to_filename(const QcTileSpec & tile_spec, const QString & format, const QString & directory);
QcTileSpec filename_to_tile_spec(const QString & filename);

void write_tile_image(const QString & filename, const QByteArray & bytes);
QByteArray read_tile_image(const QString & filename);

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __TILE_IMAGE_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
