#ifndef FA18_NATIVE_POSTFLIGHT_H
#define FA18_NATIVE_POSTFLIGHT_H
#include "native_record_selection.h"
#include "native_record_view.h"
#include "native_context_publication.h"

typedef enum {
    FA18_POSTFLIGHT_FINISH,FA18_POSTFLIGHT_THREE,FA18_POSTFLIGHT_FOUR,
    FA18_POSTFLIGHT_FIVE,FA18_POSTFLIGHT_SIX,FA18_POSTFLIGHT_SEVEN,
    FA18_POSTFLIGHT_NINE,FA18_POSTFLIGHT_125,FA18_POSTFLIGHT_OTHER
} FA18NativePostflightMode;
typedef struct FA18NativePostflight FA18NativePostflight;
typedef struct {
    /* Explicit parent-proof seam when publication is not yet bound.
     * Production can bind the actual ordinary-state publication below. */
    int (*prepare)(void *context,FA18NativePostflight *state,unsigned slot,
                       uint16_t event,int seven);
    void *context;
} FA18NativePostflightOps;
struct FA18NativePostflight {
    FA18NativeSceneRecords *records;
    FA18NativeRecordSelection *selection;
    const PortFieldWindow *parameters;
    FA18NativeRecordViewWork *view_work;
    const FA18NativePostflightOps *ops;
    FA18NativeContextPublication *publication;
    const PortFieldByte *phase_fields;
    uint16_t *current_slot,*target_record,*dispatch_gate,*command_word,*view_heading;
    uint8_t *space_latch,*report_latch,*status,*blocked,*mode,*player_phase,*player_flags_f,
        *sequence_phase,*sequence_step,*context_gate,*post_input_event,*saved_view,*view_side,
        *saved_context,*context_select,*context_smooth,*context_started,*context_clear,*refresh,
        *limit,*admitted,*aux,*ready_mode;
};
/* Complete C09E06 and all its mode owners/shared tails. Selection release,
 * C0A3EA readiness and C0A12E table restoration are direct native operations.
 * event is used by standalone mode entries; FINISH reads the source selection
 * word before release, retaining its high byte for the generic phase write. */
int fa18_schedule_native_postflight(FA18NativePostflight *state,
                                      FA18NativePostflightMode mode,uint16_t event);
int fa18_restore_native_postflight_view(FA18NativePostflight *state,unsigned slot);
int fa18_native_postflight_ready(FA18NativePostflight *state,int *ready);
#endif
