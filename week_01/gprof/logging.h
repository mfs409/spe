#pragma once

#include <format>

#include "config.h"

/// Logging exists to provide verbose (per operation) logging
struct Logging {
  static bool enabled; // Is logging enabled?

  /// Configure (or reconfigure) the global logging mode
  ///
  /// @param cfg The command-line configuration
  static void configure(config_t *cfg) { enabled = cfg->logging; }

  /// Log a message only if logging is enabled
  ///
  /// @param msg
  void log(std::string msg) {
    if (enabled)
      printf("%s\n", msg.c_str());
  }
};

/// A global to manage logging
Logging logging;