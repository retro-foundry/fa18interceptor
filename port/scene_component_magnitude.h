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

#endif
