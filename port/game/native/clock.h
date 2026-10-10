#ifndef FA18_NATIVE_CLOCK_H
#define FA18_NATIVE_CLOCK_H
#include <stdint.h>
/* C16D04's timer.device GetSysTime boundary. Interactive hosts provide their
 * actual microsecond clock; deterministic diagnostics can retain PAL time. */
typedef uint64_t (*NativeClockRead)(void *context);
typedef struct {
    unsigned requests;
    uint32_t low_bits_seen;
    uint64_t last_microseconds;
} NativeClockStats;
void native_clock_set_source(NativeClockRead read,void *context);
NativeClockStats native_clock_stats(void);
void native_clock_set(unsigned pal_ticks);
void native_clock_request(void);
void native_clock_sample(void);
#endif
