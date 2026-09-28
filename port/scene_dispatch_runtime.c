#include "scene_dispatch_runtime.h"
#include "coordinate_update_positive_pair.h"
#include "scene_dispatch_negative_coordinate_pose.h"

#include <string.h>

static int16_t word_at(const uint8_t *bytes, unsigned offset) {
    return (int16_t)fa18_be16(bytes + offset);
}

static int32_t long_at(const uint8_t *bytes, unsigned offset) {
    return (int32_t)fa18_be32(bytes + offset);
}

static int negative_geometry(void *context, uint16_t selector_index,
                             int16_t geometry[5]) {
    return fa18_scene_dispatch_negative_geometry(context, selector_index, geometry);
}

static int positive_coordinate(void *context,
                               const FA18SceneCoordinateUpdateInput *input,
                               int16_t output[2]) {
    return fa18_update_coordinate_positive_pair(context, input, output);
}

static int load_template(const FA18Hunks *hunks, int16_t source_offset,
                         FA18SceneDispatchTemplate *template_record) {
    if (!hunks || !template_record || source_offset < 0 ||
        FA18_SCENE_DISPATCH_TEMPLATE_HUNK >= hunks->count)
        return -1;
    for (unsigned i = 0; i != FA18_SCENE_DISPATCH_TEMPLATE_POINTERS; ++i)
        if (!fa18_hunk_pointer(hunks, FA18_SCENE_DISPATCH_TEMPLATE_HUNK,
                               (uint32_t)source_offset + i * 4u,
                               &template_record->segment[i],
                               &template_record->offset[i]))
            template_record->segment[i] = UINT32_MAX;
    return 0;
}

static int template_class(const FA18Hunks *hunks,
                          const FA18SceneDispatchTemplate *template_record,
                          uint8_t *class_nibble) {
    const uint32_t segment_index = template_record->segment[1];
    const uint32_t offset = template_record->offset[1];
    const FA18HunkSegment *segment;
    const uint8_t *descriptor;
    int16_t selector;
    uint16_t table_offset;

    if (!hunks || !template_record || !class_nibble || segment_index >= hunks->count)
        return -1;
    segment = &hunks->segments[segment_index];
    if (!segment->data || offset > segment->size || segment->size - offset < 7u)
        return -1;
    descriptor = segment->data + offset;
    selector = word_at(descriptor, 0);
    if (selector < 0) {
        table_offset = (uint16_t)selector & 0x0fffu;
    } else if (selector & 0x4000) {
        table_offset = (uint16_t)word_at(descriptor, 2) & 0x0fffu;
    } else {
        table_offset = (uint16_t)word_at(descriptor, 4) & 0x0fffu;
    }
    if ((uint32_t)table_offset + 7u > segment->size - offset) return -1;
    *class_nibble = descriptor[6u + table_offset] & 0x0fu;
    return 0;
}

int fa18_initialize_scene_dispatch_runtime(
    const FA18Hunks *hunks, const FA18SceneDispatchTable *table,
    const FA18SceneDispatchSelectionInput *selection_input,
    int16_t inherited_d7, const FA18CoordinateAngleTable *coordinate_table,
    const FA18RecordMatrixUpdateOps *matrix_ops,
    FA18SceneDispatchRuntime *runtime) {
    FA18SceneDispatchSelection selection;
    int selected;

    FA18SceneDispatchSourceRecord first_source = {0};
    uint16_t last_record_index = 0;
    int last_was_created = 0;
    if (!hunks || !table || !selection_input || !coordinate_table || !matrix_ops || !runtime)
        return -1;
    runtime->hunks = hunks;
    selected = fa18_select_scene_dispatch_records(table, selection_input, &selection);
    if (selected != 0) return selected;
    for (uint16_t source_index = 0;; ++source_index) {
        FA18SceneDispatchSourceRecord source;
        FA18SceneDispatchGeometry geometry;
        const int decoded = fa18_scene_dispatch_source_record(
            table, selection.records, source_index, &source);
        uint16_t record_index;
        uint8_t class_nibble;

        if (decoded <= 0) {
            if (decoded < 0 || !last_was_created) return decoded < 0 ? -1 : 0;
            if ((int16_t)first_source.source_word_1 >= 0) return -1;
            const int published = fa18_publish_scene_dispatch_negative_coordinate_pose(
                &runtime->record[last_record_index], (int16_t)first_source.source_word_1,
                negative_geometry, (void *)table, positive_coordinate,
                (void *)coordinate_table, matrix_ops);
            return published < 0 ? -1 : 0;
        }
        if (!source_index) first_source = source;
        record_index = source.source_word_1 & 0x007fu;
        if (record_index >= FA18_SCENE_DISPATCH_RECORD_COUNT ||
            load_template(hunks, source.source_offset,
                          &runtime->template_record[record_index]) != 0 ||
            template_class(hunks, &runtime->template_record[record_index],
                           &class_nibble) != 0) {
            return -1;
        }
        /* `$C28B84` already-published bit-6 route retains the live slot. */
        if (runtime->record[record_index].bytes[1] & 0x40u) {
            last_was_created = 0;
            continue;
        }
        if (fa18_scene_dispatch_geometry(table, source.geometry_offset, &geometry) != 0)
            return -1;
        const FA18SceneDispatchCreateInput input = {
            source.record_type, (uint8_t)record_index, class_nibble,
            source.source_flags, source.source_word_1, source.geometry_offset,
            geometry.coordinate_x, geometry.coordinate_z,
            geometry.component_x, geometry.component_z, geometry.altitude,
            inherited_d7
        };
        if (fa18_create_scene_dispatch_record(&runtime->record[record_index],
                                              &input, matrix_ops) != 0)
            return -1;
        last_record_index = record_index;
        last_was_created = 1;
    }
}

int fa18_scene_dispatch_runtime_resolve_negative_record(
    void *context, uint16_t record_index, FA18SceneNegativePoseRecord *record) {
    FA18SceneDispatchRuntime *runtime = context;
    const FA18SceneDispatchRecord *source;
    if (!runtime || !record || record_index >= FA18_SCENE_DISPATCH_RECORD_COUNT)
        return -1;
    source = &runtime->record[record_index];
    record->flags_byte_01 = source->bytes[1];
    memcpy(record->matrix.value, source->matrix_update.attitude_matrix,
           sizeof record->matrix.value);
    record->base.value[0] = long_at(source->bytes, 0x14);
    record->base.value[1] = long_at(source->bytes, 0x18);
    record->base.value[2] = long_at(source->bytes, 0x1c);
    record->angles[0] = word_at(source->bytes, 0x66);
    record->angles[1] = word_at(source->bytes, 0x68);
    record->angles[2] = word_at(source->bytes, 0x6a);
    return 0;
}

int fa18_scene_dispatch_runtime_resolve_negative_descriptor(
    void *context, uint16_t record_index, FA18SceneNegativePoseDescriptor *descriptor) {
    FA18SceneDispatchRuntime *runtime = context;
    const FA18SceneDispatchTemplate *template_record;
    const FA18HunkSegment *segment;
    uint32_t segment_index, offset;

    if (!runtime || !descriptor || record_index >= FA18_SCENE_DISPATCH_RECORD_COUNT)
        return -1;
    template_record = &runtime->template_record[record_index];
    if (!runtime->hunks) return -1;
    segment_index = template_record->segment[4];
    offset = template_record->offset[4];
    if (segment_index >= runtime->hunks->count) return -1;
    segment = &runtime->hunks->segments[segment_index];
    if (!segment->data || offset > segment->size || segment->size - offset < 6u)
        return -1;
    descriptor->value_02 = long_at(segment->data + offset, 2);
    return 0;
}
