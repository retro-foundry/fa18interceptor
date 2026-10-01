#include "map_packet_original_pass.h"

typedef struct {
    const FA18MapPacketOriginalPassInput *input;
} FA18MapPacketOriginalPassContext;

static int resolve_control_stream(void *context, uint32_t address,
                                  const uint8_t **stream, size_t *size) {
    FA18MapPacketOriginalPassContext *owner = context;
    if (!owner || !owner->input) return -1;
    return fa18_resolve_map_packet_control_stream(
        (void *)owner->input->static_data, address, stream, size);
}

int fa18_prepare_original_map_packet_record(
    const FA18MapPacketOriginalPassInput *input,
    const FA18MapPacketPassSelectorResult *pass, uint8_t mode,
    FA18MapPacketRecordStageInput *record) {
    size_t directory_offset;
    if (!input || !pass || !record || !input->static_data)
        return -1;
    if (pass->directory.record_base_address < FA18_MAP_PACKET_PACKET_RUNTIME_BASE)
        return -1;
    directory_offset = (size_t)(pass->directory.record_base_address -
                                FA18_MAP_PACKET_PACKET_RUNTIME_BASE);
    if (directory_offset >= input->static_data->packet_size) return -1;
    *record = input->record;
    record->use_relative_offset = 1;
    record->relative_offset = (FA18MapPacketRelativeOffsetInput){
        mode,
        pass->coordinate.row_min, pass->directory.row_max,
        pass->coordinate.column_min, pass->directory.column_max,
        pass->directory.layout == FA18_MAP_PACKET_DIRECTORY_WIDE,
        input->allow_negative_packet, pass->directory.record_base_address,
        input->static_data->packet_bytes + directory_offset,
        input->static_data->packet_size - directory_offset,
        fa18_resolve_map_packet_control_pair,
        fa18_resolve_map_packet_static_packet,
        (void *)input->static_data
    };
    return 0;
}

static int resolve_record(void *context, const FA18MapPacketPassSelectorResult *pass,
                          uint8_t mode, FA18MapPacketRecordStageInput *record) {
    FA18MapPacketOriginalPassContext *owner = context;
    return !owner ? -1 : fa18_prepare_original_map_packet_record(
        owner->input, pass, mode, record);
}

int fa18_run_original_map_packet_pass(
    const FA18MapPacketOriginalPassInput *input,
    FA18MapPacketProjectionRecord *records, size_t record_capacity,
    uint16_t *record_count, FA18MapPacketPassSelectorResult *pass,
    FA18MapPacketControlWalkerRoute *route) {
    FA18MapPacketOriginalPassContext context;
    FA18MapPacketPassRunnerInput runner;
    if (!input || !input->static_data) return -1;
    context.input = input;
    runner = (FA18MapPacketPassRunnerInput){
        input->selector, resolve_control_stream, resolve_record,
        &context
    };
    return fa18_run_map_packet_pass(&runner, records, record_capacity,
                                    record_count, pass, route);
}
