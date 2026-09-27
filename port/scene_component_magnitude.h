#ifndef FA18_SCENE_COMPONENT_MAGNITUDE_H
#define FA18_SCENE_COMPONENT_MAGNITUDE_H

#include <stddef.h>
#include <stdint.h>

/* `$C1D9D8` is a word-addressed fixed-point ratio table. */
typedef struct {
    const uint16_t *words;
    size_t count;
} FA18SceneMagnitudeTable;

/* `$C1D974-$C1D9D6`: table-assisted bound of three component words.  A
 * return of -1 represents a source DIVU exception or an unrepresentable
 * caller-owned table lookup. */
int fa18_scene_component_magnitude(const FA18SceneMagnitudeTable *table,
                                   int16_t component_0,
                                   int16_t component_1,
                                   int16_t component_2,
                                   int16_t *result);

#endif
