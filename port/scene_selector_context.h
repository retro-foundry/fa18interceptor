#ifndef FA18_SCENE_SELECTOR_CONTEXT_H
#define FA18_SCENE_SELECTOR_CONTEXT_H

#include <stddef.h>
#include <stdint.h>

enum { FA18_SCENE_SELECTOR_CONTEXT_RECORD_BYTES = 10 };

/* Caller-owned direct inputs at `$C1C8B0`: either the selected control
 * record or the alternate `$C45C3E/$C45C46` longword pair. */
typedef struct {
    uint8_t alternate_source_enabled;
    const uint8_t *active_record;
    size_t active_record_size;
    int32_t alternate_first;
    int32_t alternate_second;
} FA18SceneSelectorContextInput;

/* Direct state written by `$C1C8F2-$C1C906`. */
typedef struct {
    int16_t first_selector;
    int16_t second_selector;
    uint8_t append_enabled;
    uint8_t first_status;
    uint8_t second_status;
} FA18SceneSelectorContextState;

/* `$C1C8B0-$C1C912`: derive the first selector pair from the selected record
 * or masked alternate longwords, then arm the shared append state. The
 * preceding `$C1CA82` route and following `$C1D10C` consumers remain owned
 * by their respective source stages. */
int fa18_update_scene_selector_context(
    const FA18SceneSelectorContextInput *input,
    FA18SceneSelectorContextState *state);

#endif
