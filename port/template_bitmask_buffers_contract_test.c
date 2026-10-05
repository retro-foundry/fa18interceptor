#include "template_bitmask_buffers.h"

#include <assert.h>
#include <string.h>

static void put16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

static void initialize_stream(uint8_t *bytes, size_t size) {
    assert(size >= 0x108);
    for (size_t row = 0; row != FA18_TEMPLATE_BITMASK_ROWS; ++row)
        put16(bytes + row * 2u, 0x100);
    put16(bytes + 0x100, 0xffff);
}

int main(void) {
    uint8_t first[0x108] = {0};
    uint8_t second[0x108] = {0};
    uint8_t third[0x108] = {0};
    uint8_t hunk[0x308] = {0};
    FA18TemplateBitmaskBuffers buffers;
    FA18HunkSegment segments[67] = {{0}};
    FA18Hunks hunks = {segments, 67};

    initialize_stream(first, sizeof first);
    initialize_stream(second, sizeof second);
    initialize_stream(third, sizeof third);
    put16(first, 0x100); put16(first + 0x100, 4);
    put16(first + 0x102, 0); put16(first + 0x104, 63);
    put16(second + 2, 0x100); put16(second + 0x100, 2);
    put16(second + 0x102, 32);
    assert(fa18_build_template_bitmask_buffers(first, sizeof first,
                                                second, sizeof second,
                                                third, sizeof third, &buffers) == 0);
    assert(buffers.first[3] == 0x01 && buffers.first[4] == 0x80 &&
           buffers.second[23] == 0x01 && !buffers.third[0]);
    first[0] = 0;
    assert(fa18_build_template_bitmask_buffers(first, sizeof first,
                                                second, sizeof second,
                                                third, sizeof third, &buffers) == 0x43);
    memcpy(hunk, first, sizeof first);
    memcpy(hunk + 0x100, second, sizeof second);
    memcpy(hunk + 0x200, third, sizeof third);
    segments[66] = (FA18HunkSegment){.kind=FA18_HUNK_CODE,.data=hunk,.size=sizeof hunk};
    assert(fa18_initialize_template_bitmask_buffers(&hunks, &buffers) == 0x43);
    {
        uint16_t error=0xbeef;
        FA18TemplateBitmaskState bound={.buffers=&buffers,.error_word=&error};
        buffers.first[0]=0xa5;
        assert(fa18_bind_template_bitmask_streams(&bound,&hunks));
        assert(buffers.first[0]==0xa5 && error==0xbeef);
        assert(bound.streams[0]==hunk && bound.streams[1]==hunk+256 && bound.streams[2]==hunk+512);
        hunk[0]=0x80;
        assert(fa18_run_template_bitmask_state(&bound) && error==0x43 && !buffers.first[0]);
    }
    {
        uint16_t error=0xabcd; uint32_t neighbour=0x12345678;
        PortFieldByte before[4];
        FA18TemplateBitmaskState state={.streams={first,second,third},
            .stream_sizes={sizeof first,sizeof second,sizeof third},.buffers=&buffers,.error_word=&error};
        unsigned i;
        for(i=0;i<4;++i) before[i]=(PortFieldByte){.longword=&neighbour,.shift=24-8*i};
        initialize_stream(first,sizeof first); initialize_stream(second,sizeof second); initialize_stream(third,sizeof third);
        /* Odd source lengths truncate; signed bit -1 writes the previous row. */
        for(i=0;i<128;++i) put16(first+2*i,0x106);
        put16(first+0x106,0xffff); put16(first+2,0x100);
        put16(first+0x100,3); put16(first+0x102,0xffff);
        assert(fa18_run_template_bitmask_state(&state) && error==0xabcd);
        assert(buffers.first[12]==0x80 && !buffers.first[16]);
        put16(first,0x100);
        assert(!fa18_run_template_bitmask_state(&state) && error==0xabcd);
        state.before=before; state.before_count=4;
        assert(fa18_run_template_bitmask_state(&state) && neighbour==0x92345678);
        /* A source fault completes normally and retains earlier axis writes. */
        put16(first+0x102,31); put16(second,0);
        assert(fa18_run_template_bitmask_state(&state) && error==0x44 && buffers.first[0]==0x80);
        state.streams[0]=NULL; error=0xbeef;
        assert(!fa18_run_template_bitmask_state(&state) && error==0xbeef && !buffers.first[0]);
    }
    {
        static uint8_t zero_stream[0x104+2*65536];
        uint16_t error=0xbeef;
        FA18TemplateBitmaskState state={.streams={zero_stream,second,third},
            .stream_sizes={sizeof zero_stream,sizeof second,sizeof third},.buffers=&buffers,.error_word=&error};
        unsigned i;
        initialize_stream(second,sizeof second); initialize_stream(third,sizeof third);
        for(i=0;i<128;++i) put16(zero_stream+2*i,0x100);
        put16(zero_stream+0x100,0xffff); put16(zero_stream,0x102);
        put16(zero_stream+0x102,0);
        assert(fa18_run_template_bitmask_state(&state) && buffers.first[3]==1 && error==0xbeef);
        put16(zero_stream+0x102,1);
        assert(fa18_run_template_bitmask_state(&state) && buffers.first[3]==1);
        state.stream_sizes[0]-=2;
        assert(!fa18_run_template_bitmask_state(&state) && buffers.first[3]==1 && error==0xbeef);
    }
    return 0;
}
