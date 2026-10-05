/* Validation only. Parent actions and nine control-child calls execute the
 * original instructions. Other children use explicit test contracts. */
#define main reference_flight_action_validation_main
#include "flight_commands_oracle.c"
#undef main
#include "../../port/flight_command_input.c"
#include "../../build/recomp/native_flight_command_input_source.h"

#define STATE_BYTES(X) \
 X(redraw_e,BAR_REDRAWS_E) X(redraw_b,BAR_REDRAWS_B) X(redraw_c,BAR_REDRAWS_C) \
 X(redraw_d,BAR_REDRAWS_D) X(scale_redraws,SCALE_REDRAWS) \
 X(info_request,INFO_REQUEST) X(info_redraws,INFO_REDRAWS) X(hud_mode,POST_INPUT_EXPIRED) \
 X(trim_input,COMMAND_TRIM_INPUT) X(next_target,COMMAND_NEXT_TARGET_FLAG) \
 X(script_count,SCRIPT_COUNT) X(weapon_pause,COMMAND_WEAPON_PAUSE) \
 X(weapon_mode_redraws,COMMAND_WEAPON_MODE_REDRAWS) X(weapon_redraws,WEAPON_REDRAWS) \
 X(shoot_cue,SHOOT_CUE) X(gear_message,COMMAND_GEAR_MESSAGE) \
 X(flare_count,MISSION_LEVEL_B) X(chaff_count,MISSION_LEVEL_A) \
 X(flare_timer,COMMAND_FLARE_TIMER) X(chaff_timer,COMMAND_CHAFF_TIMER) \
 X(mission_flags,MISSION_FLAGS_A) X(ecm_enabled,PLAYER_FLAGS_G) \
 X(sequence_phase,SEQUENCE_PHASE) X(eject_flag,BAR_E_FLAG) \
 X(pause,PAUSE_A) X(context_started,CONTEXT_STARTED) X(stick_y,STICK_Y) X(stick_x,STICK_X)
#define COMMAND_BYTES(X) \
 X(indexed.mode,MODE_SELECT) X(indexed.origin_gate_a,ORIGIN_GATE_A) \
 X(block_flags,COMMAND_BLOCK_FLAGS) X(indexed.function_level,FUNCTION_KEY_LEVEL) \
 X(modifier,KEY_STATE) X(indexed.function_modifier,KEY_STATE+1) X(other_modifier,KEY_STATE+2)
#define STATE_WORDS(X) \
 X(emitted_requests,PENDING_COMMAND_WORD_A) X(command_word,COMMAND_WORD) \
 X(info_page,INFO_PAGE) X(spawn_gate,COMMAND_SPAWN_GATE)
static const struct {uint32_t entry,ret;} child_edges[]={
    {0xc1c214,0xc1b14a},{0xc33186,0xc1b1aa},{0xc33186,0xc1b1ce},
    {0xc0833e,0xc1b228},{0xc08394,0xc1b232},
    {0xc1b50c,0xc1b4f8},{0xc1b510,0xc1b500},{0xc1b514,0xc1b508},
    {0xc1b558,0xc1b544},{0xc1b55c,0xc1b54c},{0xc1b560,0xc1b554},
    {0xc1b602,0xc1b5d8},{0xc1b602,0xc1b5e0},{0xc33186,0xc1b5e6},
    {0xc33186,0xc1b630},{0xc25704,0xc1b65e},
    {0xc33186,0xc1bb9c},{0xc25704,0xc1bbb4},{0xc33186,0xc1bbc0},
    {0xc33186,0xc1bc4c},{0xc33186,0xc1c062},
    {0xc25704,0xc1c11c},{0xc17f8c,0xc1c16a},
    {0xc25704,0xc1c1aa},{0xc33186,0xc1c20e}
};
typedef struct {
    FA18CommandInput commands;
    FA18FlightCommandState state;
    FA18FlightCommandRecord records[6];
    uint32_t addresses[6];
} NativeFixture;
typedef struct {
    FA18CommandInput commands;
    FA18FlightCommandState state;
    FA18FlightCommandRecord records[6];
    unsigned view;
} Snapshot;
typedef struct {
    enum FlightCommandChild child;
    FA18FlightCommandChildInput input;
    FlightCommandResult result;
    Snapshot before;
    int real_control;
} ChildTrace;
typedef struct {
    NativeFixture *fixture;
    unsigned scenario,count;
    ChildTrace traces[4];
} Trace;

static unsigned view_index(const NativeFixture *f) {
    unsigned i;
    for(i=0;i<6;++i) if(f->state.viewed==&f->records[i]) return i;
    fprintf(stderr,"invalid native viewed record\n"); exit(1);
}
static void snapshot(const NativeFixture *f,Snapshot *out) {
    unsigned i;
    memset(out,0,sizeof *out);
    out->commands=f->commands; out->state=f->state; out->view=view_index(f);
    memcpy(out->records,f->records,sizeof out->records);
    out->state.commands=NULL; out->state.player=out->state.viewed=out->state.target=NULL;
    for(i=0;i<3;++i) out->state.spawn_slots[i]=NULL;
}
static void load_fixture(NativeFixture *f,const uint32_t *addresses) {
    unsigned i,view=6;
    uint32_t address=CONTROL_RECORDS+(uint32_t)(int32_t)rd_s16(VIEW_RECORD);
    memset(f,0,sizeof *f); memcpy(f->addresses,addresses,sizeof f->addresses);
#define FIELD(name,guest) f->state.name=rd_u8(guest);
    STATE_BYTES(FIELD)
#undef FIELD
#define FIELD(name,guest) f->commands.name=rd_u8(guest);
    COMMAND_BYTES(FIELD)
#undef FIELD
#define FIELD(name,guest) f->state.name=rd_u16(guest);
    STATE_WORDS(FIELD)
#undef FIELD
    f->commands.indexed.throttle=rd_s16(CONTROL_ACCUMULATOR_Y);
    f->commands.indexed.throttle_companion=rd_s16(CONTROL_ACCUMULATOR_COMPANION);
    f->state.gear_gate=rd_u32(COMMAND_GEAR_GATE);
    for(i=0;i<6;++i) {
        FA18FlightCommandRecord *r=&f->records[i];
        r->flags=rd_u16(addresses[i]); r->secondary_flags=rd_u16(addresses[i]+2);
        r->equipment_kind=rd_u8(addresses[i]+0x62); r->weapon_radar=rd_u8(addresses[i]+0x63);
        r->stick=rd_u8(addresses[i]+0x65);
        if(addresses[i]==address) view=i;
    }
    if(view==6) { fprintf(stderr,"unresolved source view %06X\n",address); exit(1); }
    f->state.commands=&f->commands; f->state.player=&f->records[0];
    f->state.viewed=&f->records[view]; f->state.target=&f->records[4];
    for(i=0;i<3;++i) f->state.spawn_slots[i]=&f->records[i+1];
}
static void store_fixture(const NativeFixture *f) {
    unsigned i;
#define FIELD(name,guest) wr_u8(guest,f->state.name);
    STATE_BYTES(FIELD)
#undef FIELD
#define FIELD(name,guest) wr_u8(guest,f->commands.name);
    COMMAND_BYTES(FIELD)
#undef FIELD
#define FIELD(name,guest) wr_u16(guest,f->state.name);
    STATE_WORDS(FIELD)
#undef FIELD
    wr_u16(CONTROL_ACCUMULATOR_Y,(uint16_t)f->commands.indexed.throttle);
    wr_u16(CONTROL_ACCUMULATOR_COMPANION,(uint16_t)f->commands.indexed.throttle_companion);
    wr_u32(COMMAND_GEAR_GATE,f->state.gear_gate);
    wr_u16(VIEW_RECORD,(uint16_t)(f->addresses[view_index(f)]-CONTROL_RECORDS));
    for(i=0;i<6;++i) {
        const FA18FlightCommandRecord *r=&f->records[i]; uint32_t address=f->addresses[i];
        wr_u16(address,r->flags); wr_u16(address+2,r->secondary_flags);
        wr_u8(address+0x62,r->equipment_kind); wr_u8(address+0x63,r->weapon_radar);
        wr_u8(address+0x65,r->stick);
    }
}
/* Test contracts only: stress reads after children and partial event results. */
static void contract_effects(NativeFixture *f,enum FlightCommandChild child,unsigned scenario) {
    if(child==FLIGHT_RADAR_RANGE && (scenario&64)) f->state.viewed=&f->records[2];
    if(child==FLIGHT_WEAPON_MODE) f->commands.block_flags=(scenario&64)?0x0f:0;
    if(child==FLIGHT_ECM) f->state.ecm_enabled=(scenario&64)?0:0x80;
    if(child==FLIGHT_HOOK) f->state.player->secondary_flags^=0x8080;
    if(child==FLIGHT_EJECT_TOGGLE) {
        f->state.eject_flag=f->state.eject_flag?0:1;
        f->commands.modifier=f->commands.indexed.function_modifier=f->commands.other_modifier=0;
    }
}
static int consume(void *context,FA18FlightCommandState *state,enum FlightCommandChild child,
                    const FA18FlightCommandChildInput *input,FlightCommandResult *result) {
    Trace *trace=context; ChildTrace *row;
    if(state!=&trace->fixture->state || trace->count>=4) return 0;
    row=&trace->traces[trace->count++]; row->child=child; row->input=*input;
    snapshot(trace->fixture,&row->before);
    row->real_control=fa18_apply_flight_control_child(state,child,input,result);
    if(!row->real_control) {
        contract_effects(trace->fixture,child,trace->scenario);
        result->event=input->event^(0x13579bdfu+child*0x10001u);
        result->carried_event_word=(int16_t)((uint16_t)input->restore_event_word^0xa55au);
    }
    row->result=*result; return 1;
}
static unsigned char leaf_seen[8];
static unsigned char control_visited[sizeof source_bytes/sizeof source_bytes[0]];
static const uint32_t leaf_entries[]={0xc08394,0xc1b50c,0xc1b510,0xc1b514,
                                     0xc1b558,0xc1b55c,0xc1b560,0xc1b602};
static int original_action(Trace *trace) {
    unsigned step,next=0,i;
    uint32_t active_return=0;
    for(step=0;step<10000;++step) {
        uint32_t pc=REG_PC;
        if(active_return && pc==active_return && REG_A[7]==0xc7ff00u) active_return=0;
        if(!active_return && pc==0xc1c23cu) return next==trace->count;
        if(!active_return && next<trace->count) {
            ChildTrace *row=&trace->traces[next];
            if(pc==child_edges[row->child].entry && rd_u32(REG_A[7])==child_edges[row->child].ret) {
                NativeFixture current; Snapshot got;
                load_fixture(&current,trace->fixture->addresses); snapshot(&current,&got);
                if(memcmp(&got,&row->before,sizeof got) || REG_D[0]!=row->input.event ||
                   ((row->child==FLIGHT_FLARE_SOUND || row->child==FLIGHT_CHAFF_SOUND) &&
                     (int16_t)REG_D[4]!=row->input.restore_event_word) ||
                   (row->child==FLIGHT_FLARE_SPAWN &&
                     (rd_u32(REG_A[7]+4)!=(uint32_t)row->input.arguments[0] ||
                      rd_u32(REG_A[7]+8)!=(uint32_t)row->input.arguments[1]))) {
                    fprintf(stderr,"child %u input/state mismatch at %06X event %08X/%08X\n",
                            row->child,pc,REG_D[0],row->input.event); return 0;
                }
                ++next;
                if(row->real_control) {
                    active_return=child_edges[row->child].ret;
                    for(i=0;i<8;++i) if(pc==leaf_entries[i]) leaf_seen[i]=1;
                } else {
                    contract_effects(&current,row->child,trace->scenario); store_fixture(&current);
                    REG_D[0]=row->result.event;
                    REG_D[4]=(REG_D[4]&0xffff0000u)|(uint16_t)row->result.carried_event_word;
                    REG_PC=rd_u32(REG_A[7]); REG_A[7]+=4;
                    continue;
                }
            }
        }
        if(!active_return && !source_parent_pc(pc)) {
            fprintf(stderr,"unexpected source flight PC %06X\n",pc); return 0;
        }
        if(!active_return) visited[pc-action_base]=1;
        else {
            for(i=0;i<sizeof source_bytes/sizeof source_bytes[0];++i)
                if(pc==source_bytes[i].pc) break;
            if(i==sizeof source_bytes/sizeof source_bytes[0]) {
                fprintf(stderr,"unexpected control-child PC %06X\n",pc); return 0;
            }
            control_visited[i]=1;
        }
        { uint16_t opcode=rd_u16(pc);
          REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
          m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]); }
    }
    return 0;
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base);
    uint8_t *before=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE),*expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):1024,scenario,row,byte;
    unsigned real_controls=0,contracts=0; char error[256];
    if(argc>2) selected_entry=(uint32_t)strtoul(argv[2],NULL,16);
    for(selected_action=COMMAND_PENDING_EMPTY;selected_action<=COMMAND_INDEXED;++selected_action)
        if(glue_command_action_pc(selected_action)==selected_entry) break;
    if(!state || !rom || !m || !base || !before || !expected || !cases ||
       !fa18_is_flight_input_command(selected_action)) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) {
        fprintf(stderr,"native flight commands: %s\n",error); return 1;
    }
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(row=0;row<sizeof source_bytes/sizeof source_bytes[0];++row)
        for(byte=0;byte<source_bytes[row].length;++byte)
            if(rd_u8(source_bytes[row].pc+byte)!=source_bytes[row].bytes[byte]) return 1;
    for(scenario=0;scenario<cases;++scenario) {
        NativeFixture native; Trace trace; uint32_t event,addresses[6];
        CommandRequest request; FA18FlightCommandOps ops={consume,&trace};
        memcpy(m,base,sizeof *m); fixture(scenario);
        /* Retargeted radar children must exercise the 11 -> 9 branch too. */
        wr_u8(CONTROL_RECORDS+0x463,(scenario&128)?0xab:0xad);
        for(row=0;row<5;++row) addresses[row]=CONTROL_RECORDS+0x200u*row;
        addresses[5]=CONTROL_RECORDS+(uint32_t)(int32_t)rd_s16(VIEW_RECORD);
        if(addresses[5]==addresses[0] || addresses[5]==addresses[1]) addresses[5]=CONTROL_RECORDS+0x1000;
        load_fixture(&native,addresses); memset(&trace,0,sizeof trace);
        trace.fixture=&native; trace.scenario=scenario;
        request=(CommandRequest){selected_action,REG_D[0],(uint8_t)REG_D[5],(uint8_t)REG_D[6],0,0};
        memcpy(before,m->chip,FA18_CHIP_SIZE); memcpy(before+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        if(!fa18_apply_flight_input_command(&native.state,&request,(int16_t)REG_D[4],&ops,&event)) return 1;
        store_fixture(&native);
        for(row=0;row<trace.count;++row) {
            ChildTrace *child=&trace.traces[row]; uint32_t sp=0xc7ff00u;
            if(child->child==FLIGHT_FLARE_SPAWN) {
                sp-=2; wr_u16(sp,(uint16_t)request.raw_event);
                sp-=4; wr_u32(sp,(uint32_t)child->input.arguments[1]);
                sp-=4; wr_u32(sp,(uint32_t)child->input.arguments[0]);
            }
            sp-=4; wr_u32(sp,child_edges[child->child].ret);
            if(child->real_control) ++real_controls; else ++contracts;
        }
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m->chip,before,FA18_CHIP_SIZE); memcpy(m->slow,before+FA18_CHIP_SIZE,FA18_SLOW_SIZE);
        if(!original_action(&trace) || REG_D[0]!=event || REG_A[7]!=0xc7ff00u ||
           memcmp(m->chip,expected,FA18_CHIP_SIZE) || memcmp(m->slow,expected+FA18_CHIP_SIZE,FA18_SLOW_SIZE)) {
            unsigned offset;
            fprintf(stderr,"native flight command %06X case %u: PC %06X event %08X/%08X SP %06X\n",
                    selected_entry,scenario,REG_PC,REG_D[0],event,REG_A[7]);
            for(offset=0;offset<FA18_SLOW_SIZE;++offset) if(m->slow[offset]!=expected[FA18_CHIP_SIZE+offset]) {
                fprintf(stderr,"RAM %06X original %02X native %02X\n",FA18_SLOW_BASE+offset,m->slow[offset],expected[FA18_CHIP_SIZE+offset]); break;
            }
            return 1;
        }
    }
    printf("native flight command %06X: %u actions matched full RAM, event and ordered child inputs; %u real control / %u contract children\n",selected_entry,cases,real_controls,contracts);
    printf("visited:"); for(row=0;row<sizeof visited;++row) if(visited[row]) printf(" %06X",action_base+row); putchar('\n');
    printf("leaves:"); for(row=0;row<8;++row) if(leaf_seen[row]) printf(" %06X",leaf_entries[row]); putchar('\n');
    printf("control_visited:");
    for(row=0;row<sizeof control_visited;++row)
        if(control_visited[row]) printf(" %06X",source_bytes[row].pc);
    putchar('\n');
    free(expected); free(before); free(base); free(m); return 0;
}
