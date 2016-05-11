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
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef __QCACHE3Q_H__
#define __QCACHE3Q_H__

/**************************************************************************************************/

#include <QHash>
#include <QCache>
#include <QSharedPointer>
#include <QDebug>

/**************************************************************************************************/

// QC_BEGIN_NAMESPACE

template <class Key, class T>
class QcCache3QDefaultEvictionPolicy
{
protected:
  // called just before a key/value pair is about to be _evicted_
  void about_to_be_evicted(const Key & key, QSharedPointer<T> obj);

  // called just before a key/value pair is about to be removed, by
  // clear(), remove() or by the destructor (which calls clear)
  void about_to_be_removed(const Key & key, QSharedPointer<T> obj);
};

/*
 * QCache3Q
 *
 * A cache template class for managing QSharedPointers to objects with an
 * associated key. It's a lot like QCache, but uses an alternative algorithm
 * '3Q' -- which is the '2Q' algorithm plus an extra queue for previously popular
 * but evicted nodes, and a 'ghost' list of recent evictions to make a better
 * placement choice if they are requested again.
 *
 * New nodes enter the cache on the "newbies" queue, which is evicted LRA
 * (least-recently-added). If a newbie is popular enough (it has been requested
 * more than promoteAt times), it will be promoted to a "regular". Regulars
 * are evicted LRU (least-recently-used). If a regular is under consideration
 * for eviction, its popularity is compared to the mean popularity of the whole
 * regulars queue. If it is greater, it is instead moved to the "hobos" queue.
 * The "hobos" queue is also evicted LRU, but has a maximum size constraint
 * so eviction from it is less likely than from the regulars.
 *
 * Tweakables:
 *  * max_cost = maximum total cost for the whole cache
 *  * min_recent = minimum size that q1 ("newbies") has to be before eviction
 *                 from it takes place
 *  * max_old_popular = maximum size that q3 ("hobos") can reach before eviction
 *                      from it takes place
 *  * promote_at = minimum popularity necessary to promote a node from
 *                 "newbie" to "regular"
 */
template <class Key, class T, class EvictionPolicy = QcCache3QDefaultEvictionPolicy<Key, T> >
class QcCache3Q : public EvictionPolicy
{
private:
  class Queue;

  // Fixme: inlined data structure
  class Node
  {
  public:
    inline explicit Node() : queue(nullptr), next(nullptr), previous(nullptr), pop(0), cost(0) {}

    Queue * queue;
    Node * next;
    Node * previous;
    Key key;
    QSharedPointer<T> value;
    quint64 pop; // popularity, incremented each ping
    int cost;
  };

  class Queue
  {
  public:
    inline explicit Queue() : first(nullptr), last(nullptr), cost(0), pop(0), size(0) {}

    Node *first;
    Node *last;
    int cost; // total cost of nodes on the queue
    quint64 pop; // sum of popularity values on the queue
    int size; // size of the queue
  };

  Queue * m_q1; // "newbies": seen only once, evicted LRA (least-recently-added)
  Queue * m_q2; // regular nodes, promoted from newbies, evicted LRU
  Queue * m_q3; // "hobos": evicted from q2 but were very popular (above mean)
  Queue * m_q1_evicted; // ghosts of recently evicted newbies and regulars
  QHash<Key, Node *> m_lookup;

public:
  explicit QcCache3Q(int max_cost = 100, int min_recent = -1, int max_old_popular = -1);
  inline ~QcCache3Q() { clear(); delete m_q1; delete m_q2; delete m_q3; delete m_q1_evicted; }

  inline int max_cost() const { return m_max_cost; }
  void set_max_cost(int max_cost, int min_recent = -1, int max_old_popular = -1);

  inline int promote_at() const { return m_promote; }
  inline void set_promote_at(int p) { m_promote = p; }

  inline int total_cost() const { return m_q1->cost + m_q2->cost + m_q3->cost; }

  void clear();
  bool insert(const Key & key, QSharedPointer<T> object, int cost = 1);
  QSharedPointer<T> object(const Key & key) const;
  QSharedPointer<T> operator[](const Key & key) const;

  void remove(const Key & key);

  void print_stats();

  // Copy data directly into a queue. Designed for single use after construction
  void deserialize_queue(int queue_number, const QList<Key> & keys,
			 const QList<QSharedPointer<T> > & values, const QList<int> & costs);
  // Copy data from specific queue into list
  void serialize_queue(int queue_number, QList<QSharedPointer<T> > & buffer);

private:
  void rebalance();
  void unlink(Node * node);
  void link_front(Node * node, Queue * queue);

private:
  // make these private so they can't be used
  inline QcCache3Q(const QcCache3Q<Key, T, EvictionPolicy> &) {}
  inline QcCache3Q<Key, T, EvictionPolicy> & operator=(const QcCache3Q<Key, T, EvictionPolicy> &) {}

private:
  int m_max_cost, m_min_recent, m_max_old_popular;
  int m_hit_count, m_miss_count, m_promote;
};

/**************************************************************************************************/

#ifndef QC_MANUAL_INSTANTIATION
#include "cache3q.hxx"
#endif

/**************************************************************************************************/

// QC_END_NAMESPACE

#endif /* __QCACHE3Q_H__ */

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
