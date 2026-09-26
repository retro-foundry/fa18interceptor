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

typedef struct {
    uint8_t table_selection;
    uint16_t record_cursor;
    uint16_t record_limit;
    int16_t vertical_offset;
    uint8_t renderer_mode;
} FA18PostflightScene;

typedef struct {
    int16_t horizontal_offset;
    int16_t vertical_offset;
    int16_t horizontal_min;
    int16_t horizontal_max;
    int16_t vertical_base;
    uint8_t renderer_mode;
    uint8_t suppressed_planes;
} FA18PostflightComponent;

typedef struct {
    int16_t base_x;
    int16_t base_y;
    int16_t horizontal_offset;
    int16_t vertical_offset;
    uint8_t renderer_mode;
} FA18PostflightGroup;

/* Frame-395 entry contract recovered at $C31392: selector zero, ten-record
 * initial limit, and renderer mode zero. */
void fa18_postflight_scene_init(FA18PostflightScene *scene,
                                uint8_t table_selection,
                                uint16_t record_limit,
                                int16_t vertical_offset);

int fa18_postflight_component_coordinates(
    const FA18PostflightComponent *component, int16_t *x, int16_t *y);

int fa18_postflight_group_coordinates(const FA18PostflightGroup *group,
                                      int16_t *x, int16_t *y);

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
