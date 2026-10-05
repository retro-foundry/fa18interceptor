#include "terrain_selector_origin_adjustment.h"
#include <assert.h>

int main(void) {
    uint8_t bytes[2]={0x40,0}; uint16_t magnitude=7; int16_t normalized[3]={1,2,3};
    PortFieldWindow table={.bytes=bytes,.byte_count=2};
    FA18NativeVectorMath math={&table,&magnitude,normalized};
    int32_t candidate[3]={0x120,0,0},smoothed[3]={0,0,0};
    int32_t origin[3]={0x401000,0x2000,0x3000},companion[3]={0,0,0};
    FA18TerrainSelectorOriginAdjustmentState state={.magnitude=0x4800,.candidate=candidate,
        .smoothed_delta=smoothed,.origin=origin,.negated_companion=companion,
        .shift=1,.vector_math=&math};
    assert(fa18_adjust_terrain_selector_origin(&state)==0);
    /* The quartered input enters actual normalization. Its shared outputs
     * are scaled with the saved shift and consumed by the origin publisher. */
    assert(state.magnitude==0x1200 && magnitude==0x48 && normalized[0]==511);
    assert(smoothed[0]==0x3fe && !smoothed[1] && !smoothed[2]);
    assert(origin[0]==0x4013fe && origin[1]==0x2000 && origin[2]==0x3000);
    assert(companion[0]==-0x13fe && companion[1]==-0x2000 && companion[2]==-0x3000);
    state.vector_math=NULL;
    assert(fa18_adjust_terrain_selector_origin(&state)==-1);
    assert(origin[0]==0x4013fe && fa18_adjust_terrain_selector_origin(NULL)==-1);
    return 0;
}
