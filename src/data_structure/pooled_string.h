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
  typedef unsigned int IdType; // Fixme: useful ?

  static QString UndefinedString;

private:
  struct QcPooledStringData
  {
    QcPooledStringData()
      : id(0), string(), reference_counter(0)
    {}
    QcPooledStringData(IdType id, QString string)
      : id(id), string(string), reference_counter(1)
    {}

    IdType id;
    QString string;
    uint reference_counter;
  };

public:
  static bool has_string(const QString & string); // only use for test !

public:
  QcPooledString();
  QcPooledString(const QString & string);
  // QcPooledString(const char * string);
  QcPooledString(const QcPooledString & other);
  ~QcPooledString();

  QcPooledString & operator=(const QcPooledString & other);

  inline bool operator==(const QcPooledString & other) {
    return m_data == other.m_data;
  }
  inline bool operator!=(const QcPooledString & other) {
    return !operator==(other);
  }

  bool is_defined() const { return m_data != nullptr; };
  inline IdType id() const { return is_defined() ? m_data->id : 0; }
  inline const QString & string() const { return is_defined() ? m_data->string : UndefinedString; }
  inline const QString & operator*() const { return string(); }
  inline uint reference_counter() const { return is_defined() ? m_data->reference_counter : 0; }

private:
  static inline IdType string_to_id(const QString & string) {
      return m_string_map.contains(string) ? m_string_map[string] : 0;
  }
  // static inline const QString * id_to_string(IdType id) {
  //     return m_id_map.contains(id) ? &m_id_map[id].string : nullptr;
  // }
  static inline void increment_ref_counter(QcPooledStringData * data) {
    data->reference_counter++;
  }
  static inline void decrement_ref_counter(QcPooledStringData * data) {
    data->reference_counter--;
  }
  static QcPooledStringData * add_string(const QString & string);

private:
  static QMutex m_mutex;
  static IdType m_last_id;
  static QMap<IdType, QcPooledStringData> m_id_map;
  static QMap<QString, IdType> m_string_map;

  QcPooledStringData * m_data;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __POOLED_STRING_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
