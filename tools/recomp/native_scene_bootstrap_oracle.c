/* CPU/ROM/source RAM exist only in this validation oracle. Native owners are
 * compared independently around three explicitly contracted pending children. */
#define FA18_SCENE_PLAYER_HELPERS_ONLY
#define FA18_SCENE_SOURCE_HEADER "../../build/recomp/native_scene_bootstrap_source.h"
#include "native_scene_player_oracle.c"
#include "../../port/startup_ranges.c"
#include "../../port/renderer_clear.c"
#include "../../port/viewed_record_word.c"
#include "native_template_gate_fixture.h"
#define fa18_place_native_scene_root boot_place_direct
#include "../../port/scene_bootstrap_native.c"
#undef fa18_place_native_scene_root

enum { BOOT_PLANE_BYTES=8048, BOOT_RAM_BYTES=FA18_CHIP_SIZE+FA18_SLOW_SIZE };
typedef struct {
    SceneState scene;
    FA18NativeStartupRanges startup;
    FA18NativeViewedRecordWord viewed;
    FA18NativeGraphicsSetup graphics;
    FA18NativeGraphicsPlane planes[10];
    FA18NativeRendererClear renderer;
    FA18NativeScenePlacement placement;
    FA18NativeSceneBootstrap bootstrap;
    GateOracleState gates;
    uint8_t buffers[10][BOOT_PLANE_BYTES],fifth;
    uint8_t scene_limit,previous_limit,context_state,transition,previous_state,byte_be;
    uint16_t menu_return,word_4fda0,countdown,word_a6,word_a8,history,minimum,scales[2],depth[24];
    uint16_t words[52]; PortFieldByte fields[104];
    uint32_t message_queue,message_timer,valid[2],reference;
} BootstrapState;
static const uint32_t boot_children[]={0xc09266,0xc1c63e,0xc1c860};
static uint8_t boot_child_ram[3][BOOT_RAM_BYTES];
static unsigned boot_scenario,boot_source_calls,boot_native_calls;
static BootstrapState *boot_native_active;
static uint32_t boot_plane_address(unsigned i) { return 0x10000+0x2200*i; }
static void boot_snapshot(uint8_t *out) {
    memcpy(out,fa18_machine->chip,FA18_CHIP_SIZE);
    memcpy(out+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
}
static int boot_equal(const uint8_t *expected) {
    unsigned i;
    for(i=0;i<BOOT_RAM_BYTES;++i) {
        uint32_t address=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
        if(selected_entry!=0xc090c2 && address>=0xc7fd00 && address<0xc7ff00) continue;
        if(rd_u8(address)!=expected[i]) {
            fprintf(stderr,"bootstrap %06X case %u RAM %06X source %02X native %02X\n",
                    selected_entry,boot_scenario,address,expected[i],rd_u8(address)); return 0;
        }
    }
    return 1;
}
static void boot_store(BootstrapState *s) {
    unsigned i,j; uint8_t value;
    SceneState *p=&s->scene;
    gate_store(&s->gates);
    if(selected_entry==0xc1c40c) return;
    store_scene(p);
    for(i=0;i<104;++i) {
        if(!port_read_field_byte(s->fields+i,&value)) abort();
        wr_u8(0xc458c0+i,value);
    }
    for(i=0;i<10;++i) for(j=0;j<BOOT_PLANE_BYTES;++j) wr_u8(boot_plane_address(i)+j,s->buffers[i][j]);
    wr_u8(0xc458a7,s->scene_limit); wr_u8(0xc458a8,s->previous_limit);
    wr_u8(CONTEXT_STATE,s->context_state); wr_u8(0xc458be,s->byte_be);
    wr_u16(0xc4fda2,s->menu_return); wr_u16(0xc4fda0,s->word_4fda0);
    wr_u16(POST_INPUT_COUNTDOWN,s->countdown); wr_u16(0xc459a6,s->word_a6); wr_u16(0xc459a8,s->word_a8);
    wr_u16(HISTORY_RECORD,s->history); wr_u16(READOUT_MINIMUM,s->minimum);
    wr_u16(MATRIX_ROW_SCALES,s->scales[0]); wr_u16(MATRIX_ROW_SCALES+2,s->scales[1]);
    wr_u32(MESSAGE_QUEUE,s->message_queue); wr_u32(MESSAGE_TIMER,s->message_timer);
    wr_u32(READOUT_SOURCE_VALID,s->valid[0]); wr_u32(0xc45b02,s->valid[1]); wr_u32(REFERENCE_18,s->reference);
    wr_u32(0xc45664,p->context.map_middle_cache); wr_u32(ORIGIN_SMOOTHED_DELTA,p->context.smoothed_delta);
    wr_u32(ORIGIN_SMOOTHED_DELTA+4,p->context.auxiliary_delta[0]); wr_u32(ORIGIN_SMOOTHED_DELTA+8,p->context.auxiliary_delta[1]);
    wr_u16(VIEW_PAN,p->context.pan); wr_u16(VIEW_ROTATE,p->context.rotate);
    wr_u16(LINE_LAST_ROW,p->view.line_last_row); wr_u16(SPAN_ORIGIN,p->view.span_origin);
    wr_u16(SPAN_ORIGIN_Y,p->view.span_origin_y); wr_u16(ZOOM_SCALE,p->view.zoom_scale);
    for(i=0;i<24;++i) wr_u16(DEPTH_VALUES+2*i,s->depth[i]);
}
static int boot_load(BootstrapState *s) {
    unsigned i,j; SceneState *p=&s->scene;
    memset(s,0,sizeof *s);
    gate_load(&s->gates);
    if(selected_entry==0xc1c40c) return 1;
    if(!load_scene(p)) return 0;
    for(i=0;i<52;++i) {
        s->words[i]=rd_u16(0xc458c0+2*i);
        s->fields[2*i]=(PortFieldByte){.unsigned_word=s->words+i,.shift=8};
        s->fields[2*i+1]=(PortFieldByte){.unsigned_word=s->words+i};
    }
    for(i=0;i<10;++i) {
        for(j=0;j<BOOT_PLANE_BYTES;++j) s->buffers[i][j]=rd_u8(boot_plane_address(i)+j);
        s->planes[i]=(FA18NativeGraphicsPlane){s->buffers[i],BOOT_PLANE_BYTES,0};
        if(i<9) s->graphics.source[i]=s->planes+i;
    }
    s->renderer=(FA18NativeRendererClear){&s->graphics,s->planes+9,&s->fifth};
    s->scene_limit=rd_u8(0xc458a7); s->previous_limit=rd_u8(0xc458a8);
    s->context_state=rd_u8(CONTEXT_STATE); s->byte_be=rd_u8(0xc458be);
    s->menu_return=rd_u16(0xc4fda2); s->word_4fda0=rd_u16(0xc4fda0);
    s->countdown=rd_u16(POST_INPUT_COUNTDOWN); s->word_a6=rd_u16(0xc459a6); s->word_a8=rd_u16(0xc459a8);
    s->history=rd_u16(HISTORY_RECORD); s->minimum=rd_u16(READOUT_MINIMUM);
    s->scales[0]=rd_u16(MATRIX_ROW_SCALES); s->scales[1]=rd_u16(MATRIX_ROW_SCALES+2);
    s->message_queue=rd_u32(MESSAGE_QUEUE); s->message_timer=rd_u32(MESSAGE_TIMER);
    s->valid[0]=rd_u32(READOUT_SOURCE_VALID); s->valid[1]=rd_u32(0xc45b02); s->reference=rd_u32(REFERENCE_18);
    p->context.map_middle_cache=rd_u32(0xc45664); p->context.smoothed_delta=rd_u32(ORIGIN_SMOOTHED_DELTA);
    p->context.auxiliary_delta[0]=rd_u32(ORIGIN_SMOOTHED_DELTA+4);
    p->context.auxiliary_delta[1]=rd_u32(ORIGIN_SMOOTHED_DELTA+8);
    p->context.pan=rd_u16(VIEW_PAN); p->context.rotate=rd_u16(VIEW_ROTATE);
    p->view.line_last_row=rd_u16(LINE_LAST_ROW); p->view.span_origin=rd_u16(SPAN_ORIGIN);
    p->view.span_origin_y=rd_u16(SPAN_ORIGIN_Y); p->view.zoom_scale=rd_u16(ZOOM_SCALE);
    for(i=0;i<24;++i) s->depth[i]=rd_u16(DEPTH_VALUES+2*i);
    s->placement.player=&p->setup;
    s->bootstrap=(FA18NativeSceneBootstrap){.context=&p->context,.player=&p->setup,.placement=&s->placement,.startup=&s->startup,
        .viewed_word=&s->viewed,.renderer=&s->renderer,.gates=&s->gates.state,.scene_limit=&s->scene_limit,.previous_scene_limit=&s->previous_limit,
        .context_state=&s->context_state,.menu_transition=&s->transition,.previous_state_byte=&s->previous_state,.byte_458be=&s->byte_be,
        .menu_return_word=&s->menu_return,.word_4fda0=&s->word_4fda0,.countdown=&s->countdown,
        .word_459a6=&s->word_a6,.word_459a8=&s->word_a8,.history_record=&s->history,.readout_minimum=&s->minimum,
        .row_scales={s->scales,s->scales+1},.message_queue_first=&s->message_queue,.message_timer=&s->message_timer,
        .readout_valid={s->valid,s->valid+1},.reference_18=&s->reference,.depth_values=s->depth,.depth_count=24};
    return fa18_bind_native_scene_bootstrap(&s->bootstrap,&p->queue,s->fields,104);
}
/* Effects are generated independently for source and native. No child output
 * or entry data is copied from the source run into a native game owner. */
static void boot_child_effect(BootstrapState *s,unsigned child) {
    unsigned view_index=(boot_scenario+3*child+7)%16;
    uint8_t mode=(uint8_t)(boot_scenario+17*child),limit=(uint8_t)(boot_scenario^child^0xb7);
    uint16_t flags=(uint16_t)(0x1357^boot_scenario^child),count=(uint16_t)(0x3344+child);
    uint32_t origin=0x11223344+boot_scenario+child,delta=0x55667788^boot_scenario^child;
    if(s) {
        s->scene.flight.viewed=s->scene.bank.aircraft+view_index;
        s->scene.commands.message_state=mode; s->scene_limit=limit; s->countdown=count;
        s->scene.bank.aircraft[2].flags=flags; s->scene.context.origin_first=origin;
        s->scene.context.smoothed_delta=delta; s->depth[23]=(uint16_t)(child+0x9876);
        if(!child) {
            /* Mutate the actual bound asset bytes before the real gate call. */
            s->gates.streams[0x300]=0; s->gates.streams[0x301]=3; s->gates.stream_changed=1;
        }
    } else {
        wr_u16(VIEW_RECORD,(uint16_t)(512*view_index)); wr_u8(MESSAGE_STATE_C,mode);
        wr_u8(0xc458a7,limit); wr_u16(POST_INPUT_COUNTDOWN,count);
        wr_u16(CONTROL_RECORDS+2*512,flags); wr_u32(SELECTOR_ORIGIN,origin);
        wr_u32(ORIGIN_SMOOTHED_DELTA,delta); wr_u16(DEPTH_VALUES+46,(uint16_t)(child+0x9876));
        if(!child) wr_u16(TEMPLATE_SELECTOR_X+0x300,3);
    }
}
static int boot_child_native(BootstrapState *s,FA18NativeSceneBootstrap *parent,unsigned child) {
    if(parent!=&s->bootstrap || boot_native_calls++!=child || boot_source_calls!=3) return 0;
    boot_store(s);
    if(!boot_equal(boot_child_ram[child]) || !verify_record_owners(&s->scene)) return 0;
    boot_child_effect(s,child); return 1;
}
int boot_place_direct(FA18NativeScenePlacement *placement) {
    return boot_native_active && placement==&boot_native_active->placement &&
        boot_child_native(boot_native_active,&boot_native_active->bootstrap,0);
}
static int boot_update(void *context,FA18NativeSceneBootstrap *parent) { return boot_child_native(context,parent,1); }
static int boot_refresh(void *context,FA18NativeSceneBootstrap *parent) { return boot_child_native(context,parent,2); }
static void boot_fixture(unsigned scenario) {
    static const uint8_t gates[]={0,1,0x80,0xff}; unsigned i,j;
    boot_source_calls=boot_native_calls=0;
    if(selected_entry==0xc1c40c) {
        for(i=0;i<15;++i) REG_DA[i]=random_value();
        REG_A[7]=0xc7ff00; wr_u32(REG_A[7],0xc70000); REG_PC=selected_entry;
        m68k_set_reg(M68K_REG_SR,0x2700|(scenario&31)); SET_CYCLES(1000000000); fa18_next_event=INT64_MAX;
        gate_fixture(scenario,1); return;
    }
    scene_fixture(scenario);
    for(i=0;i<104;++i) wr_u8(0xc458c0+i,(uint8_t)random_value());
    wr_u16(VIEW_RECORD,(uint16_t)(512*(scenario%16)));
    for(i=0;i<10;++i) {
        wr_u32(RENDER_BUFFERS_A+4*i,boot_plane_address(i));
        for(j=0;j<BOOT_PLANE_BYTES;++j) wr_u8(boot_plane_address(i)+j,(uint8_t)random_value());
    }
    wr_u8(FIFTH_BUFFER_USED,gates[scenario%4]);
    wr_u8(0xc458a7,(uint8_t)random_value()); wr_u8(0xc458a8,(uint8_t)random_value());
    wr_u8(CONTEXT_STATE,(uint8_t)random_value()); wr_u8(0xc458be,(uint8_t)random_value());
    wr_u16(0xc4fda0,(uint16_t)random_value()); wr_u16(0xc4fda2,(uint16_t)random_value());
    wr_u16(POST_INPUT_COUNTDOWN,(uint16_t)random_value()); wr_u16(0xc459a6,(uint16_t)random_value()); wr_u16(0xc459a8,(uint16_t)random_value());
    wr_u16(HISTORY_RECORD,(uint16_t)random_value()); wr_u16(READOUT_MINIMUM,(uint16_t)random_value());
    wr_u32(MESSAGE_QUEUE,random_value()); wr_u32(MESSAGE_TIMER,random_value());
    wr_u32(READOUT_SOURCE_VALID,random_value()); wr_u32(0xc45b02,random_value()); wr_u32(REFERENCE_18,random_value());
    for(i=0;i<3;++i) {
        wr_u16(MATRIX_ROW_SCALES+2*i,(uint16_t)random_value());
        wr_u32(ORIGIN_SMOOTHED_DELTA+4*i,random_value());
        wr_u16(LINE_LAST_ROW+2*i,(uint16_t)random_value());
    }
    wr_u32(0xc45664,random_value()); wr_u16(VIEW_PAN,(uint16_t)random_value()); wr_u16(VIEW_ROTATE,(uint16_t)random_value());
    for(i=0;i<24;++i) wr_u16(DEPTH_VALUES+2*i,(uint16_t)random_value());
    gate_fixture(scenario,0);
}
static int boot_original(void) {
    unsigned step,i;
    for(step=0;step<10000000;++step) {
        uint32_t pc=REG_PC; uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        for(i=0;i<3;++i) if(pc==boot_children[i]) break;
        if(i<3) {
            if(selected_entry!=0xc08f26 || boot_source_calls++!=i) return 0;
            boot_snapshot(boot_child_ram[i]);
            boot_child_effect(NULL,i);
            /* Incidental child CPU outputs are deliberately unrelated to
             * completion status and semantic mutations. A6/A7 stay valid. */
            for(i=0;i<14;++i) REG_DA[i]=random_value();
            REG_PC=rd_u32(REG_A[7]); REG_A[7]+=4;
            continue;
        }
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i) if(scene_source_bytes[i].pc==pc) break;
        if(i==sizeof scene_source_bytes/sizeof scene_source_bytes[0]) {
            fprintf(stderr,"unexpected bootstrap PC %06X\n",pc); return 0;
        }
        scene_seen[i]=1; opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        if(pc==0xc1c472 || pc==0xc1c4be || pc==0xc1c508) gate_bit_seen[rd_u16(REG_A[1])]=1;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    return 0;
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size),*rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(BOOT_RAM_BYTES); BootstrapState *native=malloc(sizeof *native);
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):4096,scenario,i,j;
    char error[256];
    selected_entry=argc>2?(uint32_t)strtoul(argv[2],NULL,16):0xc08f26;
    if(!state || !rom || !m || !base || !before || !expected || !native || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    if(!gate_assets()) { fprintf(stderr,"original Hunk-66 binding differs\n"); return 1; }
    for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
        for(j=0;j<scene_source_bytes[i].length;++j) if(rd_u8(scene_source_bytes[i].pc+j)!=scene_source_bytes[i].bytes[j]) return 1;
    for(scenario=0;scenario<cases;++scenario) {
        int ok; FA18NativeSceneBootstrapOps ops={boot_update,boot_refresh,native};
        boot_scenario=scenario; memcpy(m,base,sizeof *m); boot_fixture(scenario);
        if(!boot_load(native) || (selected_entry!=0xc1c40c && !verify_record_owners(&native->scene))) return 1;
        memcpy(before,m,sizeof *m); boot_snapshot(expected); boot_store(native);
        if(!boot_equal(expected)) { fprintf(stderr,"initial ownership roundtrip failed\n"); return 1; }
        memcpy(m,before,sizeof *m);
        if(!boot_original()) { fprintf(stderr,"original bootstrap failed case %u\n",scenario); return 1; }
        if(selected_entry==0xc1c40c && !scenario) for(i=0;i<3;++i) for(j=0;j<2048;++j)
            if(gate_axis_bytes(&gate_asset_buffers,i)[j]!=rd_u8(TEMPLATE_GATES_X+2048*i+j)) {
                fprintf(stderr,"original disk gate expansion differs axis %u byte %u\n",i,j); return 1;
            }
        boot_snapshot(expected); memcpy(m,before,sizeof *m);
        if(selected_entry==0xc08f26) {
            FA18NativeSceneBootstrapCall call={&native->bootstrap,&ops};
            boot_native_active=native;
            ok=fa18_native_scene_bootstrap_callback(&call);
            boot_native_active=NULL;
        } else if(selected_entry==0xc09620) ok=fa18_prepare_native_scene_player(&native->scene.setup);
        else if(selected_entry==0xc090c2) ok=fa18_clear_native_startup_ranges(&native->startup);
        else if(selected_entry==0xc1c40c) ok=fa18_run_template_bitmask_state(&native->gates.state);
        else return 1;
        if(!ok) { fprintf(stderr,"native bootstrap failed case %u child %u\n",scenario,boot_native_calls); return 1; }
        boot_store(native);
        if(!boot_equal(expected) || boot_source_calls!=boot_native_calls) return 1;
        if(selected_entry!=0xc1c40c && (!verify_record_owners(&native->scene) ||
           native->scene.flight.viewed!=native->scene.bank.aircraft+rd_u16(VIEW_RECORD)/512)) return 1;
    }
    printf("native bootstrap %06X: %u calls match full RAM, live owners and %u ordered child contracts per call\n",selected_entry,cases,selected_entry==0xc08f26?3:0);
    for(i=j=0;i<65536;++i) j+=gate_bit_seen[i]!=0;
    printf("gate assets: %u original Hunk-66 bytes; %u/65536 bit indices exercised\n",gate_hunk_bytes,j);
    if(selected_entry==0xc1c40c && cases>=4096 && j!=65536) return 1;
    printf("visited:"); for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i) if(scene_seen[i]) printf(" %06X",scene_source_bytes[i].pc);
    puts(""); free(native); free(expected); free(before); free(base); free(m); return 0;
}
