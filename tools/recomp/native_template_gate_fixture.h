/* Validation-only source-format fixtures and native owner import/export.
 * No expected gate output is calculated here. */
#include "../../port/template_bitmask_buffers.c"
#include "../../port/disk.h"
#include "../../port/romfree/placement.h"
enum { GATE_NEIGHBOURS=4096, GATE_STREAM_BYTES=0x20400 };
typedef struct {
    FA18TemplateBitmaskBuffers buffers;
    FA18TemplateBitmaskState state;
    uint8_t before_bytes[GATE_NEIGHBOURS],after_bytes[GATE_NEIGHBOURS],streams[GATE_STREAM_BYTES];
    PortFieldByte before[GATE_NEIGHBOURS],after[GATE_NEIGHBOURS];
    uint16_t error;
    unsigned stream_changed;
} GateOracleState;
static uint8_t gate_bit_seen[65536];
static unsigned gate_hunk_bytes;
static FA18TemplateBitmaskBuffers gate_asset_buffers;
static void gate_load(GateOracleState *s) {
    unsigned i,axis;
    memset(s,0,sizeof *s);
    for(i=0;i<GATE_NEIGHBOURS;++i) {
        s->before_bytes[i]=rd_u8(TEMPLATE_GATES_X-GATE_NEIGHBOURS+i);
        s->after_bytes[i]=rd_u8(TEMPLATE_GATES_Z+2048+i);
        s->before[i]=(PortFieldByte){.byte=s->before_bytes+i};
        s->after[i]=(PortFieldByte){.byte=s->after_bytes+i};
    }
    for(axis=0;axis<3;++axis) for(i=0;i<2048;++i)
        gate_axis_bytes(&s->buffers,axis)[i]=rd_u8(TEMPLATE_GATES_X+axis*2048+i);
    for(i=0;i<GATE_STREAM_BYTES;++i) s->streams[i]=rd_u8(TEMPLATE_SELECTOR_X+i);
    s->error=rd_u16(ERROR_CODE);
    s->state=(FA18TemplateBitmaskState){.buffers=&s->buffers,.before=s->before,.before_count=GATE_NEIGHBOURS,
        .after=s->after,.after_count=GATE_NEIGHBOURS,.error_word=&s->error};
    for(axis=0;axis<3;++axis) {
        s->state.streams[axis]=s->streams+axis*256;
        s->state.stream_sizes[axis]=GATE_STREAM_BYTES-axis*256;
    }
}
static void gate_store(const GateOracleState *s) {
    unsigned i,axis;
    for(i=0;i<GATE_NEIGHBOURS;++i) {
        uint8_t value;
        if(!port_read_field_byte(s->before+i,&value)) abort();
        wr_u8(TEMPLATE_GATES_X-GATE_NEIGHBOURS+i,value);
        if(!port_read_field_byte(s->after+i,&value)) abort();
        wr_u8(TEMPLATE_GATES_Z+2048+i,value);
    }
    for(axis=0;axis<3;++axis) for(i=0;i<2048;++i) {
        const uint8_t *bytes=axis==0?s->buffers.first:axis==1?s->buffers.second:s->buffers.third;
        wr_u8(TEMPLATE_GATES_X+axis*2048+i,bytes[i]);
    }
    wr_u16(ERROR_CODE,s->error);
    if(s->stream_changed) {
        wr_u8(TEMPLATE_SELECTOR_X+0x300,s->streams[0x300]);
        wr_u8(TEMPLATE_SELECTOR_X+0x301,s->streams[0x301]);
    }
}
static void gate_fixture(unsigned scenario,int standalone) {
    unsigned i,axis,row; uint32_t cursor=TEMPLATE_SELECTOR_X+0x302;
    for(i=0;i<3*2048+2*GATE_NEIGHBOURS;++i)
        wr_u8(TEMPLATE_GATES_X-GATE_NEIGHBOURS+i,(uint8_t)random_value());
    if(standalone && !scenario) return; /* Actual original Hunk-66 data. */
    wr_u16(TEMPLATE_SELECTOR_X+0x300,0xffff);
    for(axis=0;axis<3;++axis) for(row=0;row<128;++row) {
        uint32_t directory=TEMPLATE_SELECTOR_X+256*axis;
        if(standalone && scenario>=1 && scenario<=3) {
            wr_u16(directory+2*row,(uint16_t)(TEMPLATE_SELECTOR_X+0x300-directory));
            continue;
        }
        if((row+scenario+axis)%4==0) {
            wr_u16(directory+2*row,(uint16_t)(TEMPLATE_SELECTOR_X+0x300-directory));
        } else {
            unsigned length=2+(scenario+row+axis)%14,count=length/2;
            wr_u16(directory+2*row,(uint16_t)(cursor-directory));
            wr_u16(cursor,(uint16_t)length); cursor+=2;
            for(i=0;i<count;++i,cursor+=2) wr_u16(cursor,(uint16_t)random_value());
        }
    }
    if(standalone && scenario>=1 && scenario<=3) {
        unsigned selected=scenario-1,count=scenario==3?16383:65536;
        uint32_t directory=TEMPLATE_SELECTOR_X+256*selected;
        wr_u16(directory+2*(scenario==3?127:0),(uint16_t)(TEMPLATE_SELECTOR_X+0x302-directory));
        wr_u16(TEMPLATE_SELECTOR_X+0x302,(uint16_t)(scenario==3?0x7fff:scenario-1));
        for(i=0;i<count;++i) wr_u16(TEMPLATE_SELECTOR_X+0x304+2*i,(uint16_t)(scenario==2?65535-i:i));
    } else if(scenario%8==5) {
        static const uint16_t invalid[]={0,0x8000,0xffff};
        axis=(scenario/8)%3; row=(scenario*17)%128;
        wr_u16(TEMPLATE_SELECTOR_X+axis*256+2*row,invalid[(scenario/24)%3]);
    }
}
static int gate_assets(void) {
    FA18Disk disk; FA18Hunks hunks={0}; size_t length; uint8_t *image; unsigned i;
    FA18TemplateBitmaskBuffers buffers; uint16_t error=0xbeef;
    FA18TemplateBitmaskState state={.buffers=&buffers,.error_word=&error};
    if(!fa18_disk_open(&disk,"FA-18 Interceptor (1988)(Electronic Arts)[cr A-Ha].adf")) { fprintf(stderr,"gate disk open failed\n"); return 0; }
    image=fa18_disk_read(&disk,"F-18 Interceptor",&length);
    if(!image || !fa18_hunks_load(&hunks,image,length) || !fa18_bind_template_bitmask_streams(&state,&hunks)) { fprintf(stderr,"gate file/Hunk bind failed\n"); return 0; }
    for(i=0;i<hunks.segments[66].size;++i) {
        unsigned j; uint8_t expected=hunks.segments[66].data[i];
        for(j=0;j<hunks.segments[66].reloc_count;++j) {
            const FA18HunkReloc *reloc=hunks.segments[66].relocs+j;
            if(i>=reloc->offset && i<reloc->offset+4) {
                uint32_t value=fa18_be32(hunks.segments[66].data+reloc->offset)+fa18_placements[reloc->target].payload_base;
                expected=(uint8_t)(value>>(24-8*(i-reloc->offset)));
            }
        }
        if(expected!=rd_u8(TEMPLATE_SELECTOR_X+i)) { fprintf(stderr,"gate relocated Hunk byte %u disk %02X source %02X\n",i,expected,rd_u8(TEMPLATE_SELECTOR_X+i)); return 0; }
    }
    for(i=0;i<3;++i) if(state.streams[i]!=hunks.segments[66].data+256*i) return 0;
    if(!fa18_run_template_bitmask_state(&state) || error!=0xbeef) { fprintf(stderr,"gate original asset run failed error %04X\n",error); return 0; }
    gate_asset_buffers=buffers;
    gate_hunk_bytes=hunks.segments[66].size;
    fa18_hunks_free(&hunks); free(image); fa18_disk_close(&disk); return 1;
}
