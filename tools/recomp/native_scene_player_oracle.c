/* Original instructions validate shared native record/player owners only. */
#define main reference_publication_validation_main
#include "command_publication_oracle.c"
#undef main
#define bind_byte oracle_queue_bind_byte
#define bind_word oracle_queue_bind_word
#include "../../port/command_queue.c"
#undef bind_byte
#undef bind_word
#include "../../port/native_scene_records.c"
#include "../../port/scene_player_setup.c"
#include "../../port/context_command_controls.c"
#ifndef FA18_SCENE_SOURCE_HEADER
#define FA18_SCENE_SOURCE_HEADER "../../build/recomp/native_scene_player_source.h"
#endif
#include FA18_SCENE_SOURCE_HEADER

typedef struct {
    FA18CommandInput commands;
    FA18FlightCommandState flight;
    FA18ViewCommandState view;
    FA18ContextCommandState context;
    FA18CommandEffects effects;
    FA18CommandQueue queue;
    FA18NativeSceneRecords bank;
    FA18NativeScenePlayerSetup setup;
    uint8_t mission_b,mission_c,flags[6],phase,selection;
    uint16_t limit,selected,shown,marker;
    uint32_t warnings,events,position[3];
} SceneState;
static const uint32_t flag_addresses[]={PLAYER_FLAGS_A,PLAYER_FLAGS_B,PLAYER_FLAGS_C,
    PLAYER_FLAGS_D,PLAYER_FLAGS_E,PLAYER_FLAGS_F};
static uint8_t scene_seen[sizeof scene_source_bytes/sizeof scene_source_bytes[0]];

static int load_scene(SceneState *s) {
    uint8_t records[16*512],work[16*32],neighbors[FA18_COMMAND_QUEUE_NEIGHBORS],keys[128];
    unsigned i;
    memset(s,0,sizeof *s);
    for(i=0;i<sizeof records;++i) records[i]=rd_u8(CONTROL_RECORDS+i);
    for(i=0;i<sizeof work;++i) work[i]=rd_u8(WORKSPACE_RECORDS+i);
    if(!fa18_import_native_scene_records(&s->bank,&s->commands,records,sizeof records,work,sizeof work)) return 0;
    s->flight.commands=&s->commands; s->flight.player=s->bank.aircraft;
    s->flight.viewed=s->bank.aircraft+5; s->flight.target=s->bank.aircraft+4;
    for(i=0;i<3;++i) s->flight.spawn_slots[i]=s->bank.aircraft+i+1;
    s->view.flight=&s->flight; s->context.view=&s->view;
    s->context.records=s->bank.geometry; s->context.record_count=16; s->effects.context=&s->context;
    for(i=0;i<sizeof neighbors;++i) neighbors[i]=rd_u8(KEY_RAW-128+i);
    for(i=0;i<128;++i) keys[i]=rd_u8(KEY_TABLE+i);
    if(!fa18_initialize_command_queue(&s->queue,&s->context,neighbors,sizeof neighbors,keys,sizeof keys)) return 0;
    s->flight.mission_flags=rd_u8(MISSION_FLAGS_A); s->flight.ecm_enabled=rd_u8(PLAYER_FLAGS_G);
    s->flight.spawn_gate=rd_u16(COMMAND_SPAWN_GATE);
    s->mission_b=rd_u8(MISSION_FLAGS_B); s->mission_c=rd_u8(MISSION_FLAGS_C);
    s->limit=rd_u16(PLAYER_LIMIT); s->selected=rd_u16(SELECTED_RECORD);
    s->shown=rd_u16(MESSAGE_SHOWN); s->marker=rd_u16(0xc45ae4);
    s->warnings=rd_u32(WARNING_CAUSES); s->events=rd_u32(EVENT_BITS);
    s->effects.message_code=rd_u16(MESSAGE_CODE);
    s->context.origin_first=rd_u32(SELECTOR_ORIGIN);
    s->view.origin_middle=rd_u32(SELECTOR_ORIGIN_MIDDLE);
    s->context.origin_third=rd_u32(SELECTOR_ORIGIN_THIRD);
    for(i=0;i<3;++i) { s->context.negated[i]=rd_u32(ORIGIN_NEGATED_COMPANION+4*i); s->position[i]=REG_D[i]; }
    s->setup=(FA18NativeScenePlayerSetup){.records=&s->bank,.flight=&s->flight,.effects=&s->effects,
        .mission_flags_b=&s->mission_b,.mission_flags_c=&s->mission_c,.phase=&s->phase,
        .selection_active=&s->selection,.limit=&s->limit,.selected_record=&s->selected,
        .message_shown=&s->shown,.message_marker=&s->marker,.warning_causes=&s->warnings,.event_bits=&s->events};
    for(i=0;i<6;++i) { s->flags[i]=rd_u8(flag_addresses[i]); s->setup.player_flags[i]=s->flags+i; }
    return fa18_bind_native_scene_player(&s->setup,&s->queue);
}
static void store_scene(const SceneState *s) {
    unsigned i,j; uint8_t bytes[512],value;
    for(i=0;i<16;++i) {
        if(!fa18_read_native_scene_record(s->bank.records+i,0,bytes,512)) abort();
        for(j=0;j<512;++j) wr_u8(CONTROL_RECORDS+512*i+j,bytes[j]);
        for(j=0;j<32;++j) wr_u8(WORKSPACE_RECORDS+32*i+j,s->bank.work[i][j]);
    }
    for(i=0;i<FA18_COMMAND_QUEUE_NEIGHBORS;++i) {
        if(!port_read_field_byte(s->queue.slots+i,&value)) abort();
        wr_u8(KEY_RAW-128+i,value);
    }
    wr_u8(MISSION_FLAGS_A,s->flight.mission_flags); wr_u8(PLAYER_FLAGS_G,s->flight.ecm_enabled);
    wr_u16(COMMAND_SPAWN_GATE,s->flight.spawn_gate);
    wr_u8(MISSION_FLAGS_B,s->mission_b); wr_u8(MISSION_FLAGS_C,s->mission_c);
    wr_u16(PLAYER_LIMIT,s->limit); wr_u16(SELECTED_RECORD,s->selected);
    wr_u16(MESSAGE_SHOWN,s->shown); wr_u16(0xc45ae4,s->marker);
    wr_u32(WARNING_CAUSES,s->warnings); wr_u32(EVENT_BITS,s->events);
    wr_u16(MESSAGE_CODE,s->effects.message_code);
    for(i=0;i<6;++i) wr_u8(flag_addresses[i],s->flags[i]);
    wr_u32(SELECTOR_ORIGIN,s->context.origin_first); wr_u32(SELECTOR_ORIGIN_MIDDLE,s->view.origin_middle);
    wr_u32(SELECTOR_ORIGIN_THIRD,s->context.origin_third);
    for(i=0;i<3;++i) wr_u32(ORIGIN_NEGATED_COMPANION+4*i,s->context.negated[i]);
}
/* Compare named game owners independently of the byte-view exporter. This
 * catches an incorrect view binding that otherwise preserves packed output. */
static int verify_record_owners(const SceneState *s) {
    unsigned i,j,k;
    for(i=0;i<16;++i) {
        uint32_t a=CONTROL_RECORDS+512*i;
        const FA18FlightCommandRecord *f=s->bank.aircraft+i;
        const FA18ContextCommandRecord *g=s->bank.geometry+i;
        if(f->flags!=rd_u16(a) || f->secondary_flags!=rd_u16(a+2) ||
           f->equipment_kind!=rd_u8(a+0x62) || f->weapon_radar!=rd_u8(a+0x63) ||
           f->stick!=rd_u8(a+0x65) || g->angle!=rd_u16(a+0x68) ||
           g->command_record!=f || *s->bank.records[i].level!=rd_u8(a+0x2b)) return 0;
        for(j=0;j<3;++j) {
            if(g->position[j]!=rd_u32(a+0x14+4*j)) return 0;
            for(k=0;k<3;++k) {
                if(g->inverse[j][k]!=rd_s16(a+0x92+6*j+2*k) ||
                   s->bank.records[i].forward[j][k]!=rd_s16(a+0x80+6*j+2*k)) return 0;
            }
        }
    }
    return s->flight.weapon_mode_redraws==rd_u8(COMMAND_WEAPON_MODE_REDRAWS) &&
        s->flight.weapon_redraws==rd_u8(WEAPON_REDRAWS) && s->flight.chaff_count==rd_u8(MISSION_LEVEL_A) &&
        s->flight.flare_count==rd_u8(MISSION_LEVEL_B) && s->commands.indexed.player_ready==rd_u8(PLAYER_READY);
}
static void scene_fixture(unsigned scenario) {
    static const uint8_t phases[]={0,1,0x80,0xff}; unsigned i;
    for(i=0;i<16*512+16*32;++i) wr_u8(CONTROL_RECORDS+i,(uint8_t)random_value());
    for(i=0;i<FA18_COMMAND_QUEUE_NEIGHBORS;++i) wr_u8(KEY_RAW-128+i,(uint8_t)random_value());
    for(i=0;i<6;++i) wr_u8(flag_addresses[i],(uint8_t)random_value());
    wr_u8(MISSION_FLAGS_A,(uint8_t)random_value()); wr_u8(MISSION_FLAGS_B,(uint8_t)random_value());
    wr_u8(MISSION_FLAGS_C,(uint8_t)random_value()); wr_u8(PLAYER_FLAGS_G,(uint8_t)random_value());
    wr_u8(PLAYER_PHASE,phases[scenario%4]); wr_u16(PLAYER_LIMIT,(uint16_t)random_value());
    wr_u16(SELECTED_RECORD,(uint16_t)random_value()); wr_u16(COMMAND_SPAWN_GATE,(uint16_t)random_value());
    wr_u16(VIEW_RECORD,5*512);
    wr_u16(MESSAGE_CODE,(uint16_t)random_value()); wr_u16(MESSAGE_SHOWN,(uint16_t)random_value());
    wr_u16(0xc45ae4,(uint16_t)random_value()); wr_u32(WARNING_CAUSES,random_value()); wr_u32(EVENT_BITS,random_value());
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    SET_CYCLES(100000000); REG_A[7]=0xc7ff00; wr_u32(REG_A[7],0xc70000);
    m68k_set_reg(M68K_REG_SR,0x2700|(scenario&31)); REG_PC=selected_entry;
    fa18_recomp_abort=0; fa18_next_event=INT64_MAX;
}
#ifndef FA18_SCENE_PLAYER_HELPERS_ONLY
static int original_scene(void) {
    unsigned step,i;
    for(step=0;step<5000;++step) {
        uint32_t pc=REG_PC; uint16_t opcode;
        if(selected_entry==0xc08f76 && pc==0xc08faa) return 1;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            if(scene_source_bytes[i].pc==pc) break;
        if(i==sizeof scene_source_bytes/sizeof scene_source_bytes[0]) {
            fprintf(stderr,"unexpected scene PC %06X\n",pc); return 0;
        }
        scene_seen[i]=1; opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    return 0;
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE); SceneState *native=malloc(sizeof *native);
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):4096,scenario,i,j;
    char error[256];
    selected_entry=argc>2?(uint32_t)strtoul(argv[2],NULL,16):0xc09620;
    if(!state || !rom || !m || !base || !before || !expected || !native || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
        for(j=0;j<scene_source_bytes[i].length;++j)
            if(rd_u8(scene_source_bytes[i].pc+j)!=scene_source_bytes[i].bytes[j]) return 1;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t result[3]; int ok;
        memcpy(m,base,sizeof *m); scene_fixture(scenario);
        if(!load_scene(native) || !verify_record_owners(native)) return 1;
        memcpy(before,m,sizeof *m);
        if(!original_scene()) { fprintf(stderr,"original scene failed case %u\n",scenario); return 1; }
        for(i=0;i<3;++i) result[i]=REG_D[i];
        if(!verify_record_owners(native) && (selected_entry==0xc0910c || selected_entry==0xc0915a)) return 1;
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        switch(selected_entry) {
        case 0xc0840e: ok=fa18_reset_native_mission_objects(&native->setup); break;
        case 0xc09620: ok=fa18_prepare_native_scene_player(&native->setup); break;
        case 0xc095c0: ok=fa18_reset_native_scene_player(&native->setup); break;
        case 0xc08f76: ok=fa18_clear_native_bootstrap_records(&native->bank); break;
        case 0xc0910c: ok=fa18_native_scene_start_position(native->position); break;
        case 0xc0915a: {
            FA18ContextCommandChildInput input={0}; FA18ContextCommandChildResult output;
            for(i=0;i<3;++i) input.position[i]=(int32_t)native->position[i];
            ok=fa18_apply_context_control_child(&native->context,CONTEXT_COMMAND_SET_OBSERVER,&input,&output);
            for(i=0;i<3;++i) native->position[i]=(uint32_t)output.position[i];
            break;
        }
        default: return 1;
        }
        if(!ok) return 1;
        if(selected_entry==0xc0910c || selected_entry==0xc0915a)
            for(i=0;i<3;++i) if(native->position[i]!=result[i]) return 1;
        store_scene(native);
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            uint32_t address=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
            if((selected_entry==0xc0840e || selected_entry==0xc09620) && address>=0xc7fd00 && address<0xc7ff00) continue;
            if(rd_u8(address)!=expected[i]) {
                fprintf(stderr,"scene %06X case %u byte %06X source %02X native %02X\n",
                    selected_entry,scenario,address,expected[i],rd_u8(address)); return 1;
            }
        }
        if(!verify_record_owners(native) || native->flight.viewed!=native->bank.aircraft+5) return 1;
    }
    printf("native scene %06X: %u calls match full game RAM and shared record owners; no child contracts\n",selected_entry,cases);
    printf("visited:"); for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
        if(scene_seen[i]) printf(" %06X",scene_source_bytes[i].pc);
    puts(""); free(native); free(expected); free(before); free(base); free(m); return 0;
}
#endif
