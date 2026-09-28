#ifndef FA18_PARENT_UPDATE_PREFIX_H
#define FA18_PARENT_UPDATE_PREFIX_H
#include <stdint.h>
typedef int (*FA18ParentUpdatePrefixStage)(void *);
typedef struct { FA18ParentUpdatePrefixStage input_phase,pre_input,pre_update,prepare,indexed,main_update; void *context; } FA18ParentUpdatePrefixOps;
typedef struct { uint16_t frame_counter,local_frame,stage_marker; uint8_t skip_flag; } FA18ParentUpdatePrefixState;
int fa18_run_parent_update_prefix(FA18ParentUpdatePrefixState *,const FA18ParentUpdatePrefixOps *);
#endif
