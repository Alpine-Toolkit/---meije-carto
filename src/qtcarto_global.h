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

#ifndef QTCARTO_GLOBAL_H
#define QTCARTO_GLOBAL_H

/**************************************************************************************************/

#include <QtCore/qglobal.h>

/**************************************************************************************************/

// QtCarto Version
#define QTCARTO_VERSION_MAJOR 1
#define QTCARTO_VERSION_MINOR 0
#define QTCARTO_VERSION_PATCH 0

// Can be used like #if (QTCARTO_VERSION >= QTCARTO_VERSION_CHECK(4, 4, 0))
#define QTCARTO_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))
// QTCARTO_VERSION is (major << 16) + (minor << 8) + patch.
#define QTCARTO_VERSION QTCARTO_VERSION_CHECK(QTCARTO_VERSION_MAJOR, QTCARTO_VERSION_MINOR, QTCARTO_VERSION_PATCH)

#if defined(QTCARTO_LIBRARY)
#  define QC_EXPORT Q_DECL_EXPORT
#else
#  define QC_EXPORT Q_DECL_IMPORT
#endif

// cf. qglobal.h
#define QTCARTO_USE_NAMESPACE using namespace ::QTCARTO_NAMESPACE;
#define QTCARTO_BEGIN_NAMESPACE namespace QTARTO_NAMESPACE {
#define QTCARTO_END_NAMESPACE }

#endif // QTCARTO_GLOBAL_H

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
