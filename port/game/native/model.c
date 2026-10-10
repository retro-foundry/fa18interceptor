/* Source C1EE14: distance/LOD selection, bound vertices and model commands.
 * C1ED48 is the source alias. C07846 extends flat geometry with paired edges.
 * The existing draw_stream.c owns command geometry and host raster submission.
 * C1ED4C selects aircraft/cockpit streams; C1F000 transforms record hulls.
 * C22AC0/C22ADE starts destroyed-aircraft expiry before its descriptor draw.
 * Reached missing children fail explicitly, never substitute geometry. */
#include "model.h"
#include "frontend.h"
#include "../globals.h"
#include "../draw_stream.h"
#include "../fixed_math.h"
#include "../plane_tests.h"
#include "../view_transform.h"
#include "../stages.h"
#include "../polygon_clip.h"
#include "../projection.h"
#include "../projection_readouts.h"
#include "../fault.h"
#include "../vertex_tail.h"
#include "../model_strips.h"
#include "../history_projection.h"
#include "../cockpit_script.h"
#include "../faces.h"
#include "../render_line.h"
#include "../context_publication.h"
#include "../messages.h"
#include "../main_loop_timers.h"
#include <stdio.h>
#include <stdlib.h>

enum { CONTROL_STREAM=0xC45A36, HEADER_BYTE=0xC4585B,
    DISTANCE_GATE=0xC45ABA, VISIT_CLOCK=0xC458BD };
static void ground_bounds_input(void *context,enum MainTimerPhase phase,uint32_t value,uint32_t other) {
    MainTimerBounds *bounds=context;
    switch(phase) {
    case MT_BOUNDS_OFFSET: bounds->cursor=other; break;
    case MT_BOUNDS_LOAD: bounds->record=value; break;
    case MT_BOUNDS_VECTOR: bounds->width=(int16_t)value; bounds->height=(int16_t)other; break;
    default: break;
    }
}
static MainTimerBounds ground_bounds_scale(void *context,enum MainTimerChild child) {
    MainTimerBounds bounds=*(MainTimerBounds *)context;
    if(child!=MT_SCALE_BOUNDS) abort();
    /* C25298-C252A8 supply the endpoint delta as (D5,0,D7), with
     * D0=46, to the existing C2574A register-entry normalization. */
    NormalizedVectorState vector={.scale=46,.x=(uint32_t)(int32_t)bounds.width,
        .z=(uint32_t)(int32_t)bounds.height};
    vector=normalize_record_vector(vector,NULL,NULL);
    bounds.width=(int16_t)vector.x; bounds.height=(int16_t)vector.z;
    return bounds;
}
void native_model_prepare_ground_bounds(void) {
    /* C0F50E executes this after hunk relocation, before the first scene.
     * The hunk leaves the six derived corner words zero in every record. */
    MainTimerBounds bounds={0};
    const MainTimerHooks hooks={.consume=ground_bounds_scale,.observe=ground_bounds_input,.context=&bounds};
    prepare_setup_bounds(&hooks);
}
static void missing(const char *part,gaddr at) {
    fprintf(stderr,"native model missing %s at %06X\n",part,at);
    fprintf(stderr,"native model player position x=%.3f y=%.3f z=%.3f; pose_index=%u mode=%u stage=%06X\n",
        rd_s32(CONTROL_RECORDS+20)/256.0,rd_s32(CONTROL_RECORDS+24)/256.0,
        rd_s32(CONTROL_RECORDS+28)/256.0,rd_u8(SCENE_POSE_ENTRY),rd_u8(MODE_SELECT),rd_u32(STAGE_CALLBACK));
    fflush(stderr);abort();
}
static int16_t word(gaddr *p) { int16_t n=rd_s16(*p); *p+=2; return n; }
static ReadoutState projection_child(void *context,enum ReadoutChild child) {
    if(child!=PR_FAULT) abort();
    fault_hook();return *(const ReadoutState *)context;
}
static int16_t shift_word(int16_t n,int count) {
    count&=63; return count>=16?(n<0?-1:0):(int16_t)(n>>count);
}
static int32_t shift_long(int32_t n,int count) {
    count&=63; return count>=32?(n<0?-1:0):n>>count;
}
static int16_t scaled_range(void) {
    uint32_t range=rd_u16(MAGNITUDE);
    if(!rd_u8(ZOOM_FLAGS)) {
        uint32_t zoom=(uint32_t)(int32_t)rd_s16(ZOOM_SCALE)<<8;
        uint32_t quotient=zoom/0x80u;
        if(quotient>0xffffu) missing("zoom division overflow",ZOOM_SCALE);
        range=(range*quotient)>>8;
    }
    return (int16_t)range;
}
static int16_t dot(gaddr matrix,const int16_t point[3]) {
    uint32_t sum=0;
    for(int k=0;k<3;++k) sum+=(uint32_t)((int32_t)point[k]*rd_s16(matrix+2*k));
    return (int16_t)((int32_t)sum>>8);
}
static void transform_vertex(gaddr input,gaddr output,gaddr frame,int flat) {
    int16_t point[3]; int down=8-rd_s16(frame-6),shift=rd_s16(frame-8);
    for(int k=0;k<3;++k) {
        int32_t origin=(int32_t)(rd_u32(frame-0x20+4*k)+rd_u32(SHADOW_OFFSET_X+4*k));
        point[k]=(int16_t)shift_long(origin,down);
    }
    point[0]=(int16_t)(point[0]+shift_word(rd_s16(input),shift));
    if(flat) point[2]=(int16_t)(point[2]+shift_word(rd_s16(input+2),shift));
    else {
        point[1]=(int16_t)(point[1]+shift_word(rd_s16(input+2),shift));
        point[2]=(int16_t)(point[2]+shift_word(rd_s16(input+4),shift));
    }
    for(int row=0;row<3;++row) {
        int16_t value;
        if(flat) {
            int16_t horizontal[3]={point[0],0,point[2]};
            value=(int16_t)(dot(VIEW_ANGLE_MATRIX+6*row,horizontal)+rd_s16(frame-0x78+2*row));
        } else value=dot(VIEW_ANGLE_MATRIX+6*row,point);
        wr_s16(output+2*row,value);
    }
}
static int first_visible(gaddr p) {
    int16_t z=rd_s16(p+4),x=rd_s16(p),y=rd_s16(p+2);
    int16_t extent=(int16_t)(z+(z>>1));
    return z>0 && x<=extent && (int16_t)-x<=extent && y<=extent && (int16_t)-y<=extent;
}
static void record_finish(gaddr record,gaddr frame,int16_t timer) {
    /* C1F87A -> C2D082: expiry shape layers, then C0D04C history points. */
    int scale=15-timer;
    if(scale<=15) {
        int32_t world[3]={rd_s16(record+0xc),rd_s32(record+0x10),rd_s16(record+0xe)};
        world[0]=(int32_t)((uint32_t)world[0]+((uint32_t)(int32_t)(int16_t)(rd_s16(record+6)-rd_s16(0xc4594c))<<14));
        world[2]=(int32_t)((uint32_t)world[2]+((uint32_t)(int32_t)(int16_t)(rd_s16(record+8)-rd_s16(0xc4594e))<<14));
        int kind=rd_u8(record+0x62)==0x15?3:(rd_u8(record+4)&2)?2:1;
        reset_line_style();wr_u32(POLY_COMPLEMENT,0);
        int32_t vertical=(int32_t)((uint32_t)world[1]+rd_u32(PROJECTION_Y));
        int32_t absolute=vertical<0?(int32_t)(0u-(uint32_t)vertical):vertical;
        if(absolute<=0x8000) {
            int16_t radius=0;
            if(kind==3) {
                int32_t distance[3];int32_t maximum=0;
                for(int k=0;k<3;++k) {
                    distance[k]=(int32_t)((uint32_t)world[k]+(uint32_t)(int32_t)rd_s16(PROJECTION_WORDS+2*k));
                    if(distance[k]<0) distance[k]=(int32_t)(0u-(uint32_t)distance[k]);
                    if(distance[k]>maximum) maximum=distance[k];
                }
                int down=maximum<=0x4000?0:maximum<=0x40000?4:8;
                int16_t length=(int16_t)magnitude3((int16_t)(distance[0]>>down),(int16_t)(distance[1]>>down),(int16_t)(distance[2]>>down));
                uint16_t numerator=(uint16_t)((0x2800u*(uint16_t)scale)/6u+0x2800u);
                radius=length>0?(int16_t)((uint32_t)(int32_t)(int16_t)numerator/(uint16_t)length):127;
                if(radius>127) radius=127;
            }
            int16_t point[3];int shift=rd_s16(BOUND_SHIFT);
            world[0]=(int32_t)((uint32_t)world[0]+(uint32_t)(int32_t)rd_s16(PROJECTION_WORDS));
            world[1]=vertical;
            world[2]=(int32_t)((uint32_t)world[2]+(uint32_t)(int32_t)rd_s16(PROJECTION_WORDS+4));
            for(int k=0;k<3;++k) point[k]=(int16_t)shift_long(world[k],shift);
            draw_shape(point[0],point[1],point[2],(uint16_t)scale,(int8_t)kind,radius,(int16_t)shift);
            if(kind>=3) {
                ++kind;if(--scale<0) scale=0;
                draw_shape(point[0],point[1],point[2],(uint16_t)scale,(int8_t)kind,radius,(int16_t)shift);
                if(kind>=4) {
                    ++kind;if(--scale<0) scale=0;
                    draw_shape((int16_t)(point[0]+20),(int16_t)(point[1]+10),(int16_t)(point[2]+20),
                        (uint16_t)scale,(int8_t)kind,radius,(int16_t)shift);
                }
            }
        }
    }
    (void)frame;
    HistoryProjectionWork history={0};draw_history_projection(&history);
}
static int record_vertices(gaddr bound,gaddr frame) {
    /* C1F000-C1F2EA: rotate the hull into the record's cached local points,
     * then shift and place the visible subset through the view matrix. The
     * source consumes one extra cached point when the visible subset is full. */
    gaddr record=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(SCRIPT_RECORD);
    if(!(rd_u16(record)&0x40)) return 0;
    wr_u8(record+0x7b,rd_u8(frame-0x88));
    if(rd_u16(record)&0x400) {
        int16_t timer=rd_s16(record+0x4c);
        if(timer==14) {
            unsigned count=rd_u16(BOUND_SHIFT)&63;
            uint32_t scaled=count>=32?0:(uint32_t)(int32_t)rd_s16(frame-0x28)<<count;
            int32_t denominator=shift_long((int32_t)scaled,1);
            int16_t radius=120;
            if((int16_t)denominator>0) {
                uint32_t d=(uint16_t)denominator;
                uint32_t packed=((0x10000u%d)<<16)|(0x10000u/d);
                if((int32_t)packed<=120) radius=(int16_t)packed;
            }
            wr_u8(0xc45869,(uint8_t)radius);wr_u8(0xc4586a,(uint8_t)(radius>>1));
            wr_u8(0xc457b8,(uint8_t)(4-((radius>>1)>>4)));
        }
        if(timer<=12) {record_finish(record,frame,timer);return -1;}
    }
    int16_t origin[3];int down=8-rd_s16(frame-6),shift=rd_s16(frame-8);
    for(int k=0;k<3;++k) {
        int32_t offset=k==1?rd_s32(POSITION_LEVEL):
            (int32_t)(rd_u32(frame-0x20+4*k)+rd_u32(SHADOW_OFFSET_X+4*k));
        origin[k]=(int16_t)shift_long(offset,down);wr_s16(frame-0x14+2*k,origin[k]);
    }
    for(int i=0;i<9;++i) wr_s16(BOUND_MATRIX+2*i,(int16_t)(rd_s16(record+0x92+2*i)>>6));
    int visible=rd_s8(bound+9)-1,extra=rd_s8(bound+8)-rd_s8(bound+9);
    if(extra<0) missing("invalid hull counts C1F2D4",bound);
    gaddr input=bound+10,cache=record+0xa4,output=WORKSPACES;
    unsigned cached=0;int first=1;
    for(;;) {
        int16_t point[3],local[3];
        for(int k=0;k<3;++k) point[k]=rd_s16(input+2*k);
        input+=6;
        for(int row=0;row<3;++row) local[row]=dot(BOUND_MATRIX+6*row,point);
        if(cached<58) {
            for(int k=0;k<3;++k) wr_s16(cache+6*cached+2*k,local[k]);
            ++cached;
        }
        if(!first) --visible;
        if(visible>=0) {
            if(rd_u8(frame-0x7f)&1) {
                /* C1F158 -> C1F2EE reloads the original vertex at A1-6.
                 * An attached camera uses model coordinates; the rotated
                 * cache is for the world-space branch below. */
                view_transform(input-6,(int16_t)shift,output);
            } else {
                for(int k=0;k<3;++k) point[k]=(int16_t)(shift_word(local[k],shift)+origin[k]);
                for(int row=0;row<3;++row) wr_s16(output+2*row,dot(VIEW_ANGLE_MATRIX+6*row,point));
            }
            if(first && !(rd_u8(frame-0x7f)&1) && rd_s16(0xc459c0)>=0 && rd_u16(0xc459c0)==rd_u16(SCRIPT_RECORD)) {
                ReadoutState point={.x=(uint32_t)(int32_t)rd_s16(output),.y=(uint32_t)(int32_t)rd_s16(output+2),
                    .value=(uint32_t)(int32_t)rd_s16(output+4)};
                const ReadoutHooks hooks={.consume=projection_child,.context=&point};
                point=project_and_plot_point_result(point,-5,&hooks); /* C1F1D0 uses returned D0/D1, including rejection. */
                wr_s16(TARGET_MARK,(int16_t)(point.x-rd_u16(0xc45988)));
                wr_s16(TARGET_MARK+2,(int16_t)(point.y-rd_u16(0xc458d8)));
            }
            if(first && rd_s16(frame-0x62)>1 && !first_visible(output)) return 0;
            output+=6;
        } else if(--extra<=0) return 1;
        first=0;
    }
}
static void flat_extensions(gaddr *input,gaddr *output,gaddr frame) {
    /* C07846 pairs each new point with the last initial edge's displacement.
     * Nonnegative separator supplies two new edge endpoints, then repeats. */
    for(;;) {
        int16_t count=word(input),edge[3];
        if(count<0) return;
        for(int k=0;k<3;++k) edge[k]=(int16_t)(rd_s16(*output-6+2*k)-rd_s16(*output-12+2*k));
        do {
            transform_vertex(*input,*output,frame,1); *input+=4;
            for(int k=0;k<3;++k) wr_s16(*output+6+2*k,(int16_t)(rd_s16(*output+2*k)+edge[k]));
            *output+=12;
        } while(--count>0);
        if(word(input)<0) return;
        for(int i=0;i<2;++i) {
            transform_vertex(*input,*output,frame,1); *input+=4; *output+=6;
        }
    }
}
static int ground_face(gaddr *stream,unsigned variant) {
    int count=word(stream);gaddr input=WORKSPACES;
    if(variant==0x68) input+=(gaddr)(int32_t)word(stream);
    wr_u16(CLIP_INPUT,0);wr_s16(CLIP_INPUT+2,(int16_t)count);
    uint16_t depths=0xffff;
    int copied=variant==0x68?count+1:count;
    for(int i=0;i<copied;++i) {
        gaddr point=variant==0x60?WORKSPACES+(gaddr)(int32_t)word(stream):input+6*i;
        wr_u32(CLIP_INPUT+4+6*i,rd_u32(point));wr_u16(CLIP_INPUT+8+6*i,rd_u16(point+4));
        depths&=rd_u16(point+4);
    }
    if(variant!=0x68) wr_s16(CURRENT_COLOUR,word(stream));
    if((int16_t)depths<0) return 0;
    if(variant==0x68) {
        wr_u16(CURRENT_COLOUR,7);wr_u16(LINE_STYLE,3);wr_u16(LINE_COLOUR,3);
    }
    int drawn=clip_and_draw_polygon();
    if(variant==0x68) {wr_u16(LINE_STYLE,15);wr_u16(LINE_COLOUR,0xffff);}
    return drawn;
}
static int command(uint16_t code,gaddr *stream,gaddr frame) {
    unsigned index=code&0x3fffu;
    int16_t eye_x=rd_s16(frame-0x26),eye_z=rd_s16(frame-0x22);
    /* Source command directory C1FCE8; IDs are data offsets, not CPU opcodes. */
    switch(index) {
    case 0x000: return 0;
    case 0x004: case 0x0e8: {
        gaddr point=WORKSPACES+(gaddr)(int32_t)word(stream);
        wr_s16(CURRENT_COLOUR,word(stream));
        return project_view_point_mode(rd_s16(point),rd_s16(point+2),rd_s16(point+4),
            index==4?rd_s16(BOUND_SHIFT):-2,rd_s16(frame-0x28),0);
    }
    case 0x008: return draw_selected_segment_clipped(stream);
    case 0x00c: return draw_tested_face(stream,frame);
    case 0x010: return draw_record_shadow(stream,frame);
    case 0x018: return draw_interpolated_segments(stream,frame); /* C206E4. */
    case 0x01c: return draw_side_face(stream);
    case 0x020: return draw_side_triangle(stream);
    case 0x024: return draw_parallelogram_face_near(stream);
    case 0x028: return draw_block_face(stream);
    case 0x02c: return draw_segment_run(stream);
    case 0x030: return draw_offset_run(stream);
    case 0x034: return draw_segment_pairs(stream);
    case 0x038: return draw_offset_segments(stream);
    case 0x03c: return draw_offset_face(stream);
    case 0x040: return draw_mixed_face(stream);
    case 0x044: case 0x048: return draw_quad_list(stream);
    case 0x04c: return draw_segment_lattice(stream);
    case 0x050: return draw_face_lattice(stream);
    case 0x054: return draw_face_grid(stream);
    case 0x058: return draw_segment_grid(stream);
    case 0x05c: return draw_quad_strip(stream);
    case 0x060: case 0x064: case 0x068: return ground_face(stream,index);
    case 0x06c: return draw_parallelogram_face_2(stream);
    case 0x078: return draw_parallelogram_face(stream);
    case 0x07c: return edge_alignment_test(stream,eye_x,eye_z,rd_s16(frame-0x28));
    case 0x080: return draw_face_lattice_plain(stream);
    case 0x084:
        /* C207FE: carrier hull latches the current model's flagged-view
         * shadow state. It consumes no stream words and returns zero. */
        if(!rd_u8(CONTEXT_SELECT) && viewed_record_flagged()) wr_u16(frame-0x7a,1);
        return 0;
    case 0x088: extend_parallelograms_scaled(stream); return 0;
    case 0x08c: extend_parallelograms(stream); return 0;
    case 0x090: extend_six_point_block_scaled(stream); return 0; /* C20F78. */
    case 0x094: extend_six_point_block(stream); return 0; /* C20FC4. */
    case 0x098: derive_workspace_extensions(); return 0;
    case 0x09c: return draw_square_faces(stream);
    case 0x0a0: return draw_split_square(stream);
    case 0x0a4: extend_block_scaled(stream); return 0;
    case 0x0a8: *stream=derive_extended_shown_vertices(*stream); return 0;
    case 0x0ac: *stream=derive_compact_shown_vertices(*stream); return 0;
    case 0x0b0: split_record_and_stream_edges(stream); return 0;
    case 0x0b4: derive_workspace_midpoint_extensions(); return 0;
    case 0x0c0: return draw_selected_segment(stream);
    case 0x0c4: {
        /* C1FF46: project the first face vertex when its side test rejects. */
        for(int i=0;i<3;++i) {
            gaddr point=WORKSPACES+(gaddr)(int32_t)word(stream);
            wr_u32(CLIP_INPUT+4+6*i,rd_u32(point));
            wr_u16(CLIP_INPUT+8+6*i,rd_u16(point+4));
        }
        uint16_t kind=(uint16_t)word(stream);int16_t eye[3];
        for(int k=0;k<3;++k) eye[k]=rd_s16(frame-0x26+2*k);
        if(face_test_passes(kind,rd_u32(frame-0x2c),stream,eye)) return 0;
        wr_u16(CURRENT_COLOUR,kind);
        return project_view_point_mode(rd_s16(CLIP_INPUT+4),rd_s16(CLIP_INPUT+6),
            rd_s16(CLIP_INPUT+8),rd_s16(BOUND_SHIFT),rd_s16(frame-0x28),0);
    }
    case 0x0c8: *stream=skip_to_type_block(*stream); return 0;
    case 0x0cc: *stream=skip_if_shown_record_flag(*stream); return 0;
    case 0x0d0: *stream=derive_edge_vertices(*stream); return 0;
    case 0x0d4: offset_block_copies(stream); return 0;
    case 0x0d8: *stream=skip_for_low_class(*stream); return 0;
    case 0x0dc: return draw_indexed_face_list(stream,frame);
    case 0x0e0: derive_shown_reflected_vertices(); return 0;
    case 0x0e4: return draw_face_grid_plain(stream);
    case 0x0ec: case 0x0f0: case 0x0f4: *stream=derive_shown_vertices(*stream); return 0;
    case 0x0f8: {
        draw_scaled_stream_circle(*stream,rd_s16(frame-8));*stream+=6;
        /* C2EC82 clears its result but sets N with the -1 pair store.
         * C1F944 consequently ends this command sequence on rejection. */
        return rd_u32(PROJECTED_PAIR)==0xffffffffu?-1:1;
    }
    case 0x0fc: derive_shown_parallelogram_vertices(); return 0;
    case 0x100: derive_shown_midpoint_vertices(); return 0;
    case 0x104: derive_shown_translated_vertices(); return 0;
    case 0x108: *stream=skip_for_type_3_to_6(*stream); return 0;
    case 0x10c: return test_stream_face_accumulation(stream,frame);
    case 0x110: *stream=skip_stream_records(*stream); return 0;
    case 0x114: return draw_tested_parallelogram(stream,frame);
    case 0x118: *stream=select_workspace_script_block(*stream); return 0;
    case 0x11c: return draw_stream_circles(stream,frame);
    case 0x120:
        /* C1FEE4: skip 18-byte entries using the source clock's low nibble.
         * The initial DBRA tests before advancing, so zero skips nothing. */
        *stream+=18u*(rd_u16(STREAM_SKIP)&15u); return 0;
    case 0x124: *stream=skip_word_for_mode_57(*stream); return 0;
    case 0x128: return draw_selected_segment_near(stream);
    case 0x12c: return draw_segment_pairs_near(stream);
    case 0x130: *stream+=2; return 0;
    case 0x134: *stream=skip_counted_entries(*stream); return 0;
    default:
        fprintf(stderr,"native model stream %06X parameters %06X record %04X code %04X\n",
            *stream-2,rd_u32(frame-0x2c),rd_u16(SCRIPT_RECORD),code);
        missing("draw command",index); return 0;
    }
}
static int sequence(gaddr stream,gaddr frame) {
    int drawn=0;
    for(;;) {
        int16_t code=word(&stream);
        if(code>=0) {
            if(code!=0x7fff) {
                if(shift_word(code,rd_s16(BOUND_SHIFT))<rd_s16(frame-0x28)) {
                    code=word(&stream);
                    if(code<0 || shift_word(code,rd_s16(BOUND_SHIFT))<rd_s16(frame-0x28)) return drawn;
                } else if(word(&stream)>=0) return drawn;
            }
            int16_t count=word(&stream);
            if(count>0) {
                int16_t first=word(&stream); transform_bound_points(count,first,frame);
            }
            code=word(&stream);
        } else if(code==-1) missing("source invalid-command trap",stream-2);
        int result=command((uint16_t)code,&stream,frame);
        if(result<0) return drawn;
        drawn|=result;
        wr_u16(frame-0x7c,(uint16_t)(rd_u16(frame-0x7c)|result));
        if(code&0x4000) return drawn;
    }
}
static int control(gaddr parameters,gaddr frame) {
    gaddr stream=rd_u32(CONTROL_STREAM);
    int drawn=0;
    wr_u16(frame-0x7c,0); /* C1F712; early expiry retains the preceding value. */
    wr_u32(LINE_STYLE,0x000fffffu); wr_u32(POLY_COMPLEMENT,0);
    for(;;) {
        uint16_t code=(uint16_t)word(&stream);
        if(!(code&0x8000)) {
            uint16_t kind=(uint16_t)word(&stream);
            int toward=0; int16_t eye[3];
            for(int k=0;k<3;++k) eye[k]=rd_s16(frame-0x26+2*k);
            if((kind&0xfc00u)!=0xfc00u) {
                if(kind&0x0c00) toward=component_beyond_bound(kind,(int16_t)code);
                else if(kind&0x2000) {
                    int16_t point[3],normal[3];
                    for(int k=0;k<3;++k) point[k]=word(&stream);
                    for(int k=0;k<3;++k) normal[k]=word(&stream);
                    toward=point_toward_eye(point,normal,eye);
                } else {
                    int16_t offsets[3]={(int16_t)code,word(&stream),word(&stream)};
                    for(int i=0;i<3;++i) for(int k=0;k<3;++k)
                        wr_u16(CLIP_INPUT+4+6*i+2*k,rd_u16(WORKSPACES+(gaddr)(int32_t)offsets[i]+2*k));
                    toward=face_toward_eye(kind,parameters,&stream,eye);
                }
            }
            stream=parameters+(gaddr)(int32_t)rd_s16(stream+(toward?2:0));
            continue;
        }
        if(code==0xffff) return 0;
        wr_u16(frame-0x7a,0); /* C1F7AA: per-surface flagged-view latch. */
        if(code&0x2000) {
            gaddr commands;
            if(code&0x1000) {commands=rd_u32(stream);stream+=4;}
            else commands=parameters+(code&0xfff);
            drawn|=sequence(commands,frame);
            /* C1F7FA tests this before the single-stream completion gate.
             * C207FE continues with the next carrier/cockpit surface. */
            if(rd_u16(frame-0x7a)) continue;
            break;
        }
        gaddr list;
        if(code&0x1000) {list=rd_u32(stream);stream+=4;}
        else list=parameters+(code&0xfff);
        for(;;) {
            uint16_t entry=(uint16_t)word(&list); gaddr commands;
            if(entry&0x1000) {commands=rd_u32(list);list+=4;}
            else commands=parameters+(entry&0xfff);
            drawn|=sequence(commands,frame);
            if(rd_u16(frame-0x7a)) break;
            if(entry&0x8000) break;
        }
        /* C1F838 returns to C1F716, whose whole-list completion latch
         * remains set by C1F7DA even when C207FE ends the current surface. */
        if(rd_u16(frame-0x7a) && !(code&0x4000)) continue;
        if(code&0x4000) break;
    }
    if(!(rd_u8(HEADER_BYTE)&0x40) && (rd_u8(HEADER_BYTE)&0x10)) {
        gaddr record=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(SCRIPT_RECORD);
        if((rd_u16(record)&0x400) && rd_s16(record+0x4c)>=0 && rd_s16(record+0x4c)<16)
            record_finish(record,frame,rd_s16(record+0x4c));
        else {HistoryProjectionWork history={0};draw_history_projection(&history);}
    }
    return (int16_t)drawn;
}
static int draw_model(gaddr parameters,gaddr frame,uint16_t camera_flags) {
    int16_t range=scaled_range(); gaddr stream=rd_u32(CONTROL_STREAM),bound;
    uint16_t code; int bound_shift=rd_s16(BOUND_SHIFT);
    wr_u16(frame-0x80,camera_flags); wr_u16(frame-0x96,rd_u16(MAGNITUDE));
    wr_s16(frame-0x28,range); wr_u32(frame-0x2c,parameters);
    for(;;) {
        code=(uint16_t)word(&stream);
        if(code&0x8000) break;
        if(shift_word((int16_t)(code&0x3fff),bound_shift)<=range) {
            if(code&0x4000) {stream+=4;continue;}
            int16_t offset=rd_s16(stream); if(offset<0) return 0;
            stream=parameters+(gaddr)(int32_t)offset;
        } else {
            if(!(code&0x4000)) stream+=2;
            code=(uint16_t)word(&stream); break;
        }
    }
    if(code==0xffff) return 0;
    if(code&0x2000) {if(rd_u8(CELL_CHECKS)) return 1;}
    else if(!rd_u8(CELL_CHECKS)) {
        if(rd_s16(ZOOM_SCALE)>=0x80) return 1;
        for(gaddr p=0xc4f6ca;rd_s16(p)>=0;p+=24)
            if((rd_u16(p)>>8)==rd_u16(0xc459b4)) return 1;
    }
    int reject_first=!(code&0x4000);
    if(reject_first && rd_u8(0xc4579e) && !(rd_u16(0xc458da)&1)) return 0;
    wr_s16(frame-0x62,(int16_t)reject_first);
    bound=parameters+(code&0xfff); wr_u32(BOUND_RECORD,bound);wr_u32(CONTROL_STREAM,stream);
    wr_u8(frame-0x88,(uint8_t)rd_u16(bound));
    for(int k=0;k<3;++k) {
        int32_t origin=shift_long(rd_s32(TARGET_POINT+4*k),bound_shift);
        wr_s32(frame-0x20+4*k,origin);wr_s16(frame-0x86+2*k,(int16_t)(origin>>8));
        wr_s16(frame-0x26+2*k,(int16_t)-(int16_t)(origin>>8));
    }
    int low=rd_u8(bound+6)&15,high=rd_u8(bound+6)>>4,extra=0,difference=low;
    if(shift_word(rd_s16(bound+4),bound_shift)<rd_s16(MAGNITUDE)) {extra=low;difference=0;}
    else if(shift_word(rd_s16(bound+2),bound_shift)<rd_s16(MAGNITUDE)) {extra=high;difference=low-high;}
    wr_s16(frame-4,(int16_t)low);wr_s16(frame-6,(int16_t)difference);
    wr_s16(frame-8,(int16_t)(extra+bound_shift));
    uint8_t flags=rd_u8(bound+7);wr_u8(frame-0x64,flags);
    if(rd_s32(PROJECTION_Y)>-0x140 && !rd_u8(ATTITUDE_NEAR) && (flags&0x42)==0x42 &&
       (rd_s16(DISTANCE_GATE)<=0 || !(rd_u8(VISIT_CLOCK)&3)) &&
       edge_alignment(bound+10,rd_s16(frame-0x26),rd_s16(frame-0x22),range)<0) return 0;
    wr_u16(0xc4bf90,0);
    if(flags&1) {
        int vertices=record_vertices(bound,frame);
        if(vertices<=0) return vertices<0?rd_s16(frame-0x7c):0;
        return control(parameters,frame);
    }
    int flat=!!(flags&2),count=rd_s8(bound+8);
    gaddr input=bound+10,output=WORKSPACES;
    if(flat) {
        if(rd_s32(PROJECTION_Y)>=-0x20 && shift_word((int16_t)((flags&4)?0x170:0x60),bound_shift)<range) return 0;
        int32_t y=(int32_t)(rd_u32(frame-0x1c)+rd_u32(SHADOW_OFFSET_Y));
        int16_t height=(int16_t)shift_long(y,8-difference);
        for(int row=0;row<3;++row) wr_s16(frame-0x78+2*row,(int16_t)(((int32_t)height*rd_s16(VIEW_ANGLE_MATRIX+6*row+2))>>8));
    }
    do {
        transform_vertex(input,output,frame,flat);input+=flat?4:6;output+=6;
        if(output==WORKSPACES+6 && reject_first && !first_visible(WORKSPACES)) return 0;
    } while(--count>0);
    if(flat && (flags&4) && rd_s8(bound+8)>1) flat_extensions(&input,&output,frame);
    if(!flat && (flags&8)) {
        int16_t origin[3];
        for(int k=0;k<3;++k) {
            int32_t displacement=(int32_t)(rd_u32(frame-0x20+4*k)+rd_u32(SHADOW_OFFSET_X+4*k));
            origin[k]=(int16_t)shift_long(displacement,8-difference);
        }
        transform_model_strips(input,output,origin,rd_s16(frame-8));
    }
    return control(parameters,frame);
}
int native_model_draw(gaddr parameters,gaddr frame) {return draw_model(parameters,frame,0);}
static int aircraft_descriptor(gaddr parameters,gaddr frame) {
    /* C1ED4C: selected-view and alternate stream gates, including cockpit view. */
    if(rd_u8(HEADER_BYTE)&0x40) return 0;
    if(!rd_u8(ORIGIN_ENABLE) && rd_u16(SCRIPT_RECORD)==rd_u16(0xc458de)) {
        int camera=rd_s8(0xc457a7);
        if(camera>=3 && camera<=9) return draw_model(parameters,frame,1);
        if(rd_u32(0xc45a3a)!=rd_u32(CONTROL_STREAM)) {
            wr_u32(CONTROL_STREAM,rd_u32(0xc45a3a));
            if(!rd_u8(0xc45835)) {
                unsigned mode=rd_u8(0xc4586b);
                if(!mode || (mode<3 && shift_long(rd_s32(PROJECTION_Y),rd_s16(BOUND_SHIFT))<-0x20)) {
                    gaddr stream=rd_u32(CONTROL_STREAM);int16_t selection=rd_s16(stream);
                    if(selection<0) {if(selection==-1) return 0;stream+=4;}
                    else if(selection&0x4000) stream+=6;
                    else {int16_t offset=rd_s16(stream+2);if(offset<0) return 0;stream+=(gaddr)(int32_t)offset;}
                    wr_u32(CONTROL_STREAM,stream);
                }
            }
        }
    }
    return native_model_draw(parameters,frame);
}

static int ground_draw(gaddr frame,int32_t minimum_height) {
    /* C096CA: separate absolute ground descriptor and command stream. */
    if(rd_s32(TARGET_POINT+4)<minimum_height) return 0;
    wr_u32(LINE_STYLE,0x000fffffu);wr_s16(frame-0x96,rd_s16(MAGNITUDE));
    wr_s16(frame-0x28,scaled_range());
    gaddr stream=rd_u32(CONTROL_STREAM);int16_t code;
    for(;;) {
        code=word(&stream);if(code<0) break;
        if(shift_word(code,rd_s16(BOUND_SHIFT))>=rd_s16(frame-0x28)) {stream+=4;break;}
        gaddr next=rd_u32(stream);wr_u32(CONTROL_STREAM,next);
        if((int32_t)next<0) return 0;stream=next;
    }
    if(code==-1) return 0;
    gaddr bound=rd_u32(stream);stream+=4;
    wr_u32(BOUND_RECORD,bound);wr_u32(CONTROL_STREAM,stream);
    int shift=rd_u8(HEADER_BYTE)&15;wr_s16(frame-8,(int16_t)shift);
    int16_t origin[3]={shift_word(rd_s16(0xc45a72),shift),
        (int16_t)shift_long(rd_s32(TARGET_POINT+4)>>8,shift),shift_word(rd_s16(0xc45a76),shift)};
    for(int k=0;k<3;++k) wr_s16(frame-0x86+2*k,origin[k]);
    wr_u16(0xc4bf90,0);
    for(int row=0;row<3;++row) wr_s16(frame-0x78+2*row,(int16_t)(((int32_t)origin[1]*rd_s16(VIEW_ANGLE_MATRIX+6*row+2))>>8));
    int16_t offset[6]={origin[0],origin[1],origin[2],
        rd_s16(frame-0x78),rd_s16(frame-0x76),rd_s16(frame-0x74)};
    /* transform_ground_points adds the placement displacement itself. */
    transform_ground_points(bound+6,rd_s16(bound),(int16_t)shift,offset,WORKSPACES);
    /* C09812/C09850 share the later model's retained -$7C word.
     * C1F074 expiry can consume it before C1F712 initializes a model. */
    wr_u16(frame-0x7c,0);
    int drawn=0;
    for(;;) {
        code=word(&stream);if(code==-1) return drawn;
        if(code>=0) {
            if(code!=0x7fff) {
                if(shift_word(code,rd_s16(BOUND_SHIFT))<rd_s16(frame-0x28)) {
                    code=word(&stream);
                    if(code<0 || shift_word(code,rd_s16(BOUND_SHIFT))<rd_s16(frame-0x28)) return drawn;
                    stream=rd_u32(stream);
                }
                if(code!=0x7fff && word(&stream)>=0) stream+=2;
            }
            int16_t count=word(&stream);
            if(count) {
                int16_t first=word(&stream);
                transform_ground_points(bound+6+(gaddr)(int32_t)first,count,(int16_t)shift,offset,
                    WORKSPACES+(gaddr)(int32_t)(int16_t)(first+(first>>1)));
            }
            code=word(&stream);
        }
        int result=command((uint16_t)code,&stream,frame);if(result<0) return 0;
        drawn|=result;
        wr_u16(frame-0x7c,(uint16_t)(rd_u16(frame-0x7c)|result));
        if(code&0x4000) return drawn;
    }
}
static ContextPublicationResult expiry_selection_child(void *context,enum ContextPublicationChild child) {
    (void)context;
    if(child!=CONTEXT_PUBLISH_SELECTION_TONE) {
        fprintf(stderr,"native expiry selection child unavailable: %u\n",(unsigned)child); abort();
    }
    /* Despite the old hook name, C09DEA calls C25704: post TARGET DESTROYED,
     * not an audio tone. C11BFC later owns the message's sound and timing. */
    post_message(0x4016);
    return (ContextPublicationResult){0};
}
static int32_t scene_placement(void *context,const ScenePlacementCall *call) {
    NativeFrontend *game=context;
    if(game) ++game->model_calls;
    if(call->routine==0xc1ee14 || call->routine==0xc1ed48) return native_model_draw(call->parameters,NATIVE_SCENE_MODEL_FRAME);
    if(call->routine==0xc1ed4c) return aircraft_descriptor(call->parameters,NATIVE_SCENE_MODEL_FRAME);
    if(call->routine==0xc22b1a) {
        /* C22B1A selects the action record's model by its source lifetime;
         * every arm joins C22AFE/C1ED4C with A0 fixed at C3C6E0. */
        const gaddr record=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(SCRIPT_RECORD);
        const int16_t lifetime=rd_s16(record+0x4c);
        const gaddr stream=lifetime<0?0xc3c71cu:lifetime==0?0xc3c720u:
            lifetime<=2?0xc3c70eu:lifetime<5?0xc3c700u:0xc3c6e0u;
        wr_u32(CONTROL_STREAM,stream);wr_u32(0xc45a3au,stream);
        return aircraft_descriptor(0xc3c6e0u,NATIVE_SCENE_MODEL_FRAME);
    }
    if(call->routine==0xc22bba) {
        /* C22BBA-C22C6C: the released record's lifetime and action nibble
         * choose the original disk-backed stream before C22AFE/C1ED4C. */
        const gaddr record=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(SCRIPT_RECORD);
        const int16_t lifetime=rd_s16(record+0x4c);
        gaddr stream;
        if(lifetime>0) {
            wr_u8(record+0x7c,rd_u8(record+0x7c)|1);
            stream=lifetime>=5?0xc3c986u:lifetime<=2?0xc3c9b4u:0xc3c9a6u;
        } else stream=lifetime==0 && (rd_u8(record+0x7c)&15)?0xc3c982u:0xc3c9c2u;
        wr_u32(CONTROL_STREAM,stream);wr_u32(0xc45a3au,stream);
        return aircraft_descriptor(0xc3c986u,NATIVE_SCENE_MODEL_FRAME);
    }
    if(call->routine==0xc22ac0) {
        gaddr record=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(SCRIPT_RECORD);
        uint16_t flags=rd_u16(record);
        if(flags&0x40) {
            if(flags&0x200) {
                /* C22ADE-C22AFC. C22C70 is an empty render hook. Preserve
                 * the parameters while C09DD0 clears only this selection. */
                const ContextPublicationHooks hooks={.consume=expiry_selection_child};
                wr_u16(record+0x4c,15);
                wr_u16(record,(flags&0xfdffu)|0x400u);
                clear_matching_record_selection(&hooks);
            }
            return aircraft_descriptor(call->parameters,NATIVE_SCENE_MODEL_FRAME);
        }
        if(rd_u8(record+0x7a)!=5 && rd_s16(record+0x4c)<0) wr_u16(record,flags|0x40);
        /* C22B04-C22B18 touches only flags: retain the caller's actual
         * shift/refresh phase (or selected-position calculation). */
        return call->prior_result;
    }
    if(call->routine==0xc1ed3c) {
        if(rd_s32(TARGET_POINT+4)>=-0x280000) return 0;
        return native_model_draw(call->parameters,NATIVE_SCENE_MODEL_FRAME);
    }
    if(call->routine==0xc096ca) return ground_draw(NATIVE_SCENE_MODEL_FRAME,-0x800000);
    if(call->routine==0xc096bc) return ground_draw(NATIVE_SCENE_MODEL_FRAME,-0x380000);
    missing("placement descriptor",call->routine);return 0;
}

int32_t native_scene_placement(void *context,const ScenePlacementCall *call) {
    /* Only an actual model caller materialises the retained value in its
     * ordinary native scratch frame. Startup updates publish host state. */
    wr_u16(NATIVE_SCENE_MODEL_FRAME-0x7c,native_model_retained_result());
    const int32_t result=scene_placement(context,call);
    native_model_retain_result(rd_u16(NATIVE_SCENE_MODEL_FRAME-0x7c));
    return result;
}
