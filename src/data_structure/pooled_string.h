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

#ifndef __POOLED_STRING_H__
#define __POOLED_STRING_H__

/**************************************************************************************************/

#include <QMap>
#include <QMutex>
#include <QString>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcPooledString
{
public:
  typedef unsigned int IdType;

  static QString UndefinedString;

public:
  static bool has_string(const QString & string); // only use for test !

public:
  QcPooledString();
  QcPooledString(const QString & string);
  QcPooledString(const QcPooledString & other);
  ~QcPooledString();

  QcPooledString & operator=(const QcPooledString & other);

  inline bool operator==(const QcPooledString & other) {
    return m_id == other.m_id;
  }
  inline bool operator!=(const QcPooledString & other) {
    return !operator==(other);
  }

  bool is_defined() const { return m_id != 0; };
  inline IdType id() const { return m_id; }
  inline const QString & string() const { return is_defined() ? m_id_map[m_id].string : UndefinedString; }
  inline const QString & operator*() const { return string(); }
  // inline const QString * string() const { return is_defined() ? &m_id_map[m_id].string : nullptr; }
  // inline const QString * operator*() const { return string(); }
  inline IdType reference_counter() const { return is_defined() ? m_id_map[m_id].reference_counter : 0; }

private:
  static inline IdType string_to_id(const QString & string) {
      return m_string_map.contains(string) ? m_string_map[string] : 0;
  }
  // static inline const QString * id_to_string(IdType id) {
  //     return m_id_map.contains(id) ? &m_id_map[id].string : nullptr;
  // }
  static inline void increment_ref_counter(IdType id) {
    m_id_map[id].reference_counter++;
  }
  static inline void decrement_ref_counter(IdType id) {
    m_id_map[id].reference_counter--;
  }
  static IdType add_string(const QString & string);

private:
  struct QcPooledStringData
  {
    QcPooledStringData()
      : string(), reference_counter(0)
    {}
    QcPooledStringData(QString string)
      : string(string), reference_counter(1)
    {}

    QString string;
    IdType reference_counter;
  };

  static QMutex m_mutex;
  static IdType m_last_id;
  static QMap<IdType, QcPooledStringData> m_id_map;
  static QMap<QString, IdType> m_string_map;

  IdType m_id;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __POOLED_STRING_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
