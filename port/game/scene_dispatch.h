#ifndef FA18_GAME_SCENE_DISPATCH_H
#define FA18_GAME_SCENE_DISPATCH_H

#include <stdint.h>

#include "memory.h"

/* $C28B34: consume count+1 ten-byte scene dispatch entries and return the
 * last entry's status (0 created, -1 skipped). */
int dispatch_scene_records(gaddr stream, uint16_t count, int8_t previous_status);

/* $C28AFE: dispatch one entry, then aim the newly created record. */
int initialize_scene_record(gaddr stream);

#endif
