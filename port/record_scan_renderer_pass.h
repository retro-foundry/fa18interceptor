#ifndef FA18_RECORD_SCAN_RENDERER_PASS_H
#define FA18_RECORD_SCAN_RENDERER_PASS_H

#include <stdint.h>

#include "flagged_slot_scan.h"

typedef struct {
    uint32_t renderer_bound;
    uint8_t scan_counter;
    uint8_t table_end;
} FA18RecordScanRendererPassState;

typedef struct {
    uint8_t scan_mode;
    uint8_t skip_countdown;
    uint16_t flags;
    uint8_t auxiliary_flag;
} FA18RecordScanRendererPassInput;

typedef enum {
    FA18_RECORD_SCAN_RENDERER_PASS_COMPLETE,
    FA18_RECORD_SCAN_RENDERER_PASS_CANDIDATE_CONTINUATION
} FA18RecordScanRendererPassRoute;

/* `$C1518C-$C1522D`, `$C153BC-$C153FB`, and `$C2F490`: run the fully bounded
 * direct scan path. A mode/flag gate or a set slot `+$26` bit zero transfers
 * to the unported candidate body, reported without executing a substitute. */
int fa18_run_record_scan_renderer_pass(
    FA18FlaggedSlot slots[FA18_FLAGGED_SLOT_SCAN_SLOTS],
    const FA18RecordScanRendererPassInput *input,
    FA18RecordScanRendererPassState *state,
    FA18RecordScanRendererPassRoute *route);

#endif
