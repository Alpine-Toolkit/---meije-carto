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

#ifndef __GEOPORTAIL_WMTS_REPLY_H__
#define __GEOPORTAIL_WMTS_REPLY_H__

/**************************************************************************************************/

#include "wmts/wmts_reply.h"

#include <QNetworkReply>
#include <QPointer>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

// Fixme: check implementation versus QcWmtsReply / QcNetworkReply ... 

class QcGeoportailWmtsReply : public QcWmtsReply
{
  Q_OBJECT

public:
  explicit QcGeoportailWmtsReply(QNetworkReply * reply, const QcTileSpec & spec, const QString & format);
  ~QcGeoportailWmtsReply();

  void abort() override;

  // QNetworkReply * network_reply() const { return m_reply; } // Fixme: purpose ???

private slots:
  void network_reply_finished();
  void network_reply_error(QNetworkReply::NetworkError error);

private:
  QPointer<QNetworkReply> m_reply;
  QString m_format;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __GEOPORTAIL_WMTS_REPLY_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
