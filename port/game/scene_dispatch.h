#ifndef FA18_GAME_SCENE_DISPATCH_H
#define FA18_GAME_SCENE_DISPATCH_H

#include <stdint.h>

#include "memory.h"

/* $C28B34: consume count+1 ten-byte scene dispatch entries and return the
 * last entry's status (0 created, -1 skipped). */
int dispatch_scene_records(gaddr stream, uint16_t count, int8_t previous_status);

/* $C28AFE: dispatch one entry, then aim the newly created record. */
int initialize_scene_record(gaddr stream);

/* $C28722: select the scene stream and initialize every admitted record.
 * Hooks let register glue replay the call boundaries while C owns iteration.
 * NULL hooks run the same C operation without register replay. */
typedef struct SceneDispatchHooks {
    void (*after_date)(void *context);
    void (*selected)(gaddr stream, void *context);
    void (*scan)(gaddr stream, int accepted, int first, void *context);
    void (*finished)(gaddr end, void *context);
    void (*special)(void *context);
    void *context;
} SceneDispatchHooks;

void initialize_scene_from_mode(const SceneDispatchHooks *hooks);

#endif
