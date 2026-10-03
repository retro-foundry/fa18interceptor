#ifndef FA18_GLUE_POSTFLIGHT_SCHEDULER_H
#define FA18_GLUE_POSTFLIGHT_SCHEDULER_H
#include <stdint.h>
#define POSTFLIGHT_SCHEDULER_ENTRIES(X) \
    X(C09E06) X(C09E98) X(C09EC4) X(C0A002) X(C0A12E) X(C0A15C) \
    X(C0A1E0) X(C0A2F0) X(C0A334) X(C0A364) X(C0A3EA)
#define DECLARE_SCHEDULE(entry) int glue_##entry(void); int glue_##entry##_step(void); int glue_##entry##_owns(uint32_t pc);
POSTFLIGHT_SCHEDULER_ENTRIES(DECLARE_SCHEDULE)
#undef DECLARE_SCHEDULE
#endif
