#include "soc/bh_debug.h"

#include <stdexcept>
#include <cstdlib>
#include <string>

en_logv LogInfo::verbosity_ = en_logv::always;

void LogInfo::initialize() {

  verbosity_ = en_logv::always;

  const char* env_val = std::getenv("VERBOSE");
  if (!env_val)
    env_val = std::getenv("VERBOSITY");
  if (!env_val)
    return;

  std::string env_v(env_val);
  try {
    // TODO: rewrite int-based values to parsing of string constants
    int verbosity = std::stoi(env_v);
    if (verbosity < 0) {
      throw std::domain_error("verbosity value is out of the en_logv domain");
    }

    if (verbosity > (int)en_logv::noise)
      verbosity = (int)en_logv::noise;

    verbosity_ = (en_logv)verbosity;
    // TODO: add printing
  }
  catch(const std::exception& e) {
    fprintf(stderr, "FATAL ERROR: could not parse $env[VERBOSE]; "
            "value = %s, file: %s, line: %d\n",
            env_val, __FILE__, __LINE__);
    throw e;
  }
}

