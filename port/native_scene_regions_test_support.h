#ifndef FA18_NATIVE_SCENE_REGIONS_TEST_SUPPORT_H
#define FA18_NATIVE_SCENE_REGIONS_TEST_SUPPORT_H
/* Contract fixtures only. An explicit empty source directory prevents these
 * composed tests from depending on unrelated mission assets. The standalone
 * region proof supplies complete original-shaped rows and actual children. */
#include "native_control_record_update.h"
typedef struct {
    FA18NativeSceneRegionAssets assets;
    PortFieldWindow directory[2];
    uint8_t box[10],recorder,occupied,active,reuse;
    uint16_t random,jitter_x,jitter_z;
} FA18SceneRegionsTestStorage;
static void fa18_test_bind_scene_regions(FA18NativeSceneRegions *regions,
    FA18SceneRegionsTestStorage *storage,FA18NativeControlRecordUpdate *update) {
    storage->assets=(FA18NativeSceneRegionAssets){.parameters=&update->view->assets->parameters,
        .directory=storage->directory,.directory_count=2};
    *regions=(FA18NativeSceneRegions){.records=update->records,.view_work=update->view_work,
        .assets=&storage->assets,.pointer_groups=update->placement->pointer_groups,
        .pointer_group_count=update->placement->pointer_group_count,
        .recorder_on=&storage->recorder,.occupied=&storage->occupied,.mode=update->view->mode,
        .admitted=update->view->admitted,.active=&storage->active,.reuse_jitter=&storage->reuse,
        .random_word=&storage->random,.jitter_x=&storage->jitter_x,.jitter_z=&storage->jitter_z};
    update->regions=regions;
}
#endif
