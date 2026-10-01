#ifndef FA18_MAP_PACKET_RECORD_STAGE_H
#define FA18_MAP_PACKET_RECORD_STAGE_H

#include "map_detail_component_route.h"
#include "map_detail_fields.h"
#include "map_detail_gate.h"
#include "map_packet_relative_offset.h"
#include "map_packet_stage.h"

typedef struct {
    int8_t encoded_mode;
    FA18MapDetailGateInput gate;
    FA18MapDetailFieldsInput fields;
    FA18MapPacketStageInput packet_stage;
    uint8_t use_relative_offset;
    FA18MapPacketRelativeOffsetInput relative_offset;
    /* $C2ADD4 places A3 at a resolved packet before the header gate. */
    void (*publish_packet)(void *context, uint32_t packet_address);
    /* $C2AEF8 holds the completed packed component in D0. */
    void (*publish_seed)(void *context, uint32_t packed_seed);
    void (*publish_visibility_limit)(void *context, uint32_t limit_register);
} FA18MapPacketRecordStageInput;

typedef enum {
    FA18_MAP_PACKET_RECORD_DISPLAYED = 0,
    FA18_MAP_PACKET_RECORD_REJECTED = 1,
    FA18_MAP_PACKET_RECORD_COUNT_ERROR = 2,
    FA18_MAP_PACKET_RECORD_TERMINATOR = 3,
    FA18_MAP_PACKET_RECORD_FRAME_STOP = 4
} FA18MapPacketRecordStageRoute;

/* `$C2AD00-$C2AFF7`, excluding the caller-owned record walker and
 * coordinate-table selection: run one control byte through detail selection,
 * directory-relative packet lookup, visibility, stream selection,
 * static-pair transformation, and display. */
int fa18_run_map_packet_record_stage(
    const FA18MapPacketRecordStageInput *input,
    FA18MapPacketProjectionRecord *records, size_t record_capacity,
    uint16_t *record_count, FA18MapDetailGateResult *gate_result,
    FA18MapPacketRecordStageRoute *route);

#endif
