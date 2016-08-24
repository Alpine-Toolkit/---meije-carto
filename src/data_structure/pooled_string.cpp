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

#include "pooled_string.h"

#include <QtDebug>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

QMutex QcPooledString::m_mutex;
QcPooledString::IdType QcPooledString::m_last_id = 1;
QMap<QcPooledString::IdType, QcPooledString::QcPooledStringData> QcPooledString::m_id_map;
QMap<QString, QcPooledString::IdType> QcPooledString::m_string_map;

QString QcPooledString::UndefinedString;

bool
QcPooledString::has_string(const QString & string)
{
  m_mutex.lock();
  bool rc = m_string_map.contains(string);
  m_mutex.unlock();
  return rc;
}

QcPooledString::QcPooledStringData *
QcPooledString::add_string(const QString & string)
{
  IdType id = m_last_id++;
  QcPooledStringData data(id, string);
  m_id_map.insert(id, data);
  m_string_map.insert(string, id);
  qDebug() << "Added" << string << m_id_map.keys() << m_string_map.keys();
  return &m_id_map[id];
}

QcPooledString::QcPooledString()
  : m_data(nullptr)
{}

QcPooledString::QcPooledString(const QString & string)
{
  m_mutex.lock();
  IdType id = string_to_id(string);
  if (id) {
    m_data = &m_id_map[id];
    if (is_defined())
      increment_ref_counter(m_data);
  } else
    m_data = add_string(string);
  m_mutex.unlock();
}

QcPooledString::QcPooledString(const QcPooledString & other)
{
  m_mutex.lock();
  m_data = other.m_data;
  if (is_defined())
    increment_ref_counter(m_data);
  m_mutex.unlock();
}

QcPooledString::~QcPooledString()
{
  qDebug() << "Delete" << id();
  if (is_defined()) {
    m_mutex.lock();
    uint counter = reference_counter();
    if (counter > 1)
      decrement_ref_counter(m_data);
    else {
      QString s = string(); // mandatory in order to remove the string
      m_id_map.remove(id());
      m_string_map.remove(s);
      qDebug() << "Remove string" << s << m_id_map.keys() << m_string_map.keys();
    }
    m_mutex.unlock();
  }
}

QcPooledString &
QcPooledString::operator=(const QcPooledString & other)
{
  if (this != &other) {
    m_mutex.lock();
    m_data = other.m_data;
    if (is_defined())
      increment_ref_counter(m_data);
    m_mutex.unlock();
  }

  return *this;
}

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
