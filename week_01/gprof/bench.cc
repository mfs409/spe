#include "dlist_omap.h"
#include "experiment.h"
#include "logging.h"

using map = dlist_omap<int, int>;
using K2VAL = I2I;

/// A standardized main() function for use with all of our integer map
/// benchmarks
int main(int argc, char **argv) {
  // Parse and print the command-line options.  If it throws, terminate
  config_t *cfg = new config_t(argc, argv);
  cfg->report();

  // Enable logging
  logging.configure(cfg);

  // Create a bst and fill it
  auto ds = new map(cfg);
  fill_even<map, K2VAL>(ds, cfg);

  // Launch the test
  intmap_test<map, K2VAL>(ds, cfg);
}

/// Provide backing for the static field of the Logging object
bool Logging::enabled;