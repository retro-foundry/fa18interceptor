#ifndef FA18_GAME_UPDATE_SEQUENCE_H
#define FA18_GAME_UPDATE_SEQUENCE_H
#include "memory.h"

/* One child identity per original call site; repeated producers have distinct
 * source return boundaries. Their game behavior stays owned by those children. */
enum UpdateSequenceChild {
    UPDATE_KEYS, UPDATE_POST_INPUT, UPDATE_NOTIFY, UPDATE_VIEW_CONTROLS,
    UPDATE_INDEXED_EMPTY, UPDATE_RECORDS, UPDATE_POST_RECORD_EMPTY,
    UPDATE_MATRIX, UPDATE_PROJECTION, UPDATE_OCTANT, UPDATE_ATTITUDE,
    UPDATE_CONTEXT, UPDATE_COCKPIT_SLIDE, UPDATE_LIST_RESET, UPDATE_BUFFERS,
    UPDATE_MAP, UPDATE_MATRIX_MARK, UPDATE_PRIMARY_SCENE, UPDATE_ALTERNATE_SCENE,
    UPDATE_GRID, UPDATE_FLAGGED_SCENE, UPDATE_RANGE_DECISION,
    UPDATE_TRUE_SCENE, UPDATE_TRUE_FOLLOWUP, UPDATE_FALSE_FOLLOWUP,
    UPDATE_FALSE_SCENE, UPDATE_MESSAGE, UPDATE_RECORD_STATUS,
    UPDATE_PANEL_FRAME, UPDATE_PANEL_IMAGE, UPDATE_POSTFLIGHT,
    UPDATE_HUD, UPDATE_THREAT_LIGHTS, UPDATE_COMPASS,
    UPDATE_HEADING, UPDATE_SPEED, UPDATE_RECORD_2B, UPDATE_ALTITUDE,
    UPDATE_RECORD_72, UPDATE_GAUGE, UPDATE_PANEL_MARK, UPDATE_WEAPON,
    UPDATE_STORES, UPDATE_GRID_Z, UPDATE_GRID_X, UPDATE_ZOOM, UPDATE_MODE_BAR,
    UPDATE_SCALE, UPDATE_MESSAGE_LINE, UPDATE_INDICATOR_BARS,
    UPDATE_CONTEXT_SPEED, UPDATE_CONTEXT_ALTITUDE, UPDATE_CONTEXT_HEADING,
    UPDATE_CONTEXT_MESSAGE, UPDATE_LOST_SELECTION, UPDATE_READOUT_EMPTY,
    UPDATE_RANGE_STAGE, UPDATE_SELECTION_STAGE, UPDATE_PERIODIC_READOUT,
    UPDATE_PERIODIC_WAIT, UPDATE_PERIODIC_PAGE, UPDATE_PERIODIC_REDRAW,
    UPDATE_IDLE_STATUS, UPDATE_IDLE_WAIT, UPDATE_AFTER_TICK, UPDATE_TAIL_DRAW,
    UPDATE_TAIL_READOUT, UPDATE_FINAL, UPDATE_DISPLAY_PLANES, UPDATE_DISPLAY_END,
    UPDATE_SEQUENCE_CHILD_COUNT
};
enum UpdateSequencePhase {
    UPDATE_SEQUENCE_BEGIN, UPDATE_SEQUENCE_MARKER, UPDATE_SEQUENCE_BYTE_TEST,
    UPDATE_SEQUENCE_BYTE_LOAD, UPDATE_SEQUENCE_LONG_COMPARE,
    UPDATE_SEQUENCE_DECISION, UPDATE_SEQUENCE_RECORD,
    UPDATE_SEQUENCE_TICK_COMPARE, UPDATE_SEQUENCE_TICK_SUBTRACT,
    UPDATE_SEQUENCE_ACTIVITY_DECREMENT, UPDATE_SEQUENCE_CONTEXT_LATCH,
    UPDATE_SEQUENCE_CONTEXT_MODE, UPDATE_SEQUENCE_ZERO_ARGUMENT,
    UPDATE_SEQUENCE_DROP_ARGUMENT, UPDATE_SEQUENCE_INCREMENT,
    UPDATE_SEQUENCE_END, UPDATE_SEQUENCE_DISPLAY_FLAGS
};
typedef struct {
    enum UpdateSequencePhase phase;
    uint32_t value, mask, limit;
    gaddr record;
} UpdateSequenceEvent;
typedef struct { uint32_t value; int owner_finished; } UpdateSequenceResult;
typedef struct {
    UpdateSequenceResult (*consume)(void *context,enum UpdateSequenceChild child);
    void (*observe)(void *context,const UpdateSequenceEvent *event);
    void *context;
} UpdateSequenceHooks;

/* Complete C0EFD4-C0F3C3. Saved tick schedules the periodic work; every
 * child-sensitive global is reread at its original branch boundary. */
void run_game_update_sequence(const UpdateSequenceHooks *hooks);
/* C0F048-C0F124 scene ordering shared by the complete frame and native
 * composition. Returns zero when the alternate display child exits the owner. */
int run_game_scene_sequence(const UpdateSequenceHooks *hooks);
/* Complete C0D730-C0D748 display-buffer decision and its two explicit children. */
void submit_update_display_buffers(const UpdateSequenceHooks *hooks);
#endif
