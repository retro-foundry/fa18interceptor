#ifndef FA18_SCENE_ALTERNATE_RECORD_H
#define FA18_SCENE_ALTERNATE_RECORD_H

#include <stdint.h>

#include "scene_placement.h"

typedef struct {
    int16_t first_word;
    uint32_t handler;
} FA18SceneAlternateHandler;

typedef int (*FA18SceneAlternateHandlerLookup)(void *context,
                                               uint32_t reference,
                                               FA18SceneAlternateHandler *handler);

typedef enum {
    FA18_SCENE_ALTERNATE_RECORD_PREPARED = 0,
    FA18_SCENE_ALTERNATE_RECORD_TERMINATOR = 1,
    FA18_SCENE_ALTERNATE_RECORD_SKIPPED = 2
} FA18SceneAlternateRecordRoute;

typedef struct {
    uint16_t header;
    uint8_t selector;
    uint16_t shift;
    uint16_t kind;
    int16_t tuple[3];
    uint32_t work;
    uint32_t handler;
    uint8_t component_ready;
} FA18SceneAlternateRecordState;

/* `$C1CE38-$C1CEA3`: prepare a single `$C4F6CA`-family record.  The caller
 * owns list traversal and the later `$C1D0B6`/indirect-handler stages. */
int fa18_prepare_scene_alternate_record(
    const uint8_t bytes[FA18_SCENE_PLACEMENT_BYTES], int32_t guard_coordinate,
    FA18SceneAlternateHandlerLookup lookup, void *context,
    FA18SceneAlternateRecordState *state, FA18SceneAlternateRecordRoute *route);

#endif
