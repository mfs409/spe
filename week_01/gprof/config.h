#pragma once

#include <iostream>
#include <libgen.h>
#include <unistd.h>

/// config_t encapsulates all of the configuration options.  It standardizes the
/// format of command-line arguments, parsing of command-line arguments, and
/// reporting of command-line arguments.
///
/// The purpose of config_t is just to reduce boilerplate code.  We aren't
/// concerned about good object-oriented design, so everything is public.
struct config_t {
  size_t interval = 1;    // # seconds to run for, or # operations per thread
  bool timed_mode = true; // is `interval` a time (true) or # transactions
  size_t key_range = 256; // The range for keys in maps or for elements in sets
  size_t nthreads = 1;    // Number of threads that should run the benchmark
  size_t lookup = 34;     // % lookups.  inserts/removes evenly split the rest
  bool verbose = false;   // Print verbose output?
  std::string exe_name;   // The name of the executable
  bool logging = false;   // Use extra logging?

  /// Initialize the program's configuration by setting the strings that are not
  /// dependent on the command-line
  config_t(int argc, char **argv) : exe_name(basename(argv[0])) {
    long opt;
    while ((opt = getopt(argc, argv, "hi:k:r:t:xlv")) != -1) {
      switch (opt) {
      case 'i':
        interval = atoi(optarg);
        break;
      case 'k':
        key_range = atoi(optarg);
        break;
      case 'h':
        usage();
        std::exit(0);
        break;
      case 'r':
        lookup = atoi(optarg);
        break;
      case 't':
        nthreads = atoi(optarg);
        break;
      case 'x':
        timed_mode = !timed_mode;
        break;
      case 'l':
        logging = !logging;
        break;
      case 'v':
        verbose = !verbose;
        break;
      default:
        throw "Invalid configuration flag " + std::to_string(opt);
      }
    }
  }

  /// Usage() reports on the command-line options for the benchmark
  void usage() {
    std::cout
        << exe_name << "\n"
        << "  -i: secs to run, or # ops/thread (default 5)\n"
        << "  -h: print this message           (default false)\n"
        << "  -k: key range                    (default 256)\n"
        << "  -r: lookup ratio                 (default 34%)\n"
        << "  -t: # threads                    (default 1)\n"
        << "  -x: toggle 'i' parameter         (default true <timed mode>)\n"
        << "  -v: toggle verbose mode          (default false)\n"
        << "  -l: toggle logging mode          (default false)\n";
  }

  /// Report the current values of the configuration object as a CSV line
  void report() {
    std::cout << exe_name << ", (ikrtx), " << interval << ", " << key_range
              << ", " << lookup << ", " << nthreads << ", " << timed_mode
              << ", ";
  }
};
