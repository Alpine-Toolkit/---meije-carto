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

#ifndef __GEOPORTAIL_LICENSE_H__
#define __GEOPORTAIL_LICENSE_H__

/**************************************************************************************************/

#include <QString>
#include <QJsonObject>

#include "qtcarto_global.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QC_EXPORT QcGeoportailWmtsLicense
{
 public:
  QcGeoportailWmtsLicense(const QString & json_path);
  QcGeoportailWmtsLicense(const QString & user, const QString & password, const QString & api_key,
			  unsigned int offline_cache_limit);
  QcGeoportailWmtsLicense(const QcGeoportailWmtsLicense & other);

  QcGeoportailWmtsLicense & operator=(const QcGeoportailWmtsLicense & other);

  inline const QString & user() const { return m_user; }
  inline const QString & password() const { return m_password; }
  inline const QString & api_key() const { return m_api_key; }
  inline unsigned int offline_cache_limit() const { return m_offline_cache_limit; }

  bool operator==(const QcGeoportailWmtsLicense & rhs) const;

 private:
  void load_json(const QString & json_path);
  void read_json(const QJsonObject & json);

 private:
  QString m_user;
  QString m_password;
  QString m_api_key;
  unsigned int m_offline_cache_limit;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __GEOPORTAIL_LICENSE_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
