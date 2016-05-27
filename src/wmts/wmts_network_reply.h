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

#ifndef __WMTS_NETWORK_REPLY_H__
#define __WMTS_NETWORK_REPLY_H__

/**************************************************************************************************/

#include "wmts/wmts_reply.h"

#include <QNetworkReply>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

class QcWmtsNetworkReply : public QcWmtsReply
{
  Q_OBJECT

public:
  explicit QcWmtsNetworkReply(QNetworkReply * reply, const QcTileSpec & spec, const QString & format);
  ~QcWmtsNetworkReply();

  void process_payload();

private:
  QString m_format;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __WMTS_NETWORK_REPLY_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
