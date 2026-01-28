#pragma once

#include <iostream>
#include <random>
#include <thread>
#include <unistd.h>

#include "bench_thread_context.h"
#include "config.h"
#include "manager.h"

/// A conversion function from integer to integer.  It doesn't really do
/// anything, but we need it for symmetry with I2V.
struct I2I {
  static int convert(int i) { return i; }
};

/// Populate a map as if it were a set, with all of the even numbers in the
/// range specified by the configuration serving as the elements.
///
/// @param SET The type of the set to populate
/// @param K2V A converter from int keys to whatever value SET uses
///
/// @param set The set into which the inserts should happen
/// @param cfg The configuration object
template <class SET, class K2V> void fill_even(SET *set, config_t *cfg) {
  using namespace std;

  int start = 0;
  int end = start + cfg->key_range / 1;

  std::vector<int> v((end - start + 1) / 2 + 1);
  for (size_t i = 0; i < v.size(); i++)
    v[i] = i * 2 + start;
  // Prefill in decreasing order
  for (auto k : v) {
    auto val = K2V::convert(k);
    set->insert(k, val);
  }
}

/// Run integer set tests on map data structures as if they were sets.  This
/// requires set_t to have insert, lookup, and remove operations.
///
/// @param SET            The type of the set to populate
/// @param K2V            A converter from int keys to whatever value SET uses
///
/// @param set The set into which the inserts should happen
/// @param cfg The configuration object
template <class SET, typename K2V> void intmap_test(SET *set, config_t *cfg) {
  using namespace std;
  using event_types = bench_thread_context_t::EVENTS;

  // A manager for coordinating threads and collecting stats
  experiment_manager_t exp;

  // This is the benchmark task that each thread will perform
  auto task = [&](int id) {
    // Create thread benchmark and ds-specific contexts
    bench_thread_context_t self(id);

    // set up a PRNG for the thread
    using std::uniform_int_distribution;
    uniform_int_distribution<size_t> key_dist(0, cfg->key_range - 1);
    uniform_int_distribution<size_t> action_dist(0, 100);

    // A lambda that does one random operation
    auto tx = [&]() {
      // Generate a random key and action for the transaction
      int key;
      size_t action;
      key = key_dist(self.mt) % cfg->key_range;
      action = action_dist(self.mt);

      // Split non-lookups evenly between insert and remove
      size_t insert = (100 - cfg->lookup) / 2;

      // Each operation is protected by safe reclamation
      if (action <= cfg->lookup) {
        auto val = K2V::convert(key);
        if (set->get(key, val))
          ++self.stats[event_types::GET_T];
        else
          ++self.stats[event_types::GET_F];
      } else if (action < cfg->lookup + insert) {
        auto val = K2V::convert(key);
        if (set->insert(key, val))
          ++self.stats[event_types::INS_T];
        else
          ++self.stats[event_types::INS_F];
      } else {
        if (set->remove(key))
          ++self.stats[event_types::RMV_T];
        else
          ++self.stats[event_types::RMV_F];
      }
    };

    // Synchronize threads and get time
    exp.sync_before_launch(id, cfg);

    // Run the experiment
    if (cfg->timed_mode)
      while (exp.running.load())
        tx();
    else
      for (size_t i = 0; i < cfg->interval; ++i)
        tx();

    // arrive at the last barrier, then get the timer again
    exp.sync_after_launch(id, cfg);

    // merge stats into global
    for (size_t i = 0; i < event_types::NUM; ++i)
      exp.stats[i].fetch_add(self.stats[i]);
  };

  // Launch the threads... this thread won't run the tests
  vector<thread> threads;
  for (size_t i = 0; i < cfg->nthreads; i++)
    threads.emplace_back(task, i);
  for (size_t i = 0; i < cfg->nthreads; i++)
    threads[i].join();

  // Report statistics from the experiment
  exp.report(cfg);
}
