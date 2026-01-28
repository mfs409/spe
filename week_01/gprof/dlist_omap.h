#pragma once

#include <mutex>

#include "logging.h"

/// An ordered map, implemented as a doubly-linked list.  This map supports
/// get(), insert(), and remove() operations.
///
/// @tparam K The type of the keys stored in this map
/// @tparam V The type of the values stored in this map
template <typename K, typename V> class dlist_omap {

  /// A list node.  It has prev and next pointers, but no key or value.  It's
  /// useful for sentinels, so that K and V don't have to be default
  /// constructable.
  struct node_t {
    node_t *prev; // Pointer to predecessor
    node_t *next; // Pointer to successor

    /// Construct a node
    node_t() : prev(nullptr), next(nullptr) {}

    /// Destructor is a no-op, but it needs to be virtual because of inheritance
    virtual ~node_t() {}
  };

  /// A list node that also has a key and value.  Note that keys are const.
  struct data_t : public node_t {
    const K key; // The key of this key/value pair
    V val;       // The value of this key/value pair

    /// Construct a data_t
    ///
    /// @param _key The key that is stored in this node
    /// @param _val The value that is stored in this node
    data_t(const K &_key, const V &_val) : node_t(), key(_key), val(_val) {}

    /// Destructor is a no-op, but it needs to be virtual because of inheritance
    virtual ~data_t() {}
  };

  node_t *const head; // The list head pointer
  node_t *const tail; // The list tail pointer

  std::mutex lock; // A lock, for managing concurrency

public:
  /// Default construct a list by constructing and connecting two sentinel nodes
  ///
  /// @param cfg A configuration object
  dlist_omap(auto *cfg) : head(new node_t()), tail(new node_t()) {
    head->next = tail;
    tail->prev = head;
  }

private:
  /// get_leq is an inclusive predecessor query that returns the largest node
  /// whose key is <= the provided key.  It can return the head sentinel, but
  /// not the tail sentinel.
  ///
  /// @param key The key for which we are doing a predecessor query.
  ///
  /// @return The node that was found
  node_t *get_leq(const K key) {
    node_t *curr = head;
    auto *next = curr->next;
    while (true) {
      if (next == tail)
        return curr;
      auto next_next = next->next;
      auto nkey = static_cast<data_t *>(next)->key;
      if (nkey > key)
        return curr;
      if (nkey == key)
        return next;
      curr = next;
      next = next_next;
    }
  }

public:
  /// Search the data structure for a node with key `key`.  If not found, return
  /// false.  If found, return true, and set `val` to the value associated with
  /// `key`.
  ///
  /// @param key The key to search
  /// @param val A ref parameter for returning key's value, if found
  ///
  /// @return True if the key is found, false otherwise.  The reference
  ///         parameter `val` is only valid when the return value is true.
  bool get(const K &key, V &val) {
    std::lock_guard<std::mutex> l(lock);
    logging.log(std::format("get {}", key));
    auto n = get_leq(key);
    if (n == head || static_cast<data_t *>(n)->key != key)
      return false;
    val = static_cast<data_t *>(n)->val;
    return true;
  }

  /// Create a mapping from the provided `key` to the provided `val`, but only
  /// if no such mapping already exists.  This method does *not* have upsert
  /// behavior for keys already present.
  ///
  /// @param key The key for the mapping to create
  /// @param val The value for the mapping to create
  ///
  /// @return True if the value was inserted, false otherwise.
  bool insert(const K &key, V &val) {
    std::lock_guard<std::mutex> l(lock);
    logging.log(std::format("insert {}", key));
    auto n = get_leq(key);
    if (n != head && static_cast<data_t *>(n)->key == key)
      return false;
    auto next = n->next;
    data_t *new_dn = new data_t(key, val);
    new_dn->next = next;
    new_dn->prev = n;
    n->next = new_dn;
    next->prev = new_dn;
    return true;
  }

  /// Clear the mapping involving the provided `key`.
  ///
  /// @param me  Unused thread context
  /// @param key The key for the mapping to eliminate
  ///
  /// @return True if the key was found and removed, false otherwise
  bool remove(const K &key) {
    std::lock_guard<std::mutex> l(lock);
    logging.log(std::format("remove {}", key));
    auto n = get_leq(key);
    if (n == head || static_cast<data_t *>(n)->key != key)
      return false;
    auto pred = n->prev, succ = n->next;
    pred->next = succ;
    succ->prev = pred;
    delete n;
    return true;
  }
};
