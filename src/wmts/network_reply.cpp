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

#include "network_reply.h"

#include <QDebug>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/*!
    \class QcNetworkReply

    \brief The QcNetworkReply class manages a ...

    Instances of QcNetworkReply manage the state and results of these
    operations.

    The isFinished(), error() and errorString() methods provide information
    on whether the operation has completed and if it completed successfully.

    The finished() and error(QcNetworkReply::Error,QString)
    signals can be used to monitor the progress of the operation.

    It is possible that a newly created QcNetworkReply may be in a finished
    state, most commonly because an error has occurred. Since such an instance
    will never emit the finished() or
    error(QcNetworkReply::Error,QString) signals, it is
    important to check the result of isFinished() before making the connections
    to the signals.
*/

/*!
    \enum QcNetworkReply::Error

    Describes an error which prevented the completion of the operation.

    \value NoError
        No error has occurred.
    \value CommunicationError
        An error occurred while communicating with the service provider.
    \value ParseError
        The response from the service provider was in an unrecognizable format
        supported by the service provider.
    \value UnknownError
        An error occurred which does not fit into any of the other categories.
*/

/*!
    Constructs a tiled map reply object based on \a request,  with parent \a parent.
*/
QcNetworkReply::QcNetworkReply()
  : QObject(),
    m_error(QcNetworkReply::NoError),
    m_is_finished(false),
    m_is_cached(false)
{}

/*!
  Constructs a tiled map reply object with a given \a error and \a errorString and the specified \a parent.
*/
QcNetworkReply::QcNetworkReply(Error error, const QString & error_string)
  : QObject(),
    m_error(error),
    m_error_string(error_string),
    m_is_finished(true),
    m_is_cached(false)
{}

/*!
  Destroys this tiled map reply object.
*/
QcNetworkReply::~QcNetworkReply()
{}

/*!
  Sets whether or not this reply has finished to \a finished.

  If \a finished is true, this will cause the finished() signal to be
  emitted.

  If the operation completed successfully,
  QcNetworkReply::setMapImageData() should be called before this
  function. If an error occurred, QcNetworkReply::setError() should be used
  instead.
*/
void
QcNetworkReply::set_finished(bool finished)
{
  m_is_finished = finished;
  if (m_is_finished)
    emit this->finished();
}

/*!
  Sets the error state of this reply to \a error and the textual
  representation of the error to \a errorString.

  This will also cause error() and finished() signals to be emitted, in that
  order.
*/
void
QcNetworkReply::set_error(QcNetworkReply::Error error, const QString & error_string)
{
  m_error = error;
  m_error_string = error_string;
  emit this->error(error, error_string);
  set_finished(true);
}

/*!
  Cancels the operation immediately.

  This will do nothing if the reply is finished.
*/
void
QcNetworkReply::abort()
{
  if (!is_finished())
    set_finished(true);
}

/**************************************************************************************************/

// #include "network_reply.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
