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

#ifndef __LOCATION_SERVICE_REPLY_H__
#define __LOCATION_SERVICE_REPLY_H__

/**************************************************************************************************/

#include "wmts/location_service_query.h"
#include "wmts/network_reply.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcLocationServiceReply : public QcNetworkReply
{
  Q_OBJECT

public:
  explicit QcLocationServiceReply(QNetworkReply * reply, const QcLocationServiceQuery & query);
  ~QcLocationServiceReply();

private:
  const QcLocationServiceQuery & m_query;
};

/**************************************************************************************************/

class QcLocationServiceReverseReply : public QcNetworkReply
{
  Q_OBJECT

public:
  explicit QcLocationServiceReverseReply(QNetworkReply * reply, const QcLocationServiceReverseQuery & query);
  ~QcLocationServiceReverseReply();

private:
  const QcLocationServiceReverseQuery & m_query;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __LOCATION_SERVICE_REPLY_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
