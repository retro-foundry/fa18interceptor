/* Original-byte validation only; production uses no CPU/ROM/guest RAM. */
#ifndef FA18_SCENE_SOURCE_HEADER
#define FA18_SCENE_SOURCE_HEADER "../../build/recomp/native_context_publication_source.h"
#endif
#define FA18_SCENE_PLAYER_HELPERS_ONLY
#include "native_scene_player_oracle.c"
#include "../../port/view_command_controls.c"
#include "../../port/view_command_input.c"
#include "../../port/native_context_publication.c"
#define VIEW_BYTES(X) \
 X(detail_index,ORIGIN_DETAIL_INDEX) X(update_mask,UPDATE_MASK) X(fire_state,FIRE_STATE) \
 X(mode,VIEW_MODE) X(mode_auxiliary,0xc457a8u) X(mode_companion,0xc457a9u) \
 X(refresh_request,0xc45891u) X(zoom_flags,ZOOM_FLAGS) \
 X(redraw_first,REDRAW_FIRST) X(gauge_refresh,GAUGE_REFRESH) \
 X(grid_z_redraws,GRID_Z_REDRAWS) X(grid_x_redraws,GRID_X_REDRAWS) \
 X(redraw_bar_a,BAR_REDRAWS_A) X(redraw_bar_aux,0xc45841u) \
 X(display_update,DISPLAY_UPDATE) X(redraw_keep_state,REDRAW_KEEP_STATE)
#define VIEW_WORDS(X) \
 X(target_mark,TARGET_MARK) X(span_origin,SPAN_ORIGIN) X(span_origin_y,SPAN_ORIGIN_Y) \
 X(line_last_row,LINE_LAST_ROW) X(zoom_scale,ZOOM_SCALE) \
 X(redraw_state_word,REDRAW_STATE_WORD) X(emitted_requests,PENDING_COMMAND_WORD_B)
#define VIEW_LONGS(X) X(origin_middle,SELECTOR_ORIGIN_MIDDLE) X(redraw_state_long,REDRAW_STATE_LONG)
#define FLIGHT_BYTES(X) \
 X(redraw_b,BAR_REDRAWS_B) X(redraw_c,BAR_REDRAWS_C) X(redraw_d,BAR_REDRAWS_D) \
 X(scale_redraws,SCALE_REDRAWS) X(info_redraws,INFO_REDRAWS) \
 X(weapon_mode_redraws,COMMAND_WEAPON_MODE_REDRAWS) X(weapon_redraws,WEAPON_REDRAWS) \
 X(pause,PAUSE_A)

static void load_publication(SceneState *s) {
#define FIELD(n,a) s->view.n=rd_u8(a);
    VIEW_BYTES(FIELD)
#undef FIELD
#define FIELD(n,a) s->view.n=rd_u16(a);
    VIEW_WORDS(FIELD)
#undef FIELD
#define FIELD(n,a) s->view.n=rd_u32(a);
    VIEW_LONGS(FIELD)
#undef FIELD
#define FIELD(n,a) s->flight.n=rd_u8(a);
    FLIGHT_BYTES(FIELD)
#undef FIELD
    s->context.angle_history=rd_u16(ORIGIN_ANGLE_HISTORY);
    s->commands.modifier=rd_u8(KEY_STATE);
    s->commands.indexed.function_modifier=rd_u8(KEY_STATE+1);
    s->commands.other_modifier=rd_u8(KEY_STATE+2);
    s->flight.viewed=s->bank.aircraft+rd_u16(VIEW_RECORD)/512;
}
static void store_publication(const SceneState *s) {
    store_scene(s);
#define FIELD(n,a) wr_u8(a,s->view.n);
    VIEW_BYTES(FIELD)
#undef FIELD
#define FIELD(n,a) wr_u16(a,s->view.n);
    VIEW_WORDS(FIELD)
#undef FIELD
#define FIELD(n,a) wr_u32(a,s->view.n);
    VIEW_LONGS(FIELD)
#undef FIELD
#define FIELD(n,a) wr_u8(a,s->flight.n);
    FLIGHT_BYTES(FIELD)
#undef FIELD
    wr_u16(ORIGIN_ANGLE_HISTORY,s->context.angle_history);
    wr_u8(KEY_STATE,s->commands.modifier);
    wr_u8(KEY_STATE+1,s->commands.indexed.function_modifier);
    wr_u8(KEY_STATE+2,s->commands.other_modifier);
    wr_u16(VIEW_RECORD,(uint16_t)((s->flight.viewed-s->bank.aircraft)*512));
}
#ifndef FA18_PUBLICATION_HELPERS_ONLY
static int original_publication_graph(void) {
    unsigned step,i;
    for(step=0;step<1000;++step) {
        uint32_t pc=REG_PC; uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            if(scene_source_bytes[i].pc==pc) break;
        if(i==sizeof scene_source_bytes/sizeof scene_source_bytes[0]) {
            fprintf(stderr,"unexpected publication PC %06X\n",pc); return 0;
        }
        scene_seen[i]=1; opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    return 0;
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0; char error[256];
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    SceneState *native=malloc(sizeof *native); FA18ViewSpanOffsets spans;
    FA18NativeContextPublication p; uint16_t marker,target;
    static const uint32_t entries[]={0xc1bee8,0xc1ba86,0xc1b906};
    static const uint8_t counts[]={0,1,9,10,0x7f,0x80,0xff};
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,entry,scenario,i,j;
    if(!state || !rom || !m || !base || !before || !expected || !native || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(entry=0;entry<3;++entry) for(scenario=0;scenario<cases;++scenario) {
        uint32_t event,axis,source_event,source_axis; int16_t index; int ok;
        unsigned slot=scenario%16;
        memcpy(m,base,sizeof *m); selected_entry=entries[entry]; scene_fixture(scenario);
        for(i=0;i<FA18_COMMAND_QUEUE_NEIGHBORS;++i) wr_u8(KEY_RAW-128+i,(uint8_t)random_value());
        REG_D[0]=random_value(); REG_D[4]=random_value();
        REG_D[0]=(REG_D[0]&0xffffff00u)|(scenario&255u);
        index=(int16_t)(slot+(scenario%3==0?128:scenario%3==1?-128:0));
        REG_D[1]=(REG_D[1]&0xffff0000u)|(uint16_t)index;
        wr_u16(VIEW_RECORD,(uint16_t)((scenario/16%16)*512));
        wr_u8(KEY_TAKEN,scenario&256?1:0); wr_u8(KEY_COUNT,counts[scenario/512%7]);
        wr_u8(KEY_WRITE,(uint8_t)(scenario/32)); wr_u8(KEY_TRANSLATED_WRITE,(uint8_t)(scenario/16));
        wr_u8(CONTEXT_SELECT,scenario%3==0?1:0); wr_u8(CONTEXT_PUBLISH_RETURN_MODE,(uint8_t)random_value());
        for(i=0;i<16;++i) {
            wr_u16(CONTROL_RECORDS+512*i+0x68,(uint16_t)random_value());
            wr_u8(CONTROL_RECORDS+512*i+0x62,(uint8_t)((scenario%3==1?0x30:0x20)|(i&15)));
        }
        wr_u8(VIEW_MODE,(uint8_t)(scenario/7));
        wr_u8(REDRAW_KEEP_STATE,(uint8_t)(scenario&1));
        for(i=0;i<3;++i) wr_u8(KEY_STATE+i,(uint8_t)random_value());
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            for(j=0;j<scene_source_bytes[i].length;++j)
                if(rd_u8(scene_source_bytes[i].pc+j)!=scene_source_bytes[i].bytes[j]) return 1;
        if(!load_scene(native) || !verify_record_owners(native)) return 1;
        load_publication(native);
        for(i=0;i<256;++i) spans.values[i]=rd_s8(0xc1bad4u-128+i);
        marker=rd_u16(SELECTION_MARKER); target=rd_u16(TARGET_RECORD);
        p=(FA18NativeContextPublication){&native->bank,&native->context,&native->queue,&spans,&marker,&target};
        event=REG_D[0]; axis=REG_D[4]; memcpy(before,m,sizeof *m);
        if(!original_publication_graph()) return 1;
        source_event=REG_D[0]; source_axis=REG_D[4];
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        ok=entry==0?fa18_publish_native_context_record(&p,event,index,&axis,&event):
            entry==1?fa18_publish_native_view_key(&p,event,&axis,&event):
                fa18_publish_native_zero_view(&p,event,&axis,&event);
        if(!ok || axis!=source_axis || event!=source_event) {
            fprintf(stderr,"publication %06X case %u event %08X/%08X axis %08X/%08X\n",selected_entry,scenario,source_event,event,source_axis,axis); return 1;
        }
        store_publication(native); wr_u16(SELECTION_MARKER,marker); wr_u16(TARGET_RECORD,target);
        if(memcmp(m->chip,expected,FA18_CHIP_SIZE) ||
           memcmp(m->slow,expected+FA18_CHIP_SIZE,0x7fd00) ||
           memcmp(m->slow+0x7ff00,expected+FA18_CHIP_SIZE+0x7ff00,FA18_SLOW_SIZE-0x7ff00)) {
            for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
                uint32_t a=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
                if(a>=0xc7fd00 && a<0xc7ff00) continue;
                if(rd_u8(a)!=expected[i]) {
                    fprintf(stderr,"publication %06X case %u RAM %06X source %02X native %02X\n",selected_entry,scenario,a,expected[i],rd_u8(a)); return 1;
                }
            }
        }
        /* Observe original typed owners before the exporter can mask an
         * incorrect association. Compare independently against source RAM. */
        memcpy(m->chip,expected,FA18_CHIP_SIZE); memcpy(m->slow,expected+FA18_CHIP_SIZE,FA18_SLOW_SIZE);
        if(!verify_record_owners(native) || native->context.angle_history!=rd_u16(ORIGIN_ANGLE_HISTORY) ||
           native->flight.viewed!=native->bank.aircraft+rd_u16(VIEW_RECORD)/512 ||
           marker!=rd_u16(SELECTION_MARKER) || target!=rd_u16(TARGET_RECORD)) return 1;
    }
    printf("native context publication: %u complete calls match all game RAM, typed records, viewed identity, event and carried axis; no child contracts\n",cases*3);
    printf("visited:"); for(i=0;i<sizeof scene_seen;++i) if(scene_seen[i]) printf(" %06X",scene_source_bytes[i].pc); putchar('\n');
    free(native); free(expected); free(before); free(base); free(m); return 0;
}

#endif
