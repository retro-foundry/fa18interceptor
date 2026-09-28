#ifndef FA18_SCENE_ROOT_RECORD_H
#define FA18_SCENE_ROOT_RECORD_H

#include "scene_record_dispatch.h"
#include "scene_root_placement.h"

/* Publish the fields `$C0924A-$C095BE` writes into root slot zero.  The
 * record's type byte and every field not written by this path stay owned by
 * their source producers; this routine never invents them. */
int fa18_publish_scene_root_record(FA18SceneDispatchRecord *record,
                                   const FA18SceneRootPlacementState *placement,
                                   FA18SceneRootPlacementRoute route);

#endif
