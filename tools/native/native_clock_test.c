#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include "../../port/game/native/clock.h"
#include "../../port/game/native/storage.h"
#include "../../port/game/globals.h"
#include "../../port/game/memory.h"

static NativeStorage storage;
typedef struct { uint64_t microseconds; unsigned calls; } Sample;
static uint64_t read_sample(void *context) {
    Sample *sample=context;++sample->calls;return sample->microseconds;
}
int main(void) {
    native_storage_bind(&storage);
    /* Actual original C28722 input, retained by the escort clock probe.
     * C16D04 must copy every microsecond bit through its request boundary. */
    Sample sample={976083,0};
    native_clock_set_source(read_sample,&sample);
    native_clock_set(500); /* PAL notification must not quantize host input. */
    native_clock_sample();
    assert(sample.calls==1 && rd_u32(READOUT_SAMPLE)==0 && rd_u32(READOUT_SAMPLE+4)==976083);
    assert(rd_u16(SCENE_DISPATCH_BITS)==58579);
    assert(rd_u8(MENU_TIME_REQUEST+8)==5 && rd_u16(MENU_TIME_REQUEST+28)==10);
    sample.microseconds=999999;native_clock_sample();
    assert(rd_u32(READOUT_SAMPLE)==0 && rd_u32(READOUT_SAMPLE+4)==999999);
    sample.microseconds=1000000;native_clock_sample();
    assert(rd_u32(READOUT_SAMPLE)==1 && rd_u32(READOUT_SAMPLE+4)==0);
    for(unsigned low=0;low<32;++low) {
        sample.microseconds=1000000+low;native_clock_sample();
        assert(rd_u32(READOUT_SAMPLE)==1 && rd_u32(READOUT_SAMPLE+4)==low);
    }
    assert(native_clock_stats().low_bits_seen==UINT32_MAX && sample.calls==35);
    sample.microseconds=UINT64_C(4294967296)*1000000+123456;native_clock_sample();
    assert(rd_u32(READOUT_SAMPLE)==0 && rd_u32(READOUT_SAMPLE+4)==123456);
    native_clock_set_source(NULL,NULL);native_clock_set(49);native_clock_sample();
    assert(rd_u32(READOUT_SAMPLE)==0 && rd_u32(READOUT_SAMPLE+4)==980000);
    native_clock_set(50);native_clock_sample();
    assert(rd_u32(READOUT_SAMPLE)==1 && rd_u32(READOUT_SAMPLE+4)==0);
    assert(native_clock_stats().requests==2 && native_clock_stats().low_bits_seen==1);
    puts("C16D04 retains host precision, second rollover and all scene-selection bits; PAL diagnostics remain deterministic");
    return 0;
}
