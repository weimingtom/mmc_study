#ifndef CONFIG_H
#define CONFIG_H

#ifdef _SYS_FEATURE_TESTS_H
#error "You should include config.h as your first include file"
#endif

#define HAVE_GETHRTIME 1

#ifndef HAVE_GETHRTIME
#include <stdint.h>
typedef uint64_t hrtime_t;
extern hrtime_t gethrtime(void);
#endif

#endif
