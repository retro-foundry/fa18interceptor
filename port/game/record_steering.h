#ifndef FA18_RECORD_STEERING_H
#define FA18_RECORD_STEERING_H
#include "memory.h"
typedef struct {
 uint32_t controls,selection,turn;
 gaddr record;
} RecordSteeringState;
enum SteeringField { ST_CONTROLS=1,ST_SELECTION=2,ST_TURN=3 };
enum SteeringPhase { ST_BYTE,ST_WORD,ST_LONG,ST_AND_BYTE,ST_OR_BYTE,
 ST_TEST_WORD,ST_COMPARE_BYTE,ST_COMPARE_WORD,ST_BIT_TEST,ST_STORE_BYTE };
typedef struct {
 void (*observe)(void *,enum SteeringPhase,enum SteeringField,uint32_t,uint32_t);
 void *context;
} RecordSteeringHooks;
RecordSteeringState select_record_turn(RecordSteeringState,const RecordSteeringHooks *);
RecordSteeringState select_record_roll(RecordSteeringState,const RecordSteeringHooks *);
RecordSteeringState select_record_neutral(RecordSteeringState,const RecordSteeringHooks *);
RecordSteeringState select_record_pitch(RecordSteeringState,const RecordSteeringHooks *);
RecordSteeringState select_record_pitch_preserving_controls(RecordSteeringState,const RecordSteeringHooks *);
RecordSteeringState select_record_pitch_branch(RecordSteeringState,const RecordSteeringHooks *);
#endif
