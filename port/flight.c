#include "flight.h"

static int8_t update_lane(int8_t value, uint8_t field, uint8_t increment_code,
                          int8_t minimum, int8_t maximum, int8_t step) {
    if (field == 0) return value;
    int value16 = value;
    if (field == increment_code) {
        value16 = value16 < 0 ? step : value16 + step;
    } else {
        value16 = value16 > 0 ? -step : value16 - step;
    }
    if (value16 < minimum) value16 = minimum;
    if (value16 > maximum) value16 = maximum;
    return (int8_t)value16;
}

int fa18_flight_update_control_lanes(FA18FlightControlLanes *lanes,
                                     uint8_t packed_control) {
    if (!lanes) return -1;
    lanes->lane[0] = update_lane(lanes->lane[0], packed_control & 0x30u,
                                 0x10u, -20, 20, 1);
    lanes->lane[1] = update_lane(lanes->lane[1], packed_control & 0xc0u,
                                 0x40u, -20, 20, 1);
    lanes->lane[2] = update_lane(lanes->lane[2], packed_control & 0x0cu,
                                 0x04u, -60, 60, 3);
    return 0;
}

int fa18_flight_commit_vertical(FA18FlightPose *pose, int32_t delta) {
    if (!pose) return -1;
    pose->altitude += delta;
    pose->vertical_delta = delta;
    return 0;
}

int fa18_flight_publish_horizontal(FA18FlightPose *pose,
                                   int32_t lateral, int32_t forward) {
    if (!pose) return -1;
    pose->lateral_delta = lateral - pose->lateral;
    pose->forward_delta = forward - pose->forward;
    pose->lateral = lateral;
    pose->forward = forward;
    return 0;
}

int fa18_flight_apply_motion_terms(FA18FlightPose *pose,
                                   int32_t lateral_delta,
                                   int32_t vertical_delta,
                                   int32_t forward_delta) {
    if (!pose) return -1;
    if (fa18_flight_publish_horizontal(pose,
            pose->lateral + lateral_delta,
            pose->forward + forward_delta) != 0) return -1;
    return fa18_flight_commit_vertical(pose, vertical_delta);
}
