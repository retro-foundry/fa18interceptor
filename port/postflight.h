#ifndef FA18_POSTFLIGHT_H
#define FA18_POSTFLIGHT_H

#include <stdint.h>

typedef enum {
    FA18_POSTFLIGHT_SHARED,
    FA18_POSTFLIGHT_ADJACENT
} FA18PostflightRenderer;

typedef struct {
    int16_t x;
    int16_t y;
    uint8_t flags;
} FA18PostflightRecord;

typedef struct {
    int16_t vertical_offset;
    uint16_t table_limit;
    uint16_t submitted;
    uint8_t rejected;
} FA18PostflightState;

typedef int (*FA18PostflightSubmit)(FA18PostflightRenderer renderer,
                                    int16_t x, int16_t y, void *context);

/* Semantic translation of $C316C0-$C31721. Returns 0 when the record was
 * submitted, 1 when the selected table is full, or -1 on invalid input or
 * callback failure. The original table addresses are represented by cursor
 * and limit counts, not native pointers. */
int fa18_postflight_submit(FA18PostflightState *state,
                           FA18PostflightRecord record,
                           FA18PostflightSubmit submit, void *context);

#endif
