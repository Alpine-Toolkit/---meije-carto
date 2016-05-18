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

#include "configuration.h"

#include <QDir>
#include <QStandardPaths>
#include <QtDebug>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

QcConfig::QcConfig()
  : m_initialised(false)
{
  // on Android
  //   DataLocation = /data/data/org.alpine_toolkit
  //   GenericDataLocation = <USER> = /storage/emulated/0 = user land root
  // on Linux
  //   GenericDataLocation = /home/fabrice/.local/share
  QString generic_data_location_path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
  m_application_user_directory = QDir(generic_data_location_path).filePath(QLatin1Literal("alpine-toolkit"));
}

QcConfig::~QcConfig()
{}

void
QcConfig::init()
{
  if (m_initialised)
    return;

  create_user_application_directory();

  m_initialised = true;
}

void
QcConfig::create_user_application_directory() const
{
  QDir directory = m_application_user_directory;
  if (! directory.exists())
    if (!directory.mkpath(directory.absolutePath()))
      qWarning() << "Cannot create application user directory" << m_application_user_directory;
}

const QString
QcConfig::join_application_user_directory(const QString & path)
{
  return QDir(m_application_user_directory).absoluteFilePath(path);
}

/**************************************************************************************************/

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
