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

#ifndef __OSM_H__
#define __OSM_H__

/**************************************************************************************************/

#include <QDateTime>
#include <QList>
#include <QString>
#include <QVariant>

#include "coordinate/wgs84.h"
#include "data_structure/pooled_string.h"

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

/*
 * http://wiki.openstreetmap.org/wiki/API_v0.6/DTD
 */

class QcOsmElment
{
public:
  QcOsmElment();
  QcOsmElment(int id, int version, int changeset, bool visible, int uid, QString user, QDateTime timestamp);
  QcOsmElment(const QcOsmElment & other);
  ~QcOsmElment();

  inline int id() const { return m_id; }
  inline void set_id(int id) { m_id = id; }

  inline int version() const { return m_version; }
  inline void set_version(int version) { m_version = version; }

  inline int changeset() const { return m_changeset; }
  inline void set_changeset(int changeset) { m_changeset = changeset; }

  inline bool visible() const { return m_visible; }
  inline void set_visible(bool visible) { m_visible = visible; }

  inline int uid() const { return m_uid; }
  inline void set_uid(int uid) { m_uid = uid; }

  inline const QString & user() const { return m_user; }
  inline void set_user(const QString & user) { m_user = user; }

  inline const QDateTime & timestamp() const { return m_timestamp; }
  inline void set_timestamp(const QDateTime & timestamp) { m_timestamp = timestamp; }

  const QMap<QcPooledString, QVariant> tags() const { return m_tags; }
  void add_tag(QString key, QVariant value) { m_tags.insert(key, value); }

private:
  int m_id;
  int m_version;
  int m_changeset;
  bool m_visible;
  int m_uid;
  QString m_user;
  QDateTime m_timestamp;

  QMap<QcPooledString, QVariant> m_tags;
};

/**************************************************************************************************/

class QcOsmNode : public QcOsmElment
{
public:
  QcOsmNode();
  QcOsmNode(int id, int version, int changeset, bool visible, int uid, QString user, QDateTime timestamp,
            const QcWgsCoordinate & coordinate);
  QcOsmNode(const QcOsmNode & other);
  ~QcOsmNode();

  inline const QcWgsCoordinate coordinate() const { return m_coordinate; }
  inline void set_coordinate(const QcWgsCoordinate & coordinate) { m_coordinate = coordinate; }

private:
  // double m_latitude;
  // double m_longitude;
  QcWgsCoordinate m_coordinate;
};

/**************************************************************************************************/

class QcOsmWay : public QcOsmElment
{
public:
  QcOsmWay();
  QcOsmWay(int id, int version, int changeset, bool visible, int uid, QString user, QDateTime timestamp,
           const QList<QcOsmNode *> & nodes);
  QcOsmWay(const QcOsmWay & other);
  ~QcOsmWay();

  const QList<QcOsmNode *> nodes() const { return m_nodes; }

private:
  // QList<int> m_nodes;
  QList<QcOsmNode *> m_nodes;
};

/**************************************************************************************************/

class QcOsmMember
{
public:
  enum class Type {Node, Way, Relation};

  QcOsmMember();
  QcOsmMember(Type type, int id, QString role);
  QcOsmMember(const QcOsmMember & other);
  ~QcOsmMember();

  Type type() const { return m_type; }
  void set_type(Type type) { m_type = type; }

  int id() const { return m_id; }
  void set_id(int id) { m_id = id; }

  const QString & role() const { return m_role; }
  void set_role(const QString & role) { m_role = role; }

private:
  Type m_type;
  int m_id;
  QString m_role;
};

/**************************************************************************************************/

class QcOsmRelation : public QcOsmElment
{
public:
  QcOsmRelation();
  QcOsmRelation(int id, int version, int changeset, bool visible, int uid, QString user, QDateTime timestamp,
                const QList<QcOsmMember> & members);
  QcOsmRelation(const QcOsmRelation & other);
  ~QcOsmRelation();

  const QList<QcOsmMember> members() const { return m_members; }

private:
  QList<QcOsmMember> m_members;
};

// QC_END_NAMESPACE

/**************************************************************************************************/

#endif /* __OSM_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
