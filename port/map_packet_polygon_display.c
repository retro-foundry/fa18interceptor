#include "map_packet_polygon_display.h"

int fa18_display_map_packet_polygon(
    void *context, const FA18MapPacketProjectionRecord *records,
    uint16_t record_count, uint16_t coordinate_shift) {
    FA18MapPacketPolygonDisplay *display = context;
    FA18ClipTuple input[FA18_POLYGON_MAX_VERTICES];
    int result;

    if (!display || !display->submission || !display->result || !records ||
        !record_count || record_count > FA18_POLYGON_MAX_VERTICES ||
        coordinate_shift >= 16)
        return -1;
    for (uint16_t index = 0; index < record_count; ++index)
        input[index] = (FA18ClipTuple){records[index].value[0],
                                       records[index].value[1],
                                       records[index].value[2]};
    result = fa18_run_polygon_display_pipeline(input, record_count,
                                               (int16_t)coordinate_shift,
                                               display->submission,
                                               display->result);
    return result < 0 ? -1 : 0;
}
