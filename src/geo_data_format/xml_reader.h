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

#ifndef __QcXmlStreamReader_H__
#define __QcXmlStreamReader_H__

/**************************************************************************************************/

#include <QDateTime>
#include <QIODevice>
#include <QString>
#include <QXmlStreamReader>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

class QcXmlStreamReader : public QXmlStreamReader
{
public:
  bool read_match_start_element(const QString & name);
  bool read_match_end_element(const QString & name);
  bool match_start_element(const QString & name);
  bool match_end_element(const QString & name);
  bool read_match_empty();
  bool match_empty();

  void read_until_start_of(const QString & name);
  void read_until_end_of(const QString & name);
  void read_until_empty();

  QDateTime read_date();
  int read_int();
  double read_double();

  QString get_attribute(QString attribute);
  int get_int_attribute(QString attribute);
  double get_double_attribute(QString attribute);
};

// QC_END_NAMESPACE

#endif /* __QcXmlStreamReader_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
