#ifndef DEBUG_H_8IT79UPY
#define DEBUG_H_8IT79UPY

#include <stdio.h>

enum class en_logv: int {
  always = 0,
  error,
  debug,
  noise
};

const char* en2s(en_logv);
class LogInfo {
public:
  static bool is_verbose(en_logv level) {
    return (int)level <= (int)verbosity_;
  }
  static void initialize();
private:
  static en_logv verbosity_;
};

#define   likely(x) __builtin_expect(x, 1)
#define unlikely(x) __builtin_expect(x, 0)

#define LOG_MSG(level, ...)                     \
  do {                                          \
    if (unlikely(LogInfo::is_verbose(level))) { \
      fprintf(stderr, "%s", en2s(level));       \
      fprintf(stderr, __VA_ARGS__);             \
    }                                           \
  } while(0)



#endif /* end of include guard: DEBUG_H_8IT79UPY */
