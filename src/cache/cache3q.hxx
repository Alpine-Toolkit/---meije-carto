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

// QC_BEGIN_NAMESPACE

template <class Key, class T>
void QcCache3QDefaultEvictionPolicy<Key, T>::about_to_be_evicted(const Key & key, QSharedPointer<T> obj)
{
  Q_UNUSED(key);
  Q_UNUSED(obj);
}

template <class Key, class T>
void QcCache3QDefaultEvictionPolicy<Key, T>::about_to_be_removed(const Key & key, QSharedPointer<T> obj)
{
  Q_UNUSED(key);
  Q_UNUSED(obj);
}

/**************************************************************************************************/

template <class Key, class T, class EvictionPolicy>
void
QcCache3Q<Key, T, EvictionPolicy>::print_stats()
{ // qDebug
  qInfo("\n=== cache %p ===", this);
  qInfo("hits: %d (%.2f%%)\tmisses: %d\tfill: %.2f%%", m_hit_count,
	 100.0 * float(m_hit_count) / (float(m_hit_count + m_miss_count)),
	 m_miss_count,
	 100.0 * float(total_cost()) / float(max_cost()));
  qInfo("q1g: size=%d, pop=%llu", m_q1_evicted->size, m_q1_evicted->pop);
  qInfo("q1:  cost=%d, size=%d, pop=%llu", m_q1->cost, m_q1->size, m_q1->pop);
  qInfo("q2:  cost=%d, size=%d, pop=%llu", m_q2->cost, m_q2->size, m_q2->pop);
  qInfo("q3:  cost=%d, size=%d, pop=%llu", m_q3->cost, m_q3->size, m_q3->pop);
}

template <class Key, class T, class EvictionPolicy>
QcCache3Q<Key,T,EvictionPolicy>::QcCache3Q(int max_cost, int min_recent, int max_old_popular)
  : m_q1(new Queue), m_q2(new Queue), m_q3(new Queue), m_q1_evicted(new Queue),
    m_max_cost(max_cost), m_min_recent(min_recent), m_max_old_popular(max_old_popular),
    m_hit_count(0), m_miss_count(0), m_promote(0)
{
  if (m_min_recent < 0)
    m_min_recent = max_cost / 3;
  if (m_max_old_popular < 0)
    m_max_old_popular = max_cost / 5;
}

template <class Key, class T, class EvictionPolicy>
void
QcCache3Q<Key, T, EvictionPolicy>::serialize_queue(int queue_number, QList<QSharedPointer<T> > & buffer)
{
  Q_ASSERT(queue_number >= 1 && queue_number <= 4);

  Queue *queue = queue_number == 1 ? m_q1 :
    queue_number == 2 ? m_q2 :
    queue_number == 3 ? m_q3 :
    m_q1_evicted;

  for (Node * node = queue->first; node; node = node->next)
    buffer.append(node->value);
}

template <class Key, class T, class EvictionPolicy>
void
QcCache3Q<Key, T, EvictionPolicy>::deserialize_queue(int queue_number, const QList<Key> & keys,
						     const QList<QSharedPointer<T> > & values, const QList<int> & costs)
{
  Q_ASSERT(queue_number >= 1 && queue_number <= 4);

  int buffer_size = keys.size();
  if (buffer_size == 0)
    return;

  clear();

  Queue *queue = queue_number == 1 ? m_q1 :
    queue_number == 2 ? m_q2 :
    queue_number == 3 ? m_q3 :
    m_q1_evicted;

  for (int i = 0; i < buffer_size; ++i) {
    Node * node = new Node;
    node->value = values[i];
    node->key = keys[i];
    node->cost = costs[i];
    link_front(node, queue);
    m_lookup[keys[i]] = node;
  }
}

template <class Key, class T, class EvictionPolicy>
inline void
QcCache3Q<Key, T, EvictionPolicy>::set_max_cost(int max_cost, int min_recent, int max_old_popular)
{
  m_max_cost = max_cost;
  m_min_recent = min_recent;
  m_max_old_popular = max_old_popular;
  if (m_min_recent < 0)
    m_min_recent = max_cost / 3;
  if (m_max_old_popular < 0)
    m_max_old_popular = max_cost / 5;
  rebalance();
}

template <class Key, class T, class EvictionPolicy>
bool
QcCache3Q<Key, T, EvictionPolicy>::insert(const Key & key, QSharedPointer<T> object, int cost)
{
  if (cost > m_max_cost) {
    return false;
  }

  if (m_lookup.contains(key)) {
    Node * node = m_lookup[key];
    node->value = object;
    node->queue->cost -= node->cost;
    node->cost = cost;
    node->queue->cost += cost;

    if (node->queue == m_q1_evicted) {
      if (node->pop > (uint)m_promote) { // uint(...)
	unlink(node);
	link_front(node, m_q2);
	rebalance();
      }
    } else if (node->queue != m_q1) {
      Queue * queue = node->queue;
      unlink(node);
      link_front(node, queue);
      rebalance();
    }

    return true;
  }

  Node * node = new Node;
  node->value = object;
  node->key = key;
  node->cost = cost;
  link_front(node, m_q1);
  m_lookup[key] = node;

  rebalance();

  return true;
}

template <class Key, class T, class EvictionPolicy>
void
QcCache3Q<Key, T, EvictionPolicy>::clear()
{
  while (m_q1_evicted->first) {
    Node * node = m_q1_evicted->first;
    unlink(node);
    delete node;
  }

  while (m_q1->first) {
    Node * node = m_q1->first;
    unlink(node);
    EvictionPolicy::about_to_be_removed(node->key, node->value);
    delete node;
  }

  while (m_q2->first) {
    Node * node = m_q2->first;
    unlink(node);
    EvictionPolicy::about_to_be_removed(node->key, node->value);
    delete node;
  }

  while (m_q3->first) {
    Node * node = m_q3->first;
    unlink(node);
    EvictionPolicy::about_to_be_removed(node->key, node->value);
    delete node;
  }

  m_lookup.clear();
}

template <class Key, class T, class EvictionPolicy>
void
QcCache3Q<Key, T, EvictionPolicy>::unlink(Node * node)
{
  if (node->next)
    node->next->previous = node->previous;
  if (node->previous)
    node->previous->next = node->next;
  if (node->queue->first == node)
    node->queue->first = node->next;
  if (node->queue->last == node)
    node->queue->last = node->previous;
  node->next = 0;
  node->previous = 0;
  node->queue->pop -= node->pop;
  node->queue->cost -= node->cost;
  node->queue->size--;
  node->queue = 0;
}

template <class Key, class T, class EvictionPolicy>
void QcCache3Q<Key,T,EvictionPolicy>::link_front(Node * node, Queue * queue)
{
  node->next = queue->first;
  node->previous = 0;
  node->queue = queue;
  if (queue->first)
    queue->first->previous = node;
  queue->first = node;
  if (!queue->last)
    queue->last = node;

  queue->pop += node->pop;
  queue->cost += node->cost;
  queue->size++;
}

template <class Key, class T, class EvictionPolicy>
void
QcCache3Q<Key, T, EvictionPolicy>::rebalance()
{
  while (m_q1_evicted->size > (m_q1->size + m_q2->size + m_q3->size) * 4) {
    Node * node = m_q1_evicted->last;
    unlink(node);
    m_lookup.remove(node->key);
    delete node;
  }

  while ((m_q1->cost + m_q2->cost + m_q3->cost) > m_max_cost) {
    if (m_q3->cost > m_max_old_popular) {
      Node * node = m_q3->last;
      unlink(node);
      EvictionPolicy::about_to_be_evicted(node->key, node->value);
      m_lookup.remove(node->key);
      delete node;
    } else if (m_q1->cost > m_min_recent) {
      Node * node = m_q1->last;
      unlink(node);
      EvictionPolicy::about_to_be_evicted(node->key, node->value);
      node->value.clear();
      node->cost = 0;
      link_front(node, m_q1_evicted);
    } else {
      Node * node = m_q2->last;
      unlink(node);
      if (node->pop > (m_q2->pop / m_q2->size)) {
	link_front(node, m_q3);
      } else {
	EvictionPolicy::about_to_be_evicted(node->key, node->value);
	node->value.clear();
	node->cost = 0;
	link_front(node, m_q1_evicted);
      }
    }
  }
}

template <class Key, class T, class EvictionPolicy>
void
QcCache3Q<Key, T, EvictionPolicy>::remove(const Key & key)
{
  if (!m_lookup.contains(key)) {
    return;
  }
  Node * node = m_lookup[key];
  unlink(node);
  if (node->queue != m_q1_evicted)
    EvictionPolicy::about_to_be_removed(node->key, node->value);
  m_lookup.remove(key);
  delete node;
}

template <class Key, class T, class EvictionPolicy>
QSharedPointer<T> QcCache3Q<Key, T, EvictionPolicy>::object(const Key & key) const
{
  if (!m_lookup.contains(key)) {
    const_cast<QcCache3Q<Key, T, EvictionPolicy> *>(this)->m_miss_count++;
    return QSharedPointer<T>(nullptr);
  }

  QcCache3Q<Key, T, EvictionPolicy> *me = const_cast<QcCache3Q<Key,T,EvictionPolicy> *>(this);

  Node * node = me->m_lookup[key];
  node->pop++;
  node->queue->pop++;

  if (node->queue == m_q1) {
    me->m_hit_count++;

    if (node->pop > (quint64)m_promote) {
      me->unlink(node);
      me->link_front(node, m_q2);
      me->rebalance();
    }
  } else if (node->queue != m_q1_evicted) {
    me->m_hit_count++;

    Queue * queue = node->queue;
    me->unlink(node);
    me->link_front(node, queue);
    me->rebalance();
  } else {
    me->m_miss_count++;
  }

  return node->value;
}

template <class Key, class T, class EvictionPolicy>
inline
QSharedPointer<T> QcCache3Q<Key, T, EvictionPolicy>::operator[](const Key & key) const
{
  return object(key);
}

// QC_END_NAMESPACE

/***************************************************************************************************
 *
 * End
 *
 **************************************************************************************************/
