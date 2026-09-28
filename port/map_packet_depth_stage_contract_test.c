#include "map_packet_depth_stage.h"

#include <assert.h>

int main(void) {
    FA18ProjectionPacket packet = {0, 0, 0, -125};
    FA18MapPacketDepthStageResult result;
    assert(fa18_prepare_map_packet_depth_stage(
               &(FA18MapPacketDepthStageInput){&packet, 0, 1, 0x80},
               &result) == 0);
    assert(result.renderer_words[0] == 2 && result.renderer_words[1] == 2 &&
           !result.renderer_words[2] && !result.renderer_words[3]);
    assert(result.metric == 125 && !result.run_normal_pass && result.run_wide_pass);

    packet.depth_metric = -0x1000;
    assert(fa18_prepare_map_packet_depth_stage(
               &(FA18MapPacketDepthStageInput){&packet, 1, 0, 0x80},
               &result) == 0);
    assert(result.renderer_words[0] == 0x000f && result.renderer_words[1] == -1);
    assert(result.metric == 0x1000 && result.run_normal_pass && result.run_wide_pass);
    assert(fa18_prepare_map_packet_depth_stage(0, &result) == -1);
    return 0;
}
