#include "template_bitmask_buffers.h"

#include <string.h>

static int read_gate_source_word(const uint8_t *bytes,size_t count,size_t offset,uint16_t *value) {
    if(!bytes || offset>count || count-offset<2) return 0;
    *value=(uint16_t)(((unsigned)bytes[offset]<<8)|bytes[offset+1]); return 1;
}
static uint8_t *gate_axis_bytes(FA18TemplateBitmaskBuffers *b,unsigned axis) {
    return axis==0?b->first:axis==1?b->second:b->third;
}
/* A bounded view of this domain's adjacent fields, not an address space. */
static int gate_field(FA18TemplateBitmaskState *s,int offset,PortFieldByte *field) {
    if(offset<0) {
        size_t distance=(size_t)(-offset);
        if(!s->before || distance>s->before_count) return 0;
        *field=s->before[s->before_count-distance];
    } else if(offset>=3*FA18_TEMPLATE_BITMASK_BYTES) {
        size_t index=(size_t)(offset-3*FA18_TEMPLATE_BITMASK_BYTES);
        if(!s->after || index>=s->after_count) return 0;
        *field=s->after[index];
    } else {
        *field=(PortFieldByte){.byte=gate_axis_bytes(s->buffers,(unsigned)offset/FA18_TEMPLATE_BITMASK_BYTES)+
            (unsigned)offset%FA18_TEMPLATE_BITMASK_BYTES};
    }
    return port_field_byte_valid(field);
}
static int set_gate_bit(FA18TemplateBitmaskState *s,unsigned axis,unsigned row,uint16_t bit) {
    int signed_bit=bit<0x8000u?(int)bit:(int)bit-0x10000;
    int word=signed_bit>=0?signed_bit/32:-((-signed_bit+31)/32);
    int offset=(int)(axis*FA18_TEMPLATE_BITMASK_BYTES+row*FA18_TEMPLATE_BITMASK_ROW_BYTES)+4*word;
    PortFieldByte fields[4]; uint32_t value=0; uint8_t byte; unsigned i;
    for(i=0;i<4;++i) {
        if(!gate_field(s,offset+(int)i,fields+i) || !port_read_field_byte(fields+i,&byte)) return 0;
        value=(value<<8)|byte;
    }
    value|=UINT32_C(1)<<(bit&31u);
    for(i=0;i<4;++i) if(!port_write_field_byte(fields+i,(uint8_t)(value>>(24-8*i)))) return 0;
    return 1;
}
int fa18_run_template_bitmask_state(FA18TemplateBitmaskState *s) {
    unsigned axis,row,block;
    if(!s || !s->buffers) return 0;
    /* Preserve the three interleaved original 32-byte clear stores. */
    for(block=0;block<64;++block) for(axis=0;axis<3;++axis)
        memset(gate_axis_bytes(s->buffers,axis)+32*block,0,32);
    for(axis=0;axis<3;++axis) for(row=0;row<128;++row) {
        uint16_t relative,length,bit; size_t cursor; unsigned count,i;
        if(!read_gate_source_word(s->streams[axis],s->stream_sizes[axis],2*row,&relative)) return 0;
        if(!relative || relative>=0x8000u) {
            if(!s->error_word) return 0;
            *s->error_word=(uint16_t)(0x43+axis);
            /* Original release-build C06C02 is an actual RTS. */
            return 1;
        }
        if(!read_gate_source_word(s->streams[axis],s->stream_sizes[axis],relative,&length)) return 0;
        if(length>=0x8000u) continue;
        count=length/2u;
        if(!count) count=65536;
        cursor=(size_t)relative+2;
        for(i=0;i<count;++i,cursor+=2) {
            if(!read_gate_source_word(s->streams[axis],s->stream_sizes[axis],cursor,&bit) ||
               !set_gate_bit(s,axis,row,bit)) return 0;
        }
    }
    return 1;
}

int fa18_build_template_bitmask_buffers(const uint8_t *first_stream,
                                        size_t first_size,
                                        const uint8_t *second_stream,
                                        size_t second_size,
                                        const uint8_t *third_stream,
                                        size_t third_size,
                                        FA18TemplateBitmaskBuffers *buffers) {
    uint16_t error=0;
    FA18TemplateBitmaskState state={.streams={first_stream,second_stream,third_stream},
        .stream_sizes={first_size,second_size,third_size},.buffers=buffers,.error_word=&error};
    return fa18_run_template_bitmask_state(&state)?error:-1;
}

int fa18_bind_template_bitmask_streams(FA18TemplateBitmaskState *state,const FA18Hunks *hunks) {
    const FA18HunkSegment *segment;
    unsigned axis;
    if (!state || !hunks || !hunks->segments || hunks->count <= FA18_TEMPLATE_BITMASK_HUNK) return 0;
    segment = &hunks->segments[FA18_TEMPLATE_BITMASK_HUNK];
    if (!segment->data || segment->size < FA18_TEMPLATE_BITMASK_STREAM_C_OFFSET+256) return 0;
    for(axis=0;axis<3;++axis) {
        state->streams[axis]=segment->data+axis*256;
        state->stream_sizes[axis]=segment->size-axis*256;
    }
    return 1;
}
int fa18_initialize_template_bitmask_buffers(const FA18Hunks *hunks,
                                             FA18TemplateBitmaskBuffers *buffers) {
    uint16_t error=0;
    FA18TemplateBitmaskState state={.buffers=buffers,.error_word=&error};
    if(!fa18_bind_template_bitmask_streams(&state,hunks)) return -1;
    return fa18_run_template_bitmask_state(&state)?error:-1;
}
