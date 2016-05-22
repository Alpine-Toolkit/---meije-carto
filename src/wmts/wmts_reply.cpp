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

#include "wmts_reply.h"

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
QcWmtsReply::QcWmtsReply(QNetworkReply * reply, const QcTileSpec & tile_spec)
  : QcNetworkReply(reply),
    m_tile_spec(tile_spec)
{}

/*!
  Destroys this tiled map reply object.
*/
QcWmtsReply::~QcWmtsReply()
{}

/**************************************************************************************************/

// #include "wmts_reply.moc"

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
