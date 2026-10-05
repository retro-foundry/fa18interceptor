/* Validation only: native queue publication versus original instructions. */
#define main reference_queue_validation_main
#include "command_publication_oracle.c"
#undef main
#include "../../port/command_queue.c"
#include "../../build/recomp/native_command_queue_source.h"

typedef struct {
    FA18CommandInput c;
    FA18FlightCommandState f;
    FA18ViewCommandState v;
    FA18ContextCommandState context;
    FA18CommandQueue q;
} NativeQueue;

/* Independent field-to-source mappings, using the existing source symbols.
 * Checking these as well as RAM catches bindings to the wrong native owner. */
#define OWNERS(X) \
 X(c.origin_mode,ORIGIN_ENABLE) X(c.indexed.mode_gate,COMMAND_MODE_GATE) \
 X(c.indexed.enable_gate,COMMAND_ENABLE_GATE) X(c.indexed.mode_request,0xc45792u) \
 X(c.indexed.enable_selection,0xc45849u) X(c.indexed.origin_gate_a,ORIGIN_GATE_A) \
 X(c.indexed.origin_gate_b,ORIGIN_GATE_B) X(c.indexed.pose_entry,SCENE_POSE_ENTRY) \
 X(c.indexed.recorder_mode,RECORDER_MODE) X(c.indexed.player_ready,PLAYER_READY) \
 X(c.return_state,COMMAND_RETURN_STATE) X(c.event_counter,COMMAND_EVENT_COUNTER) \
 X(c.message_state,MESSAGE_STATE_C) \
 X(f.weapon_pause,COMMAND_WEAPON_PAUSE) X(f.hud_mode,POST_INPUT_EXPIRED) \
 X(f.eject_flag,BAR_E_FLAG) X(f.pause,PAUSE_A) X(f.context_started,CONTEXT_STARTED) \
 X(f.next_target,COMMAND_NEXT_TARGET_FLAG) X(f.sequence_phase,SEQUENCE_PHASE) \
 X(f.stick_y,STICK_Y) X(f.trim_input,COMMAND_TRIM_INPUT) X(f.stick_x,STICK_X) \
 X(f.redraw_b,BAR_REDRAWS_B) X(f.redraw_c,BAR_REDRAWS_C) X(f.redraw_d,BAR_REDRAWS_D) \
 X(f.redraw_e,BAR_REDRAWS_E) X(f.scale_redraws,SCALE_REDRAWS) X(f.info_redraws,INFO_REDRAWS) \
 X(f.weapon_mode_redraws,COMMAND_WEAPON_MODE_REDRAWS) X(f.weapon_redraws,WEAPON_REDRAWS) \
 X(f.script_count,SCRIPT_COUNT) X(f.chaff_count,MISSION_LEVEL_A) X(f.flare_count,MISSION_LEVEL_B) \
 X(f.chaff_timer,COMMAND_CHAFF_TIMER) X(f.flare_timer,COMMAND_FLARE_TIMER) \
 X(v.mode,VIEW_MODE) X(v.mode_auxiliary,0xc457a8u) X(v.mode_companion,0xc457a9u) \
 X(v.redraw_keep_state,REDRAW_KEEP_STATE) X(v.zoom_flags,ZOOM_FLAGS) X(v.redraw_first,REDRAW_FIRST) \
 X(v.gauge_refresh,GAUGE_REFRESH) X(v.grid_z_redraws,GRID_Z_REDRAWS) X(v.grid_x_redraws,GRID_X_REDRAWS) \
 X(v.display_update,DISPLAY_UPDATE) X(v.redraw_bar_a,BAR_REDRAWS_A) X(v.redraw_bar_aux,0xc45841u) \
 X(v.update_mask,UPDATE_MASK) X(context.recorder_on,RECORDER_ON) X(context.track_started,TRACK_STARTED) \
 X(context.view_request,0xc45833u) X(q.taken,KEY_TAKEN) X(q.count,KEY_COUNT) \
 X(q.write_index,KEY_WRITE) X(q.translated_index,KEY_TRANSLATED_WRITE) \
 X(c.modifier,KEY_STATE) X(c.indexed.function_modifier,KEY_STATE+1) X(c.other_modifier,KEY_STATE+2)

static int verify_owners(const NativeQueue *s, unsigned scenario) {
    unsigned i;
#define CHECK(field,address) if (s->field != rd_u8(address)) { \
    fprintf(stderr,"case %u native owner %s source %02X native %02X\n",scenario,#field,rd_u8(address),s->field); return 0; }
    OWNERS(CHECK)
#undef CHECK
    if (s->c.indexed.throttle != rd_s16(CONTROL_ACCUMULATOR_Y) ||
        s->c.indexed.throttle_companion != rd_s16(CONTROL_ACCUMULATOR_COMPANION)) return 0;
    for (i=0;i<sizeof s->q.raw;++i) if (s->q.raw[i]!=rd_u8(KEY_RAW+i)) return 0;
    for (i=0;i<sizeof s->q.translated;++i) if (s->q.translated[i]!=rd_u8(KEY_TRANSLATED+i)) return 0;
    return 1;
}

static unsigned char raw_slots[FA18_COMMAND_QUEUE_NEIGHBORS], translated_slots[FA18_COMMAND_QUEUE_NEIGHBORS];
static int original_publication(void) {
    unsigned step;
    for (step=0;step<64;++step) {
        uint32_t pc=REG_PC;
        uint16_t opcode;
        if (pc==0xc70000u && REG_A[7]==expected_sp) return 1;
        if (pc<selected_entry || pc>=source_end()) return 0;
        if (pc==0xc1c272u || pc==0xc1c2a0u) {
            uint32_t address=REG_A[3]+(uint32_t)(int32_t)(int16_t)REG_D[4];
            unsigned offset=address-(KEY_RAW-128u);
            if (offset>=FA18_COMMAND_QUEUE_NEIGHBORS) return 0;
            (pc==0xc1c272u ? raw_slots : translated_slots)[offset]=1;
        }
        visited[pc-selected_entry]=1;
        opcode=m68k_read_memory_16(pc);
        REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    return 0;
}

int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    char error[256];
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):73728,scenario,i,row;
    if (!state || !rom || !m || !base || !expected || !cases) return 1;
    if (!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) {
        fprintf(stderr,"native command queue: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m); fa18_bus_timing=0;
    for (row=0;row<sizeof source_bytes/sizeof source_bytes[0];++row)
        for (i=0;i<source_bytes[row].length;++i)
            if (rd_u8(source_bytes[row].pc+i)!=source_bytes[row].bytes[i]) return 1;
    for (scenario=0;scenario<cases;++scenario) {
        NativeQueue s;
        uint8_t data[FA18_COMMAND_QUEUE_NEIGHBORS],keys[FA18_COMMAND_KEY_TABLE_SIZE];
        uint32_t event;
        memcpy(m,base,sizeof *m);
        for (i=0;i<sizeof data;++i) wr_u8(KEY_RAW-128u+i,(uint8_t)random_value());
        fixture(scenario);
        if (scenario>=8192) {
            unsigned n=scenario-8192,raw=n&255u,index=(n>>8)&255u;
            REG_D[0]=(REG_D[0]&0xffffff00u)|raw;
            wr_u8(KEY_TAKEN,0); wr_u8(KEY_COUNT,0); wr_u8(KEY_WRITE,(uint8_t)index);
            wr_u8(KEY_TRANSLATED_WRITE,(uint8_t)(raw*7+index*13));
        }
        for (i=0;i<sizeof data;++i) data[i]=rd_u8(KEY_RAW-128u+i);
        for (i=0;i<sizeof keys;++i) keys[i]=rd_u8(KEY_TABLE+i);
        memset(&s,0,sizeof s);
        s.f.commands=&s.c; s.v.flight=&s.f; s.context.view=&s.v;
        s.c.modifier=rd_u8(KEY_STATE); s.c.indexed.function_modifier=rd_u8(KEY_STATE+1);
        s.c.other_modifier=rd_u8(KEY_STATE+2);
        if (!fa18_initialize_command_queue(&s.q,&s.context,data,sizeof data,keys,sizeof keys) ||
            !verify_owners(&s,scenario) || !fa18_publish_native_command(&s.q,REG_D[0],&event)) return 1;
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        for (i=0;i<sizeof data;++i) {
            const FA18CommandQueueByte *slot=&s.q.slots[i];
            uint8_t value=slot->byte ? *slot->byte : (uint8_t)((uint16_t)*slot->word>>slot->shift);
            expected[FA18_CHIP_SIZE+KEY_RAW-128u+i-FA18_SLOW_BASE]=value;
        }
        expected[FA18_CHIP_SIZE+KEY_STATE-FA18_SLOW_BASE]=s.c.modifier;
        expected[FA18_CHIP_SIZE+KEY_STATE+1-FA18_SLOW_BASE]=s.c.indexed.function_modifier;
        expected[FA18_CHIP_SIZE+KEY_STATE+2-FA18_SLOW_BASE]=s.c.other_modifier;
        if (!original_publication() || event!=REG_D[0] || !verify_owners(&s,scenario)) {
            fprintf(stderr,"native command queue case %u event source %08X native %08X\n",scenario,REG_D[0],event); return 1;
        }
        if (memcmp(expected,m->chip,FA18_CHIP_SIZE) || memcmp(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE)) {
            for (i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
                uint32_t address=i<FA18_CHIP_SIZE ? i : i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
                if (expected[i]!=rd_u8(address)) {
                    fprintf(stderr,"case %u byte %06X source %02X native %02X\n",scenario,address,rd_u8(address),expected[i]); return 1;
                }
            }
        }
    }
    {
        unsigned count=0,raw_count=0,translated_count=0;
        for (i=0;i<sizeof visited;++i) count+=visited[i]!=0;
        for (i=0;i<sizeof raw_slots;++i) { raw_count+=raw_slots[i]!=0; translated_count+=translated_slots[i]!=0; }
        printf("native command queue: %u calls matched full RAM, events and canonical owners; %u boundaries; %u raw and %u translated destinations\n",cases,count,raw_count,translated_count);
        printf("visited:"); for (i=0;i<sizeof visited;++i) if (visited[i]) printf(" %06X",selected_entry+i); putchar('\n');
        if (cases>=73728 && (raw_count!=138 || translated_count!=256)) return 1;
    }
    free(expected); free(base); free(m); return 0;
}
