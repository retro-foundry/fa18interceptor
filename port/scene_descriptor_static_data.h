#ifndef FA18_SCENE_DESCRIPTOR_STATIC_DATA_H
#define FA18_SCENE_DESCRIPTOR_STATIC_DATA_H

#include <stddef.h>
#include <stdint.h>

#include "hunk.h"

enum {
    FA18_SCENE_DESCRIPTOR_HUNK = 16,
    FA18_SCENE_DESCRIPTOR_RUNTIME_BASE = 0x00c22048,
    FA18_SCENE_DESCRIPTOR_POINTER_COUNT = 4
};

/* `$C1CB74` receives a relocated runtime address into original Hunk 16.
 * Keep its pointer fields as (target Hunk, target offset) pairs: the Hunk
 * loader deliberately retains pre-relocation offsets rather than host
 * addresses, and the port must not manufacture either kind of address. */
typedef struct {
    uint32_t target_hunk[FA18_SCENE_DESCRIPTOR_POINTER_COUNT];
    uint32_t target_offset[FA18_SCENE_DESCRIPTOR_POINTER_COUNT];
} FA18SceneDescriptorPointers;

typedef struct {
    const FA18Hunks *hunks;
    const uint8_t *bytes;
    size_t size;
} FA18SceneDescriptorStaticData;

/* Bind the original Hunk-16 payload that contains `$C22048-$C22C73`. */
int fa18_load_scene_descriptor_static_data(const FA18Hunks *hunks,
                                           FA18SceneDescriptorStaticData *data);

/* Resolve one source runtime descriptor address and its four consecutive
 * `$C1CC66-$C1CC78` longword pointers through original HUNK_RELOC32 records. */
int fa18_resolve_scene_descriptor_pointers(
    const FA18SceneDescriptorStaticData *data, uint32_t runtime_address,
    FA18SceneDescriptorPointers *pointers);

/* Obtain target-Hunk bytes for a relocation pair. This is deliberately
 * bounded and does not translate the pair into a process pointer or an
 * invented Amiga runtime address. */
int fa18_resolve_scene_descriptor_target(const FA18SceneDescriptorStaticData *data,
                                         uint32_t target_hunk,
                                         uint32_t target_offset,
                                         const uint8_t **bytes, size_t *size);

#endif
