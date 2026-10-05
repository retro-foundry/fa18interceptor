#ifndef FA18_NATIVE_RECORD_CONTROL_H
#define FA18_NATIVE_RECORD_CONTROL_H
#include "native_scene_records.h"
#include "native_record_view.h"
#include "field_window.h"

typedef struct {
    enum { FA18_CONTROL_STREAM_EMPTY,FA18_CONTROL_STREAM_COMMANDS,FA18_CONTROL_STREAM_CODE } kind;
    uint8_t code; /* low byte of the original negative stream value */
    PortFieldWindow commands; /* centred at the first byte after length word */
} FA18NativeControlStream;
typedef struct {
    const FA18NativeControlStream *streams; size_t count; int first_index;
    PortFieldWindow messages,metadata; /* original C23622/C2366A rows, 8-byte stride */
} FA18NativeRecordControlAssets;
typedef struct FA18NativeRecordControl FA18NativeRecordControl;
typedef struct {
    int (*tone)(void *context,FA18NativeRecordControl *state,unsigned program);
    int (*message)(void *context,FA18NativeRecordControl *state,uint16_t code);
    /* Actual C28722; parent preserves its record identity. This child also
     * produces the ordinary carried axis consumed by the later view owner. */
    int (*initialize_scene)(void *context,FA18NativeRecordControl *state,unsigned slot,uint32_t *carried_axis);
    void *context;
} FA18NativeRecordControlOps;
struct FA18NativeRecordControl {
    FA18NativeSceneRecords *records;
    const FA18NativeRecordControlAssets *assets;
    const FA18NativeRecordControlOps *ops;
    FA18NativeRecordViewWork *view_work;
    uint16_t *magnitude_bias,*stream_position,*current_slot,*target_slot;
    uint8_t *magnitude_alert,*mode,*stream_index,*stream_pending,*stream_gate;
    uint8_t *post_input_event,*playback_enable,*origin_enable,*sequence_phase,*view_selector,*stream_view_state,*scene_redraw;
};
/* Complete C23228 and C233AA, including shared C233D6 player and actual
 * C23578 next-stream owner. C23354 append is a proven unreachable arm of
 * the original BNE/BEQ pair, not a callable entry or a playback substitute. */
int fa18_update_native_record_control(FA18NativeRecordControl *state,unsigned slot,unsigned companion_slot);
int fa18_update_native_record_stream(FA18NativeRecordControl *state,unsigned slot);
int fa18_next_native_record_stream(FA18NativeRecordControl *state,unsigned slot);
#endif
