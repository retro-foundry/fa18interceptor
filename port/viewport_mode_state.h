#ifndef FA18_VIEWPORT_MODE_STATE_H
#define FA18_VIEWPORT_MODE_STATE_H
#include <stdint.h>
typedef struct {
    uint8_t current, target, countdown, state;
} FA18ViewportModeState;
#endif
