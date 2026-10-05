#include "map_packet_pass_selector.h"

#include "map_packet_control_stream.h"

int fa18_select_map_packet_pass(const FA18MapPacketPassSelectorInput *input,
                                FA18MapPacketPassSelectorResult *result) {
    if (!input || !result) return -1;
    if (fa18_initialize_map_packet_directory(input->layout, &result->directory) != 0)
        return -1;
    FA18MapPacketCoordinateSetupInput coordinate = input->coordinate;
    coordinate.coordinate_bin_shift = result->directory.coordinate_bin_shift;
    coordinate.alternate_layout = result->directory.layout ==
        FA18_MAP_PACKET_DIRECTORY_WIDE;
    if (fa18_prepare_map_packet_coordinate_setup(&coordinate, &result->coordinate) != 0)
        return -1;

    if (result->directory.layout == FA18_MAP_PACKET_DIRECTORY_NORMAL) {
        const FA18MapPacketControlStreamInput normal = {
            result->coordinate.local_x, result->coordinate.local_y, input->metric
        };
        FA18MapPacketControlStreamResult stream;
        if (fa18_select_normal_map_packet_control_stream(&normal, &stream) != 0)
            return -1;
        result->control_stream_address = stream.stream_address;
        return 0;
    }

    FA18MapPacketWideControlStreamResult wide;
    FA18MapPacketWideControlStreamRoute wide_route;
    if (fa18_select_wide_map_packet_control_stream(input->metric, &wide,
                                                    &wide_route) != 0)
        return -1;
    if (wide_route != FA18_MAP_PACKET_WIDE_LOW_FILTER) {
        result->control_stream_address = wide.stream_address;
        return 0;
    }

    const FA18MapPacketLowFilterInput filter = {
        input->metric_selector, input->table_selector, input->metric,
        input->resolve_low_filter_row, input->context
    };
    FA18MapPacketLowFilterResult filtered;
    FA18MapPacketLowFilterRoute filter_route;
    if (fa18_run_map_packet_low_filter(&filter, &filtered, &filter_route) != 0)
        return -1;
    if (filter_route == FA18_MAP_PACKET_LOW_FILTER_MATCH) {
        result->control_stream_address = filtered.stream_address;
        return 0;
    }
    const FA18MapPacketColumnTableInput column = {
        input->table_selector, input->metric_selector, input->metric
    };
    FA18MapPacketColumnTableResult selected;
    if (fa18_select_map_packet_column_table(&column, &selected) != 0) return -1;
    result->control_stream_address = selected.stream_address;
    return 0;
}
