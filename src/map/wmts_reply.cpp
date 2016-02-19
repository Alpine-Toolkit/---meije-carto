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

#include "map/wmts_reply.h"

#include <QDebug>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/*!
    \class QcWmtsReply
    \inmodule QtLocation
    \ingroup QtLocation-impl
    \since 5.6
    \internal

    \brief The QcWmtsReply class manages a tile fetch operation started
    by an instance of QGeoTiledManagerEngine.

    Instances of QcWmtsReply manage the state and results of these
    operations.

    The isFinished(), error() and errorString() methods provide information
    on whether the operation has completed and if it completed successfully.

    The finished() and error(QcWmtsReply::Error,QString)
    signals can be used to monitor the progress of the operation.

    It is possible that a newly created QcWmtsReply may be in a finished
    state, most commonly because an error has occurred. Since such an instance
    will never emit the finished() or
    error(QcWmtsReply::Error,QString) signals, it is
    important to check the result of isFinished() before making the connections
    to the signals.

    If the operation completes successfully the results are accessed by
    mapImageData() and mapImageFormat().
*/

/*!
    \enum QcWmtsReply::Error

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
QcWmtsReply::QcWmtsReply(const QcTileSpec & tile_spec, QObject * parent)
  : QObject(parent),
    m_error(QcWmtsReply::NoError),
    m_is_finished(false),
    m_is_cached(false),
    m_tile_spec(tile_spec)
{}

/*!
  Constructs a tiled map reply object with a given \a error and \a errorString and the specified \a parent.
*/
QcWmtsReply::QcWmtsReply(Error error, const QString & error_string, QObject * parent)
  : QObject(parent),
    m_error(error),
    m_error_string(error_string),
    m_is_finished(true),
    m_is_cached(false)
{}

/*!
  Destroys this tiled map reply object.
*/
QcWmtsReply::~QcWmtsReply()
{}

/*!
  Sets whether or not this reply has finished to \a finished.

  If \a finished is true, this will cause the finished() signal to be
  emitted.

  If the operation completed successfully,
  QcWmtsReply::setMapImageData() should be called before this
  function. If an error occurred, QcWmtsReply::setError() should be used
  instead.
*/
void QcWmtsReply::set_finished(bool finished)
{
  m_is_finished = finished;
  if (m_is_finished)
    emit this->finished();
}

/*!
  Return true if the operation completed successfully or encountered an
  error which cause the operation to come to a halt.
*/
bool
QcWmtsReply::is_finished() const
{
  return m_is_finished;
}

/*!
  Sets the error state of this reply to \a error and the textual
  representation of the error to \a errorString.

  This will also cause error() and finished() signals to be emitted, in that
  order.
*/
void
QcWmtsReply::set_error(QcWmtsReply::Error error, const QString & error_string)
{
  m_error = error;
  m_error_string = error_string;
  emit this->error(error, error_string);
  set_finished(true);
}

/*!
  Returns the error state of this reply.

  If the result is QcWmtsReply::NoError then no error has occurred.
*/
QcWmtsReply::Error
QcWmtsReply::error() const
{
  return m_error;
}

/*!
  Returns the textual representation of the error state of this reply.

  If no error has occurred this will return an empty string.  It is possible
  that an error occurred which has no associated textual representation, in
  which case this will also return an empty string.

  To determine whether an error has occurred, check to see if
  QcWmtsReply::error() is equal to QcWmtsReply::NoError.
*/
QString
QcWmtsReply::error_string() const
{
  return m_error_string;
}

/*!
  Returns whether the reply is coming from a cache.
*/
bool
QcWmtsReply::is_cached() const
{
  return m_is_cached;
}

/*!
  Sets whether the reply is coming from a cache to \a cached.
*/
void
QcWmtsReply::set_cached(bool cached)
{
  m_is_cached = cached;
}

/*!
  Returns the request which corresponds to this reply.
*/
QcTileSpec
QcWmtsReply::tile_spec() const
{
  return m_tile_spec;
}

/*!
  Returns the tile image data.
*/
QByteArray
QcWmtsReply::map_image_data() const
{
  return m_map_image_data;
}

/*!
  Sets the tile image data to \a data.
*/
void
QcWmtsReply::set_map_image_data(const QByteArray & data)
{
  m_map_image_data = data;
}

/*!
  Returns the format of the tile image.
*/
QString
QcWmtsReply::map_image_format() const
{
  return m_map_image_format;
}

/*!
  Sets the format of the tile image to \a format.
*/
void
QcWmtsReply::set_map_image_format(const QString & format)
{
  m_map_image_format = format;
}

/*!
  Cancels the operation immediately.

  This will do nothing if the reply is finished.
*/
void
QcWmtsReply::abort()
{
  if (!is_finished())
    set_finished(true);
}

// #include "moc_qgeotiledmapreply_p.cpp"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
