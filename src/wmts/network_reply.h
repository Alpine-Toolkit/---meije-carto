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

#ifndef __NETWORK_REPLY_H__
#define __NETWORK_REPLY_H__

/**************************************************************************************************/

#include "qtcarto_global.h"

#include <QString>
#include <QObject>

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

// QcNetworkReply is a kind of future

class QC_EXPORT QcNetworkReply : public QObject
{
  Q_OBJECT

 public:
  enum Error { // Fixme: check
    NoError,
    CommunicationError,
    ParseError,
    UnknownError
  };

 public:
  QcNetworkReply();
  QcNetworkReply(Error error, const QString & error_string);
  virtual ~QcNetworkReply();

  /*! Return true if the operation completed successfully or
    encountered an error which cause the operation to come to a halt.
  */
  bool is_finished() const { return m_is_finished; }

  /*! Returns the error state of this reply.

    If the result is QcNetworkReply::NoError then no error has occurred.
  */
  Error error() const { return m_error; }
  /*! Returns the textual representation of the error state of this reply.

    If no error has occurred this will return an empty string.  It is
    possible that an error occurred which has no associated textual
    representation, in which case this will also return an empty
    string.

    To determine whether an error has occurred, check to see if
    QcNetworkReply::error() is equal to QcNetworkReply::NoError.
  */
  QString error_string() const { return m_error_string; }

  //! Returns whether the reply is coming from a cache.
  bool is_cached() const { return m_is_cached; }

  virtual void abort();

 signals:
  void finished();
  void error(Error error, const QString & error_string = QString());

 protected:
  void set_error(Error error, const QString & error_string);
  void set_finished(bool finished);

  //! Sets whether the reply is coming from a cache to \a cached.
  void set_cached(bool cached) { m_is_cached = cached; }

 private:
  Q_DISABLE_COPY(QcNetworkReply);

 private:
  Error m_error;
  QString m_error_string;
  bool m_is_finished;
  bool m_is_cached; // Fixme: purpose ?
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __NETWORK_REPLY_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
