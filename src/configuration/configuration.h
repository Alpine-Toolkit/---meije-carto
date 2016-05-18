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

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

/**************************************************************************************************/

#include <QString>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcConfig
{
public:
  static QcConfig & instance() {
    // Thread-safe in C++11
    static QcConfig m_instance;
    return m_instance;
  }

  // Delete copy and move constructors and assign operators
  QcConfig(QcConfig const &) = delete;             // Copy construct
  QcConfig & operator=(QcConfig const &) = delete; // Copy assign
  QcConfig(QcConfig &&) = delete;                  // Move construct
  QcConfig & operator=(QcConfig &&) = delete;      // Move assign

  const QString & application_user_directory() const { return m_application_user_directory; }
  const QString join_application_user_directory(const QString & path);

  void init();

private:
  QcConfig();
  ~QcConfig();
  void create_user_application_directory() const;

private:
  bool m_initialised;
  QString m_application_user_directory;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

#endif /* __CONFIGURATION_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
