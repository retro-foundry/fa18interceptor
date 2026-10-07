/* Original opcodes/chipset are validation-only. Compile the exact host
 * raster branch against the oracle memory backend, under distinct names. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "m68kcpu.h"
#include "m68kops.h"
#include "machine.h"
#include "bus.h"
#include "recomp_runtime.h"
#include "recomp_ports.h"
#include "memory.h"
#include "globals.h"
extern int64_t fa18_next_event;
#define FA18_NATIVE
#define setup_line host_setup_line
#define draw_line host_draw_line
#define draw_line_to_row host_draw_line_to_row
#define draw_line_to_row_result host_draw_line_to_row_result
#define reset_line_style host_reset_line_style
#define draw_projected_segment host_draw_projected_segment
#define draw_clipped_segment host_draw_clipped_segment
#define composite_polygon_plane host_composite_polygon_plane
#define draw_polygon_edge host_draw_polygon_edge
#define clear_polygon_mask host_clear_polygon_mask
#define prepare_polygon host_prepare_polygon
#define prepare_polygon_to_row host_prepare_polygon_to_row
#define draw_polygon host_draw_polygon
#define scale_mark_polygon host_scale_mark_polygon
#define draw_mark_polygon host_draw_mark_polygon
#define clear_render_buffers host_clear_render_buffers
#define blit_mask_between_planes host_blit_mask_between_planes
#define clear_page_plane_tops host_clear_page_plane_tops
#define blit_lane host_blit_lane
#define submit_active_planes host_submit_active_planes
#define clip_crossing host_clip_crossing
#define clip_stage host_clip_stage
#define clip_and_draw_polygon host_clip_and_draw_polygon
#define prepare_map_packet_depth host_prepare_map_packet_depth
#define run_map_packet_pass host_run_map_packet_pass
#define native_storage_range oracle_storage_range
static uint8_t *oracle_storage_range(uint32_t a,size_t n) {
    if(a<0xc00000u || a+n>0xc80000u) abort();
    return fa18_machine->slow+a-0xc00000u;
}
#include "../../port/game/native/raster.c"
#include "../../port/game/render_line.c"
#include "../../port/game/render_buffers.c"
#include "../../port/game/render_polygon.c"
#include "../../port/game/active_planes.c"
#include "../../port/game/polygon_clip.c"
#include "../../port/game/map_packet.c"
#undef FA18_NATIVE
#undef setup_line
#undef draw_line
#undef draw_line_to_row
#undef draw_line_to_row_result
#undef reset_line_style
#undef draw_projected_segment
#undef draw_clipped_segment
#undef composite_polygon_plane
#undef draw_polygon_edge
#undef clear_polygon_mask
#undef prepare_polygon
#undef prepare_polygon_to_row
#undef draw_polygon
#undef scale_mark_polygon
#undef draw_mark_polygon
#undef clear_render_buffers
#undef blit_mask_between_planes
#undef clear_page_plane_tops
#undef blit_lane
#undef submit_active_planes
#undef clip_crossing
#undef clip_stage
#undef clip_and_draw_polygon
#undef prepare_map_packet_depth
#undef run_map_packet_pass
static uint8_t *file_bytes(const char *name,size_t *size) {
    FILE *f=fopen(name,"rb"); long n; uint8_t *p;
    if(!f || fseek(f,0,SEEK_END) || (n=ftell(f))<0 || fseek(f,0,SEEK_SET)) return NULL;
    p=malloc((size_t)n); if(!p || fread(p,1,(size_t)n,f)!=(size_t)n || fclose(f)) return NULL;
    *size=(size_t)n; return p;
}
static int original_arguments(uint32_t pc,const int16_t *points) {
    memset(REG_DA,0,sizeof REG_DA); REG_A[4]=rd_u16(LINE_LAST_ROW); REG_A[7]=0xc7ff00u; wr_u32(REG_A[7],0xc70000u);
    if(points) {
        for(unsigned i=0;i<4;++i) REG_D[i]=(uint16_t)points[i];
        REG_D[4]=0x51ab12e7; /* Independent input on no-assignment exits. */
    }
    m68k_set_reg(M68K_REG_SR,0x2700); REG_PC=pc;
    fa18_next_event=INT64_MAX; SET_CYCLES(100000000);
    for(unsigned step=0;step<2000000;++step) {
        uint16_t opcode;
        if(REG_PC==0xc70000u && REG_A[7]==0xc7ff04u) { wait_blitter(); return 1; }
        int cycles_before=GET_CYCLES();
        opcode=rd_u16(REG_PC); REG_PPC=REG_PC; REG_IR=opcode; REG_PC+=2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
        fa18_machine->cycle+=cycles_before-GET_CYCLES();
    }
    fprintf(stderr,"original renderer did not return at %06X\n",REG_PC); return 0;
}
static int original(uint32_t pc) { return original_arguments(pc,NULL); }
static int compare(const uint8_t *expected,unsigned test) {
    unsigned count=0;
    for(unsigned i=0;i<0x80000;++i) if(fa18_machine->chip[i]!=expected[i]) {
        if(count<8) fprintf(stderr,"case %u plane byte %06X: source %02X native %02X\n",test,i,expected[i],fa18_machine->chip[i]);
        ++count;
    }
    if(count) { fprintf(stderr,"%u plane/buffer differences\n",count); return 0; }
    return 1;
}
static uint32_t random_state=0x18fa;
static unsigned random_value(unsigned limit) {
    random_state=random_state*1664525u+1013904223u;
    return (random_state>>8)%limit;
}
static int map_polygon(void *context,gaddr end,int16_t origin) {
    (void)context; (void)end; (void)origin;
    host_clip_and_draw_polygon(); return 0;
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0; char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state || !rom || !data || nd!=0x100000 || !m || !before || !expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) { fputs(error,stderr); return 1; }
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    memcpy(m->chip,data,0x80000); memcpy(m->slow,data+0x80000,0x80000);
    static const int16_t line_points[][4]={
        {10,20,29,20},{29,20,10,20},{20,30,250,55},{250,30,20,55},
        {100,10,100,170},{130,170,100,10},{20,220,40,230},{40,230,20,220},
        {10,220,29,220},{20,170,300,240},{300,240,20,170},{10,30,35,240},
        {35,240,10,30},{10,20,10,20},{0,0,319,0},{10,199,20,199}
    };
    unsigned line_kinds[3]={0};
    for(unsigned test=0;test<128;++test) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        const int16_t *points=line_points[test%16];
        const int fixed_row=(test&64)!=0;
        const int16_t last_row=fixed_row?199:(test&32?179:199);
        wr_u16(LINE_LAST_ROW,(uint16_t)last_row);
        wr_u8(LINE_PLANES,(uint8_t[]){0,1,5,15}[(test>>4)&3]);
        wr_s16(LINE_COLOUR,test&8?-1:9);wr_u16(CURRENT_COLOUR,6);
        memcpy(before,m,sizeof *m);
        const LineDrawResult result=host_draw_line_to_row_result(points[0],points[1],points[2],points[3],last_row);
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);
        if(!original_arguments(fixed_row?0xc2fa78:0xc2fa7e,points)) return 1;
        uint32_t returned=result.kind==LINE_DRAW_SIZE?result.size:
            result.kind==LINE_DRAW_X_DELTA?(uint16_t)result.x_delta:0x51ab12e7;
        const uint32_t actual=result.kind==LINE_DRAW_NONE?REG_D[4]:(uint16_t)REG_D[4];
        if(returned!=actual) {
            fprintf(stderr,"Line return case %u kind %u: source %08X native %08X\n",test,result.kind,actual,returned);return 1;
        }
        if(!compare(expected,test)) return 1;
        if(memcmp(m->slow,expected+0x80000,0x7fc00)) {fprintf(stderr,"Line case %u changed non-stack slow RAM\n",test);return 1;}
        ++line_kinds[result.kind];
    }
    if(!line_kinds[0] || !line_kinds[1] || !line_kinds[2]) {fputs("Line return fixtures missed an exit kind\n",stderr);return 1;}
    printf("128 original line returns and non-stack RAM match: %u preserve input, %u X deltas, %u blit sizes\n",
        line_kinds[0],line_kinds[1],line_kinds[2]);
    memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
    for(unsigned test=0;test<160;++test) {
        int16_t x=(int16_t)random_value(270),y=(int16_t)random_value(140);
        int16_t width=(int16_t)(test<20?test%5:2+random_value(46));
        int16_t height=(int16_t)(test<20?test/5:2+random_value(38));
        for(unsigned i=0;i<4*10240;++i) m->chip[0x10000+i]=(uint8_t)random_value(256);
        memset(m->chip+0x30000,0,10240);
        wr_u16(POLY_VERTICES,4);
        wr_s16(POLY_VERTICES+2,x); wr_s16(POLY_VERTICES+4,y);
        wr_s16(POLY_VERTICES+6,x+width); wr_s16(POLY_VERTICES+8,y+height/3);
        wr_s16(POLY_VERTICES+10,x+width/2); wr_s16(POLY_VERTICES+12,y+height);
        wr_s16(POLY_VERTICES+14,x); wr_s16(POLY_VERTICES+16,y+height/2);
        wr_u16(LINE_LAST_ROW,test%3?179:(uint16_t)(y+height/2));
        wr_u8(LINE_PLANES,(uint8_t)(1+random_value(15)));
        wr_s16(LINE_COLOUR,test%2?(int16_t)random_value(16):-1);
        wr_u16(CURRENT_COLOUR,(uint16_t)random_value(16));
        wr_u16(POLY_COMPLEMENT,(uint16_t)random_value(16));
        wr_u16(POLY_MASK_BLIT,test%11==0);
        memcpy(before,m,sizeof *m);
        if(!original(0xc2ff48u)) return 1;
        memcpy(expected,m->chip,0x80000);
        memcpy(m,before,sizeof *m); host_draw_polygon();
        if(!compare(expected,test)) {
            fprintf(stderr,"x=%d y=%d width=%d height=%d planes=%x colour=%x current=%x complement=%x xor=%x\n",x,y,width,height,rd_u8(LINE_PLANES),rd_u16(LINE_COLOUR),rd_u16(CURRENT_COLOUR),rd_u16(POLY_COMPLEMENT),rd_u8(POINT_XOR_PLANES));
            return 1;
        }
    }
    memcpy(m->chip,data,0x80000); memcpy(m->slow,data+0x80000,0x80000);
    memcpy(before,m,sizeof *m);
    if(!original(0xc2fd8cu)) return 1;
    memcpy(expected,m->chip,0x80000); memcpy(m,before,sizeof *m);
    host_submit_active_planes(NULL);
    if(!compare(expected,160)) return 1;
    memcpy(before,m,sizeof *m);
    if(!original(0xc2aa9cu)) return 1;
    memcpy(expected,m->chip,0x80000); memcpy(m,before,sizeof *m);
    FA18MapPacketDepthStageResult depth=host_prepare_map_packet_depth(0x4000);
    const MapPacketHooks hooks={.host_draw_polygon=map_polygon};
    for(int wide=0;wide<2;++wide) {
        if(!wide && !depth.run_normal_pass) continue;
        wr_u16(CURRENT_COLOUR,6);
        if(host_run_map_packet_pass(0x4000,wide,&hooks)) return 1;
    }
    if(!compare(expected,161)) return 1;
    puts("160 native polygons plus source horizon and complete map passes match original plane buffers");
    return 0;
}
