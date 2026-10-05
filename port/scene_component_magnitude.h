#ifndef FA18_SCENE_COMPONENT_MAGNITUDE_H
#define FA18_SCENE_COMPONENT_MAGNITUDE_H

#include <stddef.h>
#include <stdint.h>

#include "hunk.h"
#include "field_window.h"

enum {
    FA18_SCENE_MAGNITUDE_HUNK = 8,
    FA18_SCENE_MAGNITUDE_OFFSET = 0x1710,
    FA18_SCENE_MAGNITUDE_WORDS = 258
};

/* `$C1D9D8` is a word-addressed fixed-point ratio table. */
typedef struct {
    const uint16_t *words;
    size_t count;
} FA18SceneMagnitudeTable;

typedef struct {
    uint16_t words[FA18_SCENE_MAGNITUDE_WORDS];
} FA18LoadedSceneMagnitudeTable;

int fa18_load_scene_magnitude_table(const FA18Hunks *hunks,
                                    FA18LoadedSceneMagnitudeTable *table);
/* Original Hunk-8 bytes, centred at the ratio table. Negative/overflowed
 * indices remain inside the actual hunk or require explicit adjacent owners. */
int fa18_load_scene_magnitude_window(const FA18Hunks *hunks,PortFieldWindow *window);

/* `$C1D974-$C1D9D6`: table-assisted bound of three component words.  A
 * return of -1 represents a source DIVU exception or an unrepresentable
 * caller-owned table lookup. */
int fa18_scene_component_magnitude(const FA18SceneMagnitudeTable *table,
                                   int16_t component_0,
                                   int16_t component_1,
                                   int16_t component_2,
                                   int16_t *result);

/* Same complete primitive with signed byte offsets into original immutable
 * data and explicit adjacent fields. Unlike the bounded word-table API, this
 * also supports the source's negative/overflowed component lookup paths. */
int fa18_scene_component_magnitude_window(const PortFieldWindow *table,
                                          int16_t x,int16_t y,int16_t z,int16_t *result);
/* Same primitive, also returning the numeric final vertical-ratio operand
 * consumed by callers that retain it across subsequent operations. */
int fa18_scene_component_magnitude_window_with_axis(const PortFieldWindow *table,
    int16_t x,int16_t y,int16_t z,int16_t *result,uint32_t *axis);
/* Full numeric result used by C2574A: a clamp replaces the low word while
 * retaining the original high bits. No instruction/CPU state is exposed. */
int fa18_scene_component_magnitude_window_value(const PortFieldWindow *table,
    int16_t x,int16_t y,int16_t z,uint32_t *result,uint32_t *axis);

#endif
