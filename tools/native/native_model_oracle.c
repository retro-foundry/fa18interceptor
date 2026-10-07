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
#include "followup_placements.h"
#include "main_loop_control_messages.h"
#include "display_records.h"
extern int64_t fa18_next_event;
#define FA18_NATIVE
#define setup_line host_setup_line
#define draw_line host_draw_line
#define draw_line_to_row host_draw_line_to_row
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
#define draw_filled_circle host_draw_filled_circle
#include "../../port/game/circle.c"
#define project_view_point host_project_view_point
#define project_view_point_mode host_project_view_point_mode
#define draw_display_stream_point host_draw_display_stream_point
#define draw_fixed_matrix_mark host_draw_fixed_matrix_mark
#define draw_scaled_view_circle host_draw_scaled_view_circle
#define draw_scaled_stream_circle host_draw_scaled_stream_circle
#define draw_shape host_draw_shape
#define shift_word projection_shift_word
#include "../../port/game/projection.c"
#undef shift_word
#define begin_history_projection host_begin_history_projection
#define prepare_history_projection_point host_prepare_history_projection_point
#define draw_history_projection host_draw_history_projection
#include "../../port/game/history_projection.c"
#define draw_grid_projection_packet host_draw_grid_projection_packet
#include "../../port/game/grid_projection_packet.c"
#undef draw_filled_circle
#define draw_selected_segment host_draw_selected_segment
#define draw_selected_segment_clipped host_draw_selected_segment_clipped
#define draw_selected_segment_near host_draw_selected_segment_near
#define draw_segment_pairs host_draw_segment_pairs
#define draw_segment_pairs_near host_draw_segment_pairs_near
#define draw_segment_run host_draw_segment_run
#define draw_offset_segments host_draw_offset_segments
#define draw_parallelogram_face host_draw_parallelogram_face
#define draw_parallelogram_face_2 host_draw_parallelogram_face_2
#define draw_parallelogram_face_near host_draw_parallelogram_face_near
#define draw_offset_face host_draw_offset_face
#define draw_mixed_face host_draw_mixed_face
#define extend_parallelograms host_extend_parallelograms
#define extend_parallelograms_scaled host_extend_parallelograms_scaled
#define draw_segment_grid host_draw_segment_grid
#define draw_segment_lattice host_draw_segment_lattice
#define offset_block_copies host_offset_block_copies
#define extend_block_scaled host_extend_block_scaled
#define draw_side_face host_draw_side_face
#define draw_quad_list host_draw_quad_list
#define draw_quad_strip host_draw_quad_strip
#define draw_face_grid host_draw_face_grid
#define draw_face_grid_plain host_draw_face_grid_plain
#define draw_face_lattice host_draw_face_lattice
#define draw_face_lattice_plain host_draw_face_lattice_plain
#define draw_face_lattice_with_hooks host_draw_face_lattice_with_hooks
#define draw_face_lattice_plain_with_hooks host_draw_face_lattice_plain_with_hooks
#define draw_block_face host_draw_block_face
#define draw_offset_run host_draw_offset_run
#define draw_split_square host_draw_split_square
#define draw_side_triangle host_draw_side_triangle
#define square_diagonal host_square_diagonal
#define draw_square_faces host_draw_square_faces
#define draw_record_shadow host_draw_record_shadow
#define test_stream_face host_test_stream_face
#define test_stream_face_accumulation host_test_stream_face_accumulation
#define draw_tested_face host_draw_tested_face
#define draw_tested_parallelogram host_draw_tested_parallelogram
#define draw_indexed_face_list host_draw_indexed_face_list
#define draw_stream_circles host_draw_stream_circles
#define get draw_vertex_get
#define put draw_vertex_put
#include "../../port/game/draw_stream.c"
#undef get
#undef put
#include "../../port/game/native/model.c"
#undef draw_selected_segment
#undef draw_selected_segment_clipped
#undef draw_selected_segment_near
#undef draw_segment_pairs
#undef draw_segment_pairs_near
#undef draw_segment_run
#undef draw_offset_segments
#undef draw_parallelogram_face
#undef draw_parallelogram_face_2
#undef draw_parallelogram_face_near
#undef draw_offset_face
#undef draw_mixed_face
#undef extend_parallelograms
#undef extend_parallelograms_scaled
#undef draw_segment_grid
#undef draw_segment_lattice
#undef offset_block_copies
#undef extend_block_scaled
#undef draw_side_face
#undef draw_quad_list
#undef draw_quad_strip
#undef draw_face_grid
#undef draw_face_grid_plain
#undef draw_face_lattice
#undef draw_face_lattice_plain
#undef draw_face_lattice_with_hooks
#undef draw_face_lattice_plain_with_hooks
#undef draw_block_face
#undef draw_offset_run
#undef draw_split_square
#undef draw_side_triangle
#undef square_diagonal
#undef draw_square_faces
#undef draw_record_shadow
#undef test_stream_face
#undef draw_tested_face
#undef draw_tested_parallelogram
#undef draw_indexed_face_list
#undef draw_stream_circles

#undef FA18_NATIVE
#undef setup_line
#undef draw_line
#undef draw_line_to_row
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
static gaddr oracle_parameters;
static int16_t circle_x,circle_y,circle_radius;
static unsigned strip_groups;
static int original(uint32_t pc) {
    memset(REG_DA,0,sizeof REG_DA); REG_A[4]=rd_u16(LINE_LAST_ROW); REG_A[7]=0xc7ff00u; wr_u32(REG_A[7],0xc70000u);
    REG_A[0]=oracle_parameters;
    if(pc==0xc0da38u || pc==0xc0d730u) {
        /* C0DA38 unlinks its caller's frame, not its child return address. */
        REG_A[6]=0xc7fefcu;REG_A[7]=0xc7fef0u;wr_u32(REG_A[6],0);
    }
    if(pc==0xc21b38u || pc==0xc21c86u) REG_A[2]=0x4600;
    if(pc==0xc1fe68u) REG_A[2]=0x4600;
    if(pc==0xc0cfb6u) { REG_A[6]=0x4200;REG_A[2]=0x4600; }
    if(pc==0xc1ff0au || pc==0xc207feu) { REG_A[6]=0x4200;REG_A[2]=0x4600; }
    if(pc==0xc2f1c0u) {REG_D[0]=(uint32_t)(int32_t)circle_x;REG_D[1]=(uint32_t)(int32_t)circle_y;REG_D[6]=(uint32_t)(int32_t)circle_radius;}
    m68k_set_reg(M68K_REG_SR,0x2700); REG_PC=pc;
    fa18_next_event=INT64_MAX; SET_CYCLES(100000000);
    for(unsigned step=0;step<2000000;++step) {
        uint16_t opcode;
        if(REG_PC==0xc70000u && REG_A[7]==0xc7ff04u) { wait_blitter(); return 1; }
        if(REG_PC==0xc1f598u) ++strip_groups; /* Positive strip's initial point. */
        int cycles_before=GET_CYCLES();
        opcode=rd_u16(REG_PC); REG_PPC=REG_PC; REG_IR=opcode; REG_PC+=2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
        fa18_machine->cycle+=cycles_before-GET_CYCLES();
    }
    fprintf(stderr,"original renderer did not return at %06X\n",REG_PC); return 0;
}
static int compare(const uint8_t *expected,unsigned test) {
    unsigned count=0;
    for(unsigned i=0;i<0x80000;++i) if(fa18_machine->chip[i]!=expected[i]) {
        if(i>=0x4168 && i<0x4200) continue; /* host model frame */
        if(count<8) fprintf(stderr,"case %u plane byte %06X: source %02X native %02X\n",test,i,expected[i],fa18_machine->chip[i]);
        ++count;
    }
    if(count) { fprintf(stderr,"%u plane/buffer differences\n",count); return 0; }
    return 1;
}

static unsigned calls, failures;
static int workspace_script_cases(void) {
    FA18Machine *saved=malloc(sizeof *saved);
    if(!saved) return 0;
    memcpy(saved,fa18_machine,sizeof *saved);
    unsigned cases=0;
    for(unsigned value=0;value<65536;++value) {
        wr_u16(STREAM_MODE,0); wr_u16(WORKSPACE_RECORDS+4,(uint16_t)value);
        gaddr cursor=select_workspace_script_block(0x4600);
        if(!original(0xc1fe68u) || REG_A[2]!=cursor || (uint16_t)REG_D[0]!=0) {
            fprintf(stderr,"workspace selector value %04X differs\n",value);return 0;
        }
        ++cases;
    }
    const uint16_t indices[]={15,0xffff,0x400,0x7ff};
    const uint16_t values[]={0,1,2,7,11,12,116,117,127,128,0x7fff,0x8000,0xffff};
    for(unsigned i=0;i<4;++i) for(unsigned j=0;j<13;++j) {
        wr_u16(STREAM_MODE,indices[i]);
        gaddr record=WORKSPACE_RECORDS+(gaddr)(int32_t)(int16_t)(indices[i]*32u);
        wr_u16(record+4,values[j]);
        gaddr cursor=select_workspace_script_block(0x4600);
        if(!original(0xc1fe68u) || REG_A[2]!=cursor || (uint16_t)REG_D[0]!=0) {
            fprintf(stderr,"workspace selector index %04X value %04X differs\n",indices[i],values[j]);return 0;
        }
        ++cases;
    }
    memcpy(fa18_machine,saved,sizeof *saved);free(saved);
    printf("%u workspace-script cases match original cursor/result, including all word values\n",cases);
    return 1;
}
static int stream_circle_cases(void) {
    FA18Machine *saved=malloc(sizeof *saved),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!saved || !before || !expected) return 0;
    memcpy(saved,fa18_machine,sizeof *saved);
    for(unsigned test=0;test<24;++test) {
        memcpy(fa18_machine,saved,sizeof *saved);
        gaddr cursor=0x4600;unsigned count=1+test%3;
        const int16_t depths[]={-1,0,1,100,256,1000};
        wr_u16(0x4200-8,test/12);
        wr_u16(LINE_LAST_ROW,179);wr_u32(PROJECTED_PAIR,0xffffffffu);
        for(unsigned i=0;i<count;++i) {
            gaddr point=WORKSPACES+6*i,entry=cursor+6*i;
            wr_s16(point,(test&1)?400:0);wr_s16(point+2,(int16_t)(20*i));
            wr_s16(point+4,depths[(test+i)%6]);
            wr_u16(entry,6*i);wr_u16(entry+2,(test+i)&15);
            uint16_t radius=(uint16_t)((test&2)?0:300+100*i);
            wr_u16(entry+4,(uint16_t)(radius|(i+1==count?0x8000u:0)));
        }
        memcpy(before,fa18_machine,sizeof *before);
        uint16_t result=(uint16_t)host_draw_stream_circles(&cursor,0x4200);
        memcpy(expected,fa18_machine->chip,0x80000);memcpy(expected+0x80000,fa18_machine->slow,0x80000);
        memcpy(fa18_machine,before,sizeof *before);
        if(!original(0xc0cfb6u) || (uint16_t)REG_D[0]!=result || REG_A[2]!=cursor) {
            fprintf(stderr,"circle stream case %u return/cursor differs\n",test);return 0;
        }
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?fa18_machine->chip[i]:fa18_machine->slow[i-0x80000];
            if(actual!=expected[i]) {fprintf(stderr,"circle stream case %u RAM %06X differs\n",test,i);return 0;}
        }
    }
    memcpy(fa18_machine,saved,sizeof *saved);free(expected);free(before);free(saved);
    puts("24 complete circle-stream cases match original cursor/result and all non-stack RAM/display");
    return 1;
}
static int full_selection_cases(void) {
    static const int16_t thresholds[]={-32768,-32767,-1,0,5000,14399,14400,14401,32767};
    FA18Machine *saved=malloc(sizeof *saved),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!saved || !before || !expected) return 0;
    memcpy(saved,fa18_machine,sizeof *saved);
    for(unsigned test=0;test<18;++test) {
        memcpy(fa18_machine,saved,sizeof *saved);
        const int mode=test/9;const int16_t threshold=thresholds[test%9];
        wr_u8(CONTEXT_SELECT,(uint8_t)mode);wr_u16(UPDATE_DISPLAY_FLAGS,0x2000);
        wr_s16(DISPLAY_MODE_ZERO_THRESHOLD,mode?32767:threshold);
        wr_s16(VIEW_PAN,mode?threshold:-32768);
        wr_u16(DISPLAY_SELECTION_WORD_A,0x1234);wr_u16(DISPLAY_SELECTION_WORD_B,0x5678);
        wr_u32(DISPLAY_SELECTION_LONG,0x89abcdefu);wr_u8(DISPLAY_SELECTION_FLAG,0xa5);
        memcpy(before,fa18_machine,sizeof *before);
        int result=prepare_full_display_selection();
        memcpy(expected,fa18_machine->chip,0x80000);memcpy(expected+0x80000,fa18_machine->slow,0x80000);
        memcpy(fa18_machine,before,sizeof *before);
        if(!original(test&1?0xc0d730u:0xc0da38u) || REG_D[0]!=(uint32_t)result) return 0;
        for(unsigned i=0;i<0xffc00;++i) {
            uint8_t actual=i<0x80000?fa18_machine->chip[i]:fa18_machine->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"full selection case %u data %06X: source %02X native %02X\n",test,i,actual,expected[i]);return 0;
            }
        }
    }
    memcpy(fa18_machine,saved,sizeof *saved);free(expected);free(before);free(saved);
    puts("18 full-viewport selection cases match original result, enclosing-frame exit and non-stack RAM");return 1;
}
static int carrier_commands(void) {
    FA18Machine *saved=malloc(sizeof *saved),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    memcpy(saved,fa18_machine,sizeof *saved);
    for(unsigned test=0;test<48;++test) {
        gaddr stream=0x4600;unsigned flag=test<16;
        wr_u8(CONTEXT_SELECT,test&1);wr_u16(VIEW_RECORD,(test&2)?512:0);
        gaddr viewed=CONTROL_RECORDS+rd_u16(VIEW_RECORD);
        wr_u8(viewed+4,(test&4)?0x40:0);wr_u16(0x4200-0x7a,test&8);
        if(!flag) {
            static const uint16_t kinds[]={0x0100,0x1000,0x0400,0x0800,0x0c00,0x1400,0x1800,0x1c00};
            wr_u16(stream,0);wr_u16(stream+2,6);wr_u16(stream+4,12);
            wr_u16(stream+6,kinds[test&7]);wr_u16(stream+8,0);
            wr_u32(0x4200-0x2c,0x4800);wr_u16(BOUND_SHIFT,test&3);
            for(unsigned k=0;k<6;++k) wr_s16(0x4800+2*k,(int16_t)(k<3?100+test:256-k*50));
        }
        memcpy(before,fa18_machine,sizeof *before);
        uint16_t result=flag?(uint16_t)command(0x84,&stream,0x4200):test_stream_face_accumulation(&stream,0x4200);
        memcpy(expected,fa18_machine->chip,0x80000);memcpy(expected+0x80000,fa18_machine->slow,0x80000);
        memcpy(fa18_machine,before,sizeof *before);
        if(!original(flag?0xc207fe:0xc1ff0a) || (uint16_t)REG_D[0]!=result || REG_A[2]!=stream) {
            fprintf(stderr,"carrier command case %u result/stream mismatch\n",test);return 0;
        }
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?fa18_machine->chip[i]:fa18_machine->slow[i-0x80000];
            if(actual!=expected[i]) {fprintf(stderr,"carrier command case %u memory %06X differs\n",test,i);return 0;}
        }
    }
    memcpy(fa18_machine,saved,sizeof *saved);free(expected);free(before);free(saved);
    puts("48 carrier flag/face-result cases match original result word, stream and non-stack RAM");return 1;
}
static int circles(void) {
    FA18Machine *saved=malloc(sizeof *saved),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x80000);
    memcpy(saved,fa18_machine,sizeof *saved);
    gaddr table=rd_u32(PAGE_PLANE_TABLE),offsets[4];
    for(int k=0;k<4;++k) {
        offsets[k]=rd_u32(table+4*k)-rd_u32(table+12);
        if(offsets[k]!=(gaddr)((3-k)*0x1f40)) {
            fputs("Native circle plane layout does not retain source spacing/order\n",stderr);return 0;
        }
    }
    for(unsigned test=0;test<48;++test) {
        circle_x=(int16_t)(test%3==0?2:test%3==1?317:160);
        circle_y=(int16_t)(test%4==0?1:test%4==1?179:90);
        circle_radius=(int16_t)(test<8?test:test%4==0?127:3+test*2);
        wr_u32(CIRCLE_SPANS_PTR,0x33000);
        wr_u16(CURRENT_COLOUR,(uint16_t)(test&15));wr_u16(LINE_LAST_ROW,179);
        wr_u32(LINE_STYLE,0x000fffffu);
        for(int k=0;k<4;++k) wr_u32(rd_u32(PAGE_PLANE_TABLE)+4*k,0x10000+(3-k)*0x1f40);
        memcpy(before,fa18_machine,sizeof *before);
        host_draw_filled_circle(circle_x,circle_y,circle_radius);
        memcpy(expected,fa18_machine->chip,0x80000);
        memcpy(fa18_machine,before,sizeof *before);
        if(!original(0xc2f1c0u) || !compare(expected,test)) return 0;
        /* Also exercise the actual native spacing/order, comparing logical
         * plane contents with the original's fixed contiguous arrangement. */
        memcpy(fa18_machine,before,sizeof *before);
        for(int k=0;k<4;++k) {
            memcpy(fa18_machine->chip+0x20000+offsets[k],before->chip+0x10000+(3-k)*0x1f40,8000);
            wr_u32(rd_u32(PAGE_PLANE_TABLE)+4*k,0x20000+offsets[k]);
        }
        host_draw_filled_circle(circle_x,circle_y,circle_radius);
        for(int k=0;k<4;++k) if(memcmp(fa18_machine->chip+0x20000+offsets[k],
                expected+0x10000+(3-k)*0x1f40,8000)) {
            fprintf(stderr,"native circle plane layout differs: case %u plane %d\n",test,k);return 0;
        }
    }
    memcpy(fa18_machine,saved,sizeof *saved);
    free(expected);free(before);free(saved);
    puts("48 circle span/mask cases match source buffers and relocated actual native plane layout");return 1;
}
static int32_t compare_descriptor(void *context,const ScenePlacementCall *call) {
    (void)context;
    FA18Machine *before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x80000),*vertices=malloc(0x2000),*records=malloc(0x2000),*slow=malloc(0x80000);
    memcpy(before,fa18_machine,sizeof *before);
    int result=native_scene_placement(NULL,call);
    memcpy(expected,fa18_machine->chip,0x80000);
    memcpy(vertices,fa18_machine->slow+0x48390,0x2000);
    memcpy(records,fa18_machine->slow+0x46184,0x2000);
    memcpy(slow,fa18_machine->slow,0x80000);
    memcpy(fa18_machine,before,sizeof *before);
    oracle_parameters=call->parameters;
    /* C1F074 expiry returns the incoming, uncleared model accumulator.
     * Match that caller scratch input in the independent source stack frame;
     * normal C1F712 and command ORs must still update it themselves. */
    for(unsigned i=0;i<0x98;++i) wr_u8(0xc7fefc-0x98+i,rd_u8(0x4200-0x98+i));
    if(!original(call->routine)) exit(1);
    int original_result=(int16_t)REG_D[0];
    unsigned vertex_diffs=0;
    for(unsigned i=0;i<0x2000;++i) if(vertices[i]!=fa18_machine->slow[0x48390+i]) ++vertex_diffs;
    unsigned record_diffs=0;
    for(unsigned i=0;i<0x2000;++i) if(records[i]!=fa18_machine->slow[0x46184+i]) {
        if(record_diffs<8) fprintf(stderr,"record %06X: source %02X native %02X\n",0xc46184+i,fa18_machine->slow[0x46184+i],records[i]);
        ++record_diffs;
    }
    unsigned slow_diffs=0;
    for(unsigned i=0;i<0x7fc00;++i) if(slow[i]!=fa18_machine->slow[i]) {
        if(slow_diffs<8) fprintf(stderr,"descriptor %06X data %06X: source %02X native %02X\n",call->routine,0xc00000+i,fa18_machine->slow[i],slow[i]);
        ++slow_diffs;
    }
    int okay=compare(expected,calls) && result==original_result && !vertex_diffs && !record_diffs && !slow_diffs;
    if(!okay) {
        fprintf(stderr,"descriptor %06X parameters %06X: return source=%d native=%d vertex differences=%u record differences=%u scratch source=%04X native=%04X\n",
            call->routine,call->parameters,original_result,result,vertex_diffs,record_diffs,
            rd_u16(0xc7fefc-0x7c),(unsigned)((expected[0x4200-0x7c]<<8)|expected[0x4200-0x7b]));
        ++failures;
    }
    ++calls;free(slow);free(records);free(vertices);free(expected);free(before);
    return original_result;
}
static unsigned expiry_cases;
static int32_t consume(void *context,const ScenePlacementCall *call) {
    if(call->routine!=0xc22ac0) return compare_descriptor(context,call);
    FA18Machine *before=malloc(sizeof *before),*after=malloc(sizeof *after);
    if(!before || !after) exit(1);
    memcpy(before,fa18_machine,sizeof *before);
    int32_t result=compare_descriptor(context,call);
    memcpy(after,fa18_machine,sizeof *after);
    /* Keep each reached disk-backed aircraft descriptor and its real render
     * inputs. Vary only destruction/selection boundaries for this source
     * routine; none of these fixtures enters the playable game's state. */
    for(unsigned test=0;test<4;++test) {
        memcpy(fa18_machine,before,sizeof *before);
        uint16_t chosen=rd_u16(CHOSEN_RECORD);
        gaddr record=CONTROL_RECORDS+(gaddr)(int32_t)(int16_t)chosen;
        wr_u16(record,(rd_u16(record)|0x240u)&0xfbffu);
        wr_s16(record+0x4c,-1);
        wr_u16(SELECTED_RECORD,test==0?0xffffu:test==1?(chosen^512u):chosen);
        wr_u32(WARNING_CAUSES,0xffffffffu);
        compare_descriptor(context,call);
        ++expiry_cases;
        if(rd_u16(record+0x4c)!=15 || (rd_u16(record)&0x600u)!=0x400u ||
           (test>=2 && (rd_u16(SELECTED_RECORD)!=0xffffu ||
                        rd_u32(WARNING_CAUSES)!=0xffffbdffu || rd_u16(MESSAGE_CODE)!=0x4016u))) {
            fputs("Destroyed-aircraft transition/selection contract failed\n",stderr);exit(1);
        }
        /* A second render must not restart expiry or repost the message. */
        if(test==3) {
            wr_u16(record+0x4c,12);wr_u16(MESSAGE_CODE,0);
            compare_descriptor(context,call);
            if(rd_u16(record+0x4c)!=12 || rd_u16(MESSAGE_CODE)) {
                fputs("Aircraft expiry restarted on a second render\n",stderr);exit(1);
            }
            ++expiry_cases;
        }
    }
    memcpy(fa18_machine,after,sizeof *after);
    free(after);free(before);return result;
}
static int32_t consume_followup(void *context,const FollowupPlacementEvent *call) {
    ScenePlacementCall descriptor={.routine=call->routine,.parameters=call->parameters,.header=call->header};
    return consume(context,&descriptor);
}
static int32_t native_followup(void *context,const FollowupPlacementEvent *call) {
    (void)context;
    ScenePlacementCall descriptor={.routine=call->routine,.parameters=call->parameters,.header=call->header};
    return native_scene_placement(NULL,&descriptor);
}
static void grid_triangle(void *context) {(void)context;host_draw_polygon();}
static void grid_pixel(void *context,int16_t x,int16_t y,int adjacent) {
    (void)context;if(adjacent) plot_pixel_pair(x,y);else plot_pixel(x,y);
}
static MessageWorking control_child(void *context,enum MainControlChild child) {
    (void)context;
    if(child!=MC_RESET_FACE_STATE) {fprintf(stderr,"unexpected setup control child %u\n",child);exit(1);}
    host_reset_line_style();return (MessageWorking){0};
}
static int scene_children(void) {
    const FollowupPlacementHooks followups={native_followup,NULL,NULL};
    const GridProjectionHooks grid={.triangle=grid_triangle,.pixel=grid_pixel};
    const MainControlHooks controls={control_child,NULL,NULL,NULL};
    const uint32_t entries[]={0xc279d0,0xc1518c,0xc1ccbc};
    FA18Machine *before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    for(unsigned test=0;test<3;++test) {
        memcpy(before,fa18_machine,sizeof *before);
        if(test==0) host_draw_grid_projection_packet(0x4400,&grid);
        else if(test==1) advance_main_loop_control_records(0x4500,&controls);
        else visit_followup_placements(&followups);
        memcpy(expected,fa18_machine->chip,0x80000);
        memcpy(expected+0x80000,fa18_machine->slow,0x80000);
        memcpy(fa18_machine,before,sizeof *before);
        oracle_parameters=0;
        /* The parent's JSR adds one return address before the model LINK.
         * Align its incoming local scratch, including the expiry accumulator. */
        if(test==2) for(unsigned i=0;i<0x98;++i) wr_u8(0xc7fef8-0x98+i,rd_u8(0x4200-0x98+i));
        if(!original(entries[test])) return 0;
        unsigned differences=0;
        for(unsigned i=0;i<0x100000;++i) {
            if((i>=0x4168&&i<0x4200)||(i>=0x43dc&&i<0x4400)||(i>=0x44ea&&i<0x4500)||i>=0xffc00) continue;
            uint8_t actual=i<0x80000?fa18_machine->chip[i]:fa18_machine->slow[i-0x80000];
            if(actual!=expected[i]) {
                if(differences<8) fprintf(stderr,"parent %06X address %06X: source %02X native %02X\n",entries[test],i<0x80000?i:0xc00000+i-0x80000,actual,expected[i]);
                ++differences;
            }
        }
        if(differences) {fprintf(stderr,"parent %06X: %u non-stack RAM differences\n",entries[test],differences);return 0;}
        memcpy(fa18_machine,before,sizeof *before);
    }
    free(before);free(expected);puts("Grid, setup control and complete followup parents match non-stack RAM");return 1;
}
static int hull_tails(void) {
    FA18Machine *before=malloc(sizeof *before),*saved=malloc(sizeof *saved);
    uint8_t *expected=malloc(0x80000);
    memcpy(saved,fa18_machine,sizeof *saved);
    for(unsigned test=0;test<8;++test) {
        memcpy(fa18_machine,saved,sizeof *saved);
        wr_u16(SCRIPT_RECORD,(test&2)?14*512:0);
        wr_u16(0x4600,(test&4)?6:0);
        memcpy(before,fa18_machine,sizeof *before);
        gaddr next=(test&1)?derive_extended_shown_vertices(0x4600):derive_compact_shown_vertices(0x4600);
        memcpy(expected,fa18_machine->slow,0x80000);
        memcpy(fa18_machine,before,sizeof *before);
        oracle_parameters=0;
        if(!original((test&1)?0xc21c86u:0xc21b38u) || REG_A[2]!=next) return 0;
        for(unsigned i=0;i<0x7fc00;++i) if(fa18_machine->slow[i]!=expected[i]) {
            fprintf(stderr,"hull tail case %u data %06X: source %02X native %02X\n",test,0xc00000+i,fa18_machine->slow[i],expected[i]);return 0;
        }
    }
    memcpy(fa18_machine,saved,sizeof *saved);
    free(expected);free(saved);free(before);puts("8 compact/extended hull tail cases match original data and stream position");return 1;
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m);
    if(!state||!rom||!data||nd!=0x100000||!m) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
    if(!workspace_script_cases() || !stream_circle_cases()) return 1;
    if(!full_selection_cases()) return 1;
    if(!carrier_commands()) return 1;
    if(!hull_tails()) return 1;
    if(!circles()) return 1;
    const ScenePlacementHooks hooks={consume,NULL,NULL};
    visit_scene_placements(0,&hooks);visit_scene_placements(1,&hooks);
    const FollowupPlacementHooks followups={consume_followup,NULL,NULL};
    visit_followup_placements(&followups);
    if(!expiry_cases) {fputs("No aircraft expiry descriptor exercised\n",stderr);return 1;}
    printf("%u destroyed-aircraft expiry/selection/repeated-render cases compared\n",expiry_cases);
    if(!scene_children()) return 1;
    printf("%u descriptors compared, %u failures\n",calls,failures);
    printf("%u positive model strip groups exercised in original rendering\n",strip_groups);
    return failures?1:0;
}
