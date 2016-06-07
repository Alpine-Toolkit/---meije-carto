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

#ifndef __MAP_EVENT_ROUTER_H__
#define __MAP_EVENT_ROUTER_H__

/**************************************************************************************************/

#include "qtcarto_global.h"

#include "coordinate/wgs84.h"

#include <QMap>
#include <QMetaType>
#include <QMouseEvent>
#include <QQmlEngine>
#include <QSharedPointer>
#include <QString>
#include <QWeakPointer>

/**************************************************************************************************/

class QDebug;

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/**************************************************************************************************/

// Fixme: QML here ???

class QcMapEvent
{
  Q_GADGET
  Q_PROPERTY(int buttons READ buttons CONSTANT)
  Q_PROPERTY(int button READ button CONSTANT)
  Q_PROPERTY(int modifiers READ modifiers CONSTANT)
  Q_PROPERTY(QcWgsCoordinate coordinate READ coordinate CONSTANT)
  Q_PROPERTY(QcVectorDouble projected_coordinate READ projected_coordinate CONSTANT)

public:
  QcMapEvent();
  QcMapEvent(int button, int buttons, int modifiers,
             const QcVectorDouble & projected_coordinate,
             const QcWgsCoordinate & coordinate);
  QcMapEvent(const QcMapEvent & other);
  ~QcMapEvent();

  QcMapEvent & operator=(const QcMapEvent & other);

  int button() const { return m_button; }
  int buttons() const { return m_buttons; }
  int modifiers() const { return m_modifiers; }
  const QcVectorDouble & projected_coordinate() const { return m_projected_coordinate; }
  const QcWgsCoordinate & coordinate() const { return m_coordinate; }

private:
  int m_button;
  int m_buttons;
  int m_modifiers;
  QcVectorDouble m_projected_coordinate;
  QcWgsCoordinate m_coordinate;
};

#ifndef QT_NO_DEBUG_STREAM
QC_EXPORT QDebug operator<<(QDebug, const QcMapEvent &);
#endif

Q_DECLARE_METATYPE(QcMapEvent)

/**************************************************************************************************/

class QcMapEventClient : public QObject
{
  Q_OBJECT

public:
  QcMapEventClient(const QString & name);
  ~QcMapEventClient();

  const QString & name() const { return m_name; }

  virtual void handle_mouse_press_event(const QcMapEvent & event) {Q_UNUSED(event)};
  virtual void handle_mouse_move_event(const QcMapEvent & event) {Q_UNUSED(event)};
  virtual void handle_mouse_release_event(const QcMapEvent & event) {Q_UNUSED(event)};
  virtual void handle_mouse_wheel_event(const QcMapEvent & event) {Q_UNUSED(event)};

  virtual void handle_mouse_press_and_hold_event(const QcMapEvent & event) {Q_UNUSED(event)};

  virtual void handle_clicked_event(const QcMapEvent & event) {Q_UNUSED(event)};
  virtual void handle_double_clicked_event(const QcMapEvent & event) {Q_UNUSED(event)};

private:
  QString m_name;
};

/**************************************************************************************************/

class QcMapEventRouter : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString current_client_name READ current_client_name CONSTANT)

public:
  typedef QSharedPointer<QcMapEventClient> ClientSharedPointer;
  typedef QWeakPointer<QcMapEventClient> ClientWeakPointer;

public:
  QcMapEventRouter();
  ~QcMapEventRouter();

  Q_INVOKABLE QcMapEvent create_map_event(const QMouseEvent * event,
                                          const QcVectorDouble & projected_coordinate,
                                          const QcWgsCoordinate & coordinate);

  Q_INVOKABLE void handle_mouse_press_event(const QcMapEvent & event);
  Q_INVOKABLE void handle_mouse_move_event(const QcMapEvent & event);
  Q_INVOKABLE void handle_mouse_release_event(const QcMapEvent & event);
  Q_INVOKABLE void handle_mouse_wheel_event(const QcMapEvent & event);

  Q_INVOKABLE void handle_mouse_press_and_hold_event(const QcMapEvent & event);

  Q_INVOKABLE void handle_clicked_event(const QcMapEvent & event);
  Q_INVOKABLE void handle_double_clicked_event(const QcMapEvent & event);

  void register_client(ClientSharedPointer client);
  void unregister_client(const QString & name);

  ClientSharedPointer current_client() { return m_current_client; }
  const QString & current_client_name() const { return m_current_client->name(); }

  // replace top
  // pop until
  // clear
  Q_INVOKABLE void push_client(const QString & name);
  Q_INVOKABLE void pop_client();

private:
  QMap<QString, ClientSharedPointer> m_clients;
  QList<ClientWeakPointer> m_client_stack;
  ClientSharedPointer m_current_client;
};

/**************************************************************************************************/

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __MAP_EVENT_ROUTER_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
