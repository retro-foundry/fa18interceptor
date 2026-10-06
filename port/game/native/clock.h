#ifndef FA18_NATIVE_CLOCK_H
#define FA18_NATIVE_CLOCK_H
/* timer.device GetSysTime result, sourced from the runner's PAL clock. */
void native_clock_set(unsigned pal_ticks);
void native_clock_request(void);
void native_clock_sample(void);
#endif
