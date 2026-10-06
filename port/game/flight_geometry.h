#ifndef FA18_FLIGHT_GEOMETRY_H
#define FA18_FLIGHT_GEOMETRY_H
#include "memory.h"
/* Working values retain original byte/word halves during mixed-width record,
 * scene and geometry operations. Consumers return all changed working values. */
enum GeometryValue { FG_PRIMARY,FG_DETAIL,FG_X,FG_Y,FG_Z,FG_RATE_X,FG_RATE_Y,FG_RATE_Z,
    FG_ROOT,FG_RECORD,FG_GEOMETRY,FG_SCENE,FG_TABLE,FG_FACE };
typedef struct {
    uint32_t primary,detail,x,y,z,rate_x,rate_y,rate_z;
    gaddr root,record,geometry,scene,table,face;
    int child_equal;
} GeometryState;
enum GeometryChild { FG_ZONE_FAULT,FG_ZONE_PLACE,FG_LOWER_FACES,FG_DETAIL_FACES };
enum GeometryPhase {
    FG_BYTE,FG_WORD,FG_LONG,FG_POINTER,FG_ADD_BYTE,FG_ADD_WORD,FG_ADD_LONG,FG_SUB_BYTE,FG_SUB_WORD,FG_SUB_LONG,
    FG_AND_BYTE,FG_AND_WORD,FG_AND_LONG,FG_OR_BYTE,FG_EXT_WORD,FG_EXT_LONG,FG_SWAP,FG_ASR_WORD,FG_ASR_LONG,
    FG_ASL_WORD,FG_ASL_LONG,FG_LSR_WORD,FG_ROL_LONG,FG_NEG_WORD,FG_NEG_LONG,FG_MULTIPLY,
    FG_TEST_BYTE,FG_TEST_WORD,FG_TEST_LONG,FG_COMPARE_BYTE,FG_COMPARE_WORD,FG_COMPARE_LONG,
    FG_BIT_TEST,FG_BIT_SET,FG_BIT_CLEAR,FG_REGISTER_BIT_SET,FG_REGISTER_BIT_CLEAR,
    FG_STORE_BYTE,FG_STORE_WORD,FG_STORE_LONG,FG_MEMORY_ADD_BYTE,FG_MEMORY_ADD_WORD,FG_MEMORY_ADD_LONG,
    FG_MEMORY_SUB_WORD,FG_MEMORY_SUB_LONG,FG_DECREMENT,
    FG_LOAD_WORDS,FG_LOAD_LONGS,FG_STORE_WORDS,FG_STORE_LONGS,
    FG_SAVE_RECORD,FG_RESTORE_RECORD,FG_SAVE_CELL,FG_RESTORE_CELL,FG_SAVE_RATES,FG_RESTORE_RATES,
    FG_SAVE_ORIENTATION,FG_RESTORE_ORIENTATION,FG_SAVE_SCAN,FG_RESTORE_SCAN,FG_SOUND_ARGUMENTS,
    FG_BEGIN_FRAME,FG_END_FRAME,FG_NEG_BYTE,FG_EXCHANGE,FG_PUSH_INDEX,FG_POP_INDEX
};
typedef struct {
    GeometryState (*consume)(void *context,enum GeometryChild child);
    void (*observe)(void *context,enum GeometryPhase phase,enum GeometryValue field,uint32_t value,uint32_t operand);
    gaddr (*frame)(void *context);
    GeometryState (*restored)(void *context);
    gaddr (*stack)(void *context);
    void *context;
} GeometryHooks;
void record_position_history_complete(GeometryState w,const GeometryHooks *h);
void check_record_zone_exit_complete(GeometryState w,const GeometryHooks *h);
enum ZoneExitPhase { ZONE_BEGIN, ZONE_AFTER_FAULT, ZONE_AFTER_PLACE, ZONE_COMPLETE };
typedef struct { GeometryState work; enum ZoneExitPhase phase; } ZoneExitFrame;
int advance_record_zone_exit(ZoneExitFrame *frame,const GeometryHooks *hooks);
void update_candidate_record_complete(GeometryState w,const GeometryHooks *h);
void test_candidate_faces_complete(GeometryState w,gaddr frame,const GeometryHooks *h);
#endif
