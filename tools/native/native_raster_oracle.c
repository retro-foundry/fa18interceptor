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
#define draw_clipped_segment_result host_draw_clipped_segment_result
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
#define clip_and_draw_polygon_retained host_clip_and_draw_polygon_retained
#define prepare_map_packet_depth host_prepare_map_packet_depth
#define run_map_packet_pass host_run_map_packet_pass
#define native_storage_range oracle_storage_range
static uint8_t *oracle_storage_range(uint32_t a,size_t n) {
    if(a<0xc00000u || a+n>0xc80000u) abort();
    return fa18_machine->slow+a-0xc00000u;
}
#include "../../../fa18-interceptor-decomp/port/game/native/raster.c"
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
#undef draw_clipped_segment_result
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
#undef clip_and_draw_polygon_retained
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
static int compare_pages(const uint8_t *expected) {
    unsigned count=0;
    for(unsigned plane=0;plane<8;++plane) {
        const gaddr start=rd_u32(0xc4566eu+4*plane);
        if(start>=0x80000u || start+8000>0x80000u) return 0;
        for(unsigned i=0;i<8000;++i) if(fa18_machine->chip[start+i]!=expected[start+i]) {
            if(count<8) fprintf(stderr,"page plane %u byte %u x%u/y%u: source %02X native %02X\n",
                plane,i,i%40*8,i/40,expected[start+i],fa18_machine->chip[start+i]);
            ++count;
        }
    }
    if(count) fprintf(stderr,"%u complete-page byte differences\n",count);
    return count==0;
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
    const int active_only=argc==3 && !strcmp(argv[2],"--active-planes");
    const int active_map=argc==3 && !strcmp(argv[2],"--active-map");
    const int negative_lines=argc==3 && !strcmp(argv[2],"--negative-lines");
    uint8_t *data=(argc==2 || active_only || active_map || negative_lines)?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state || !rom || !data || nd!=0x100000 || !m || !before || !expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) { fputs(error,stderr); return 1; }
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    memcpy(m->chip,data,0x80000); memcpy(m->slow,data+0x80000,0x80000);
    if(negative_lines) {
        static const int16_t points[][4]={
            {-32,84,-1,85},{-1,85,-32,84},{-24,85,0,85},{-32,85,-32,90},
            {-3,84,17,86},{17,86,-3,84},{-1,0,319,1},{319,1,-1,0}};
        const unsigned point_count=sizeof points/sizeof points[0];
        for(unsigned test=0;test<4*point_count;++test) {
            const int16_t *point=points[test%point_count];
            memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
            wr_u16(LINE_LAST_ROW,144);wr_u8(LINE_PLANES,(uint8_t[]){1,5,8,15}[test/point_count]);
            wr_s16(LINE_COLOUR,-1);wr_u16(CURRENT_COLOUR,7);
            memcpy(before,m,sizeof *m);
            host_draw_line(point[0],point[1],point[2],point[3]);
            memcpy(expected,m->chip,0x80000);memcpy(m,before,sizeof *m);
            if(!original_arguments(0xc2fa7e,point) || !compare(expected,test)) return 1;
        }
        puts("32 negative-X source lines match complete buffers");return 0;
    }
    if(active_only || active_map) {
        memcpy(before,m,sizeof *m);
        if(!original(0xc2fd8cu)) return 1;
        if(active_map && !original(0xc2aa9cu)) return 1;
        memcpy(expected,m->chip,0x80000);memcpy(m,before,sizeof *m);
        host_submit_active_planes(NULL);
        if(active_map) {
            FA18MapPacketDepthStageResult depth=host_prepare_map_packet_depth(0x4000);
            const MapPacketHooks hooks={.host_draw_polygon=map_polygon};
            for(int wide=0;wide<2;++wide) {
                if(!wide && !depth.run_normal_pass) continue;
                wr_u16(CURRENT_COLOUR,6);
                if(host_run_map_packet_pass(0x4000,wide,&hooks)) return 1;
            }
        }
        if(!compare_pages(expected)) return 1;
        printf("Captured horizon%s matches original complete plane buffers\n",active_map?" and map":"");
        return 0;
    }
    unsigned retained_returns=0,retained_coordinates=0,retained_unchanged=0;
    for(unsigned test=0;test<256;++test) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        wr_u16(CLIP_INPUT,0);wr_u16(CLIP_INPUT+2,3+test%4);
        wr_u16(LINE_LAST_ROW,179);wr_u8(LINE_PLANES,15);wr_u16(CURRENT_COLOUR,6);
        for(unsigned vertex=0;vertex<3+test%4;++vertex) {
            wr_s16(CLIP_INPUT+4+6*vertex,(int16_t)(random_value(601)-300));
            wr_s16(CLIP_INPUT+6+6*vertex,(int16_t)(random_value(601)-300));
            wr_s16(CLIP_INPUT+8+6*vertex,(int16_t)(50+random_value(201)));
        }
        /* At this independent C246A0 call's frame C7FEFC, -$1E has the
         * same overlap as the map parent's later model accumulator. */
        const uint16_t incoming=(uint16_t)(0x5100+test);
        wr_u16(0xc7fede,incoming);memcpy(before,m,sizeof *m);
        uint16_t carry=incoming;host_clip_and_draw_polygon_retained(&carry);
        memcpy(expected,m->chip,0x80000);memcpy(m,before,sizeof *m);
        /* Run the reference last, as in the line/segment cases below. Its
         * completed hardware clock must not be rewound by a machine copy. */
        if(!original(0xc246a0)) {
            fprintf(stderr,"Retained clipping case %u cycle=%llu DMA=%04X blits=%llu\n",test,
                (unsigned long long)m->cycle,m->dmacon,(unsigned long long)m->blits);
            return 1;
        }
        const uint16_t retained=rd_u16(0xc7fede);
        if(carry!=retained || !compare(expected,test)) {
            fprintf(stderr,"Map clipping retained case %u: source %04X native %04X\n",test,retained,carry);
            return 1;
        }
        if(retained==incoming) ++retained_unchanged;
        else if(retained==(0xc24974u>>16)) ++retained_returns;
        else ++retained_coordinates;
    }
    if(!retained_returns || !retained_coordinates || !retained_unchanged) {
        fputs("Map clipping carry fixtures missed a writer kind\n",stderr);return 1;
    }
    printf("256 map clipping retained words and drawing match: %u returns, %u coordinates, %u unchanged\n",
        retained_returns,retained_coordinates,retained_unchanged);
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
    static const int16_t segments[][6]={
        {-100,0,512,100,0,512},{0,-100,512,0,100,512},
        {-100,-100,512,100,100,512},{100,100,512,-100,-100,512},
        {1000,80,512,0,0,512},{-1000,-80,512,0,0,512},
        {80,1000,512,0,0,512},{-80,-1000,512,0,0,512},
        {0,0,512,1000,80,512},{0,0,512,-1000,-80,512},
        {0,0,512,80,1000,512},{0,0,512,-80,-1000,512},
        {1000,80,512,1200,100,512},{-1000,-80,512,-1200,-100,512},
        {80,1000,512,100,1200,512},{0,0,-512,100,80,-256}
    };
    unsigned segment_kinds[3]={0};
    for(unsigned test=0;test<128;++test) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        for(unsigned i=0;i<6;++i) wr_s16(SEGMENT_POINTS+2*i,segments[test%16][i]);
        wr_u16(LINE_LAST_ROW,test&64?0:179);
        wr_u8(LINE_PLANES,(uint8_t[]){0,1,5,15}[(test>>4)&3]);
        wr_s16(LINE_COLOUR,-1);wr_u16(CURRENT_COLOUR,6);
        memcpy(before,m,sizeof *m);
        const SegmentDrawResult result=host_draw_clipped_segment_result();
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);
        if(!original(0xc2ee4a)) return 1;
        const uint16_t returned=result.kind==SEGMENT_DRAW_LINE?
            result.line.kind==LINE_DRAW_SIZE?result.line.size:(uint16_t)result.line.x_delta:
            (uint16_t)result.y;
        if(returned!=(uint16_t)REG_D[4] || result.drawn!=(REG_D[0]==1)) {
            fprintf(stderr,"Segment return case %u kind %u: source %04X/%u native %04X/%u\n",
                test,result.kind,(uint16_t)REG_D[4],REG_D[0],returned,result.drawn);return 1;
        }
        if(!compare(expected,test)) return 1;
        if(memcmp(m->slow,expected+0x80000,0x7fc00)) {
            fprintf(stderr,"Segment case %u changed non-stack slow RAM\n",test);return 1;
        }
        ++segment_kinds[result.kind];
    }
    if(!segment_kinds[0] || !segment_kinds[1] || !segment_kinds[2]) {
        fputs("Segment return fixtures missed an exit kind\n",stderr);return 1;
    }
    printf("128 original clipped-segment returns and non-stack RAM match: %u endpoint heights, %u screen heights, %u line results\n",
        segment_kinds[0],segment_kinds[1],segment_kinds[2]);
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
