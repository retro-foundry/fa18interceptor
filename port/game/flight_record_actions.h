#ifndef FA18_FLIGHT_RECORD_ACTIONS_H
#define FA18_FLIGHT_RECORD_ACTIONS_H
#include "memory.h"
/* Full working values are retained where the source mixes byte/word/long
 * operations. Child consumers can replace both values and record cursors. */
enum FlightActionValue { FA_PRIMARY,FA_SELECTOR,FA_DETAIL,FA_X,FA_Y,FA_Z,FA_PRODUCT_A,FA_PRODUCT_B,
    FA_STATS,FA_RECORD,FA_SOURCE,FA_STREAM,FA_COEFFICIENTS,FA_COPY };
typedef struct {
    uint32_t primary,selector,detail,x,y,z,product_a,product_b;
    gaddr stats,record,source,stream,coefficients,copy;
} FlightActionState;
enum FlightActionChild {
    FA_RELEASE_ACTION,FA_ACTION_SOUND,FA_MANOEUVRE_ACTION,FA_SOUND_MESSAGE,
    FA_MAGNITUDE_ALERT,FA_CLONE_MESSAGE,FA_CLONE_PROJECTION,FA_RESET_STREAM_RECORD,FA_BEGIN_STREAM,
    FA_STREAM_END_MESSAGE,FA_RESET_NEXT_RECORD,FA_NEXT_STREAM,FA_STREAM_LIMIT_MESSAGE,FA_NEXT_MESSAGE,
    FA_REFRESH_ACTION_VIEW,FA_ACTION_ROTATION,FA_ACTION_MATRIX,FA_ACTION_NORMALISE,
    FA_DIRECTION_FAULT,FA_DIRECTION_LENGTH
};
enum FlightActionPhase {
    FA_BYTE,FA_WORD,FA_LONG,FA_POINTER,FA_ADD_BYTE,FA_ADD_WORD,FA_ADD_LONG,FA_SUB_BYTE,FA_SUB_WORD,FA_SUB_LONG,
    FA_AND_BYTE,FA_AND_WORD,FA_OR_BYTE,FA_EXT_WORD,FA_EXT_LONG,FA_ASR_WORD,FA_ASR_LONG,FA_ASL_WORD,FA_ASL_LONG,
    FA_LSR_BYTE,FA_LSR_WORD,FA_SWAP,FA_NEG_WORD,FA_MULTIPLY,
    FA_TEST_BYTE,FA_TEST_WORD,FA_COMPARE_BYTE,FA_COMPARE_WORD,FA_COMPARE_LONG,FA_COMPARE_ADDRESS,FA_BIT_TEST,
    FA_STORE_BYTE,FA_STORE_WORD,FA_STORE_LONG,FA_MEMORY_ADD_WORD,FA_MEMORY_BIT_SET,FA_MEMORY_BIT_CLEAR,
    FA_COPY_RECORD,FA_SOUND_ARGUMENTS,FA_SAVE_RECORD,FA_RESTORE_RECORD,FA_SAVE_RECORD_SOURCES,FA_RESTORE_RECORD_SOURCES,
    FA_LOAD_DESCRIPTOR,FA_LOAD_COEFFICIENTS,FA_LOAD_MOTION,FA_DIRECTION_ARGUMENTS,FA_DIRECTION_DIVIDE,FA_DIRECTION_PUBLISH
};
typedef struct {
    FlightActionState (*consume)(void *context,enum FlightActionChild child);
    void (*observe)(void *context,enum FlightActionPhase phase,enum FlightActionValue field,uint32_t value,uint32_t operand);
    FlightActionState (*divide_exception)(void *context);
    void *context;
} FlightActionHooks;
/* $C230E8/$C23116: select the original record action from its low flag nibble. */
void select_flight_record_action(FlightActionState w,int allow_release,const FlightActionHooks *h);
/* $C23186: the original nine-word sound argument table. */
void queue_flight_record_action_sound(const FlightActionHooks *h);
/* $C23228/$C233AA/$C23578: magnitude alert, record clone and control stream. */
void advance_flight_record_control(FlightActionState w,const FlightActionHooks *h);
void advance_flight_record_stream(FlightActionState w,const FlightActionHooks *h);
void select_next_flight_record_stream(FlightActionState w,const FlightActionHooks *h);
/* $C23354: internal source arm unreachable after the sealed BNE/BEQ pair.
 * Kept separately for source-segment proof; never a registered callable owner. */
void append_flight_record_stream(FlightActionState w,const FlightActionHooks *h);
/* $C236AA/$C23716/$C2377E: original clone/setup and common action-motion tail. */
void initialise_flight_record_manoeuvre(FlightActionState w,const FlightActionHooks *h);
void initialise_flight_record_release(FlightActionState w,const FlightActionHooks *h);
void try_flight_record_action(FlightActionState w,const FlightActionHooks *h);
/* $C2385A: common internal action-motion tail, not a separate callable owner. */
void apply_flight_record_action_motion(FlightActionState w,const FlightActionHooks *h);
/* $C257EC: direction scaling, with the original nonreturning zero-scale fault. */
void normalise_flight_record_direction(gaddr frame,const FlightActionHooks *h);
#endif
