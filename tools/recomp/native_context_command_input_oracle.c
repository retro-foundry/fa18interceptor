/* Five native context actions: actual geometry/observer, contracted audio. */
#define main reference_context_action_validation_main
#include "context_commands_oracle.c"
#undef main
#include "../../port/context_command_controls.c"
#include "../../build/recomp/native_context_command_input_source.h"

#define COMMAND_BYTES(X) \
 X(origin_mode,ORIGIN_ENABLE) X(indexed.origin_detail,ORIGIN_DETAIL_MODE) \
 X(indexed.origin_gate_a,ORIGIN_GATE_A) X(indexed.origin_gate_b,ORIGIN_GATE_B) \
 X(indexed.recorder_mode,RECORDER_MODE) X(indexed.pose_entry,SCENE_POSE_ENTRY)
#define FLIGHT_BYTES(X) X(context_started,CONTEXT_STARTED) X(pause,PAUSE_A)
#define VIEW_BYTES(X) X(detail_index,ORIGIN_DETAIL_INDEX) X(fire_state,FIRE_STATE)
#define VIEW_WORDS(X) X(line_last_row,LINE_LAST_ROW) X(emitted_requests,PENDING_COMMAND_WORD_B)
#define CONTEXT_BYTES(X) X(view_request,0xc45833u) X(track_started,TRACK_STARTED) X(recorder_on,RECORDER_ON)
#define CONTEXT_WORDS(X) X(angle_history,ORIGIN_ANGLE_HISTORY) X(pan,VIEW_PAN) X(rotate,VIEW_ROTATE)
#define CONTEXT_LONGS(X) \
 X(origin_first,SELECTOR_ORIGIN) X(origin_third,SELECTOR_ORIGIN_THIRD) X(map_middle_cache,0xc45664u) \
 X(negated[0],ORIGIN_NEGATED_COMPANION) X(negated[1],ORIGIN_NEGATED_COMPANION+4) X(negated[2],ORIGIN_NEGATED_COMPANION+8) \
 X(smoothed_delta,ORIGIN_SMOOTHED_DELTA) X(auxiliary_delta[0],ORIGIN_AUXILIARY_DELTA) X(auxiliary_delta[1],ORIGIN_AUXILIARY_DELTA+4)

typedef struct {
    FA18CommandInput commands;
    FA18FlightCommandState flight;
    FA18ViewCommandState view;
    FA18ContextCommandState context;
    FA18FlightCommandRecord aircraft[5];
    FA18ContextCommandRecord records[5];
    uint32_t addresses[5];
    uint8_t taken,recording[4];
} NativeContext;
typedef struct { NativeContext state; unsigned view,recording_active; } ContextSnapshot;
typedef struct {
    enum ContextCommandChild child;
    FA18ContextCommandChildInput input;
    FA18ContextCommandChildResult result;
    ContextSnapshot before;
} ContextChild;
typedef struct { NativeContext *state; unsigned count,scenario; ContextChild children[2]; } ContextTrace;
static ContextTrace *active_trace;

static unsigned record_index(const NativeContext *s,const FA18ContextCommandRecord *record) {
    unsigned i;
    for(i=0;i<5;++i) if(record==&s->records[i]) return i;
    fprintf(stderr,"unresolved context record\n"); exit(1);
}
static unsigned view_index(const NativeContext *s) {
    unsigned i;
    for(i=0;i<5;++i) if(s->flight.viewed==&s->aircraft[i]) return i;
    fprintf(stderr,"unresolved context view\n"); exit(1);
}
static void snapshot(const NativeContext *s,ContextSnapshot *out) {
    unsigned i;
    memset(out,0,sizeof *out); out->state=*s;
    out->view=view_index(s); out->recording_active=s->context.recording_write!=NULL;
    out->state.flight.commands=NULL; out->state.flight.viewed=NULL;
    out->state.view.flight=NULL; out->state.context.view=NULL;
    out->state.context.records=NULL; out->state.context.key_taken=NULL;
    out->state.context.recording_write=NULL;
    for(i=0;i<5;++i) out->state.records[i].command_record=NULL;
}
static ContextChild *begin_child(FA18ContextCommandState *state,enum ContextCommandChild child,
                                  const FA18ContextCommandChildInput *input) {
    ContextChild *row;
    if(!active_trace || state!=&active_trace->state->context || active_trace->count>=2) return NULL;
    row=&active_trace->children[active_trace->count++]; row->child=child; row->input=*input;
    snapshot(active_trace->state,&row->before); return row;
}
static int control(FA18ContextCommandState *state,enum ContextCommandChild child,
                     const FA18ContextCommandChildInput *input,FA18ContextCommandChildResult *result) {
    ContextChild *row=begin_child(state,child,input);
    if(!row || !fa18_apply_context_control_child(state,child,input,result)) return 0;
    row->result=*result; return 1;
}
#define fa18_apply_context_control_child control
#include "../../port/context_command_input.c"
#undef fa18_apply_context_control_child

static void load(NativeContext *s,const uint32_t *addresses) {
    unsigned i,row,column,view=5;
    uint32_t viewed=CONTROL_RECORDS+(uint32_t)(int32_t)rd_s16(VIEW_RECORD);
    memset(s,0,sizeof *s); memcpy(s->addresses,addresses,sizeof s->addresses);
#define FIELD(name,address) s->commands.name=rd_u8(address);
    COMMAND_BYTES(FIELD)
#undef FIELD
#define FIELD(name,address) s->flight.name=rd_u8(address);
    FLIGHT_BYTES(FIELD)
#undef FIELD
#define FIELD(name,address) s->view.name=rd_u8(address);
    VIEW_BYTES(FIELD)
#undef FIELD
#define FIELD(name,address) s->view.name=rd_u16(address);
    VIEW_WORDS(FIELD)
#undef FIELD
#define FIELD(name,address) s->context.name=rd_u8(address);
    CONTEXT_BYTES(FIELD)
#undef FIELD
#define FIELD(name,address) s->context.name=rd_u16(address);
    CONTEXT_WORDS(FIELD)
#undef FIELD
#define FIELD(name,address) s->context.name=rd_u32(address);
    CONTEXT_LONGS(FIELD)
#undef FIELD
    s->flight.command_word=rd_u16(COMMAND_WORD);
    s->view.origin_middle=rd_u32(SELECTOR_ORIGIN_MIDDLE); s->taken=rd_u8(KEY_TAKEN);
    for(i=0;i<4;++i) s->recording[i]=rd_u8(0xc61000u+i);
    for(i=0;i<5;++i) {
        s->aircraft[i].equipment_kind=rd_u8(addresses[i]+0x62);
        s->records[i].command_record=&s->aircraft[i]; s->records[i].angle=rd_u16(addresses[i]+0x68);
        for(row=0;row<3;++row) {
            s->records[i].position[row]=rd_u32(addresses[i]+0x14+4*row);
            for(column=0;column<3;++column) s->records[i].inverse[row][column]=rd_s16(addresses[i]+0x92+6*row+2*column);
        }
        if(viewed==addresses[i]) view=i;
    }
    if(view==5) exit(1);
    s->flight.commands=&s->commands; s->flight.viewed=&s->aircraft[view]; s->view.flight=&s->flight;
    s->context.view=&s->view; s->context.records=s->records; s->context.record_count=5; s->context.key_taken=&s->taken;
    if((int32_t)rd_u32(RECORDER_CURSOR)>0) {
        s->context.recording_write=s->recording; s->context.recording_remaining=4;
    }
}
static void store(const NativeContext *s) {
    unsigned i;
#define FIELD(name,address) wr_u8(address,s->commands.name);
    COMMAND_BYTES(FIELD)
#undef FIELD
#define FIELD(name,address) wr_u8(address,s->flight.name);
    FLIGHT_BYTES(FIELD)
#undef FIELD
#define FIELD(name,address) wr_u8(address,s->view.name);
    VIEW_BYTES(FIELD)
#undef FIELD
#define FIELD(name,address) wr_u16(address,s->view.name);
    VIEW_WORDS(FIELD)
#undef FIELD
#define FIELD(name,address) wr_u8(address,s->context.name);
    CONTEXT_BYTES(FIELD)
#undef FIELD
#define FIELD(name,address) wr_u16(address,s->context.name);
    CONTEXT_WORDS(FIELD)
#undef FIELD
#define FIELD(name,address) wr_u32(address,s->context.name);
    CONTEXT_LONGS(FIELD)
#undef FIELD
    wr_u16(COMMAND_WORD,s->flight.command_word); wr_u32(SELECTOR_ORIGIN_MIDDLE,s->view.origin_middle);
    wr_u8(KEY_TAKEN,s->taken); wr_u16(VIEW_RECORD,(uint16_t)(s->addresses[view_index(s)]-CONTROL_RECORDS));
    for(i=0;i<4;++i) wr_u8(0xc61000u+i,s->recording[i]);
}
/* Test-only audio contracts stress source reads after voice release. */
static void voice_effects(NativeContext *s,enum ContextCommandChild child,unsigned scenario) {
    if(child==CONTEXT_COMMAND_MAP_VOICES) {
        s->context.map_middle_cache^=0x07654321;
        if(scenario&512) s->flight.viewed=&s->aircraft[2];
    } else {
        s->commands.indexed.recorder_mode=(scenario&1)?1:0;
        s->context.recorder_on=(scenario&2)?1:0;
    }
}
static int voices(void *context,FA18ContextCommandState *state,enum ContextCommandChild child,
                    const FA18ContextCommandChildInput *input,FA18ContextCommandChildResult *result) {
    ContextChild *row=begin_child(state,child,input);
    (void)context;
    if(!row || (child!=CONTEXT_COMMAND_MAP_VOICES && child!=CONTEXT_COMMAND_REQUEST_VOICES)) return 0;
    voice_effects(active_trace->state,child,active_trace->scenario);
    memset(result,0,sizeof *result); result->event=input->event^0x13579bdf;
    row->result=*result; return 1;
}

static const struct {uint32_t entry,ret;} edges[]={
    {0xc091e0,0xc1b720},{0xc0915a,0xc1b762},{0xc0f4a6,0xc1bfbc},{0xc0f4a6,0xc1c09a}
};
static unsigned char control_visited[sizeof source_bytes/sizeof source_bytes[0]];
static int original(ContextTrace *trace) {
    unsigned step,next=0,i;
    uint32_t active_return=0;
    for(step=0;step<10000;++step) {
        uint32_t pc=REG_PC;
        if(active_return && pc==active_return && REG_A[7]==0xc7ff00u) {
            ContextChild *row=&trace->children[next-1];
            if(REG_D[0]!=row->result.event || REG_D[1]!=(uint32_t)row->result.position[1] ||
               REG_D[2]!=(uint32_t)row->result.position[2]) return 0;
            active_return=0;
        }
        if(!active_return && pc==0xc1c23c) return next==trace->count;
        if(!active_return && next<trace->count) {
            ContextChild *row=&trace->children[next];
            if(pc==edges[row->child].entry && rd_u32(REG_A[7])==edges[row->child].ret) {
                NativeContext current; ContextSnapshot got;
                load(&current,trace->state->addresses); snapshot(&current,&got);
                if(memcmp(&got,&row->before,sizeof got)) { fprintf(stderr,"context child state mismatch %06X\n",pc); return 0; }
                ++next;
                if(row->child==CONTEXT_COMMAND_LOCAL_TO_WORLD) {
                    if(REG_A[1]!=trace->state->addresses[record_index(trace->state,row->input.record)] ||
                       (int16_t)REG_D[3]!=row->input.local[0] || (int16_t)REG_D[4]!=row->input.local[1] ||
                       (int16_t)REG_D[5]!=row->input.local[2]) return 0;
                    active_return=edges[row->child].ret;
                } else if(row->child==CONTEXT_COMMAND_SET_OBSERVER) {
                    for(i=0;i<3;++i) if(REG_D[i]!=(uint32_t)row->input.position[i]) return 0;
                    active_return=edges[row->child].ret;
                } else {
                    if(REG_D[0]!=row->input.event) return 0;
                    voice_effects(&current,row->child,trace->scenario); store(&current);
                    REG_D[0]=row->result.event; REG_PC=rd_u32(REG_A[7]); REG_A[7]+=4; continue;
                }
            }
        }
        if(!active_return && !source_parent_pc(pc)) return 0;
        if(!active_return) visited[pc-action_base]=1;
        else {
            for(i=0;i<sizeof source_bytes/sizeof source_bytes[0];++i) if(pc==source_bytes[i].pc) break;
            if(i==sizeof source_bytes/sizeof source_bytes[0]) return 0;
            control_visited[i]=1;
        }
        { uint16_t opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
          m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]); }
    }
    return 0;
}
int main(int argc,char **argv) {
    static const int8_t pose_indices[]={-128,-1,0,1,2,3,127};
    static const uint16_t record_poses[]={0x8000,0x8001,0x8002,0x8003,0x8040,0x8080,0xffff};
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base);
    uint8_t *before=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE),*expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):4096,scenario,row,byte,real_children=0,audio_children=0;
    char error[256];
    if(argc>2) selected_entry=(uint32_t)strtoul(argv[2],NULL,16);
    for(selected_action=COMMAND_PENDING_EMPTY;selected_action<=COMMAND_INDEXED;++selected_action)
        if(glue_command_action_pc(selected_action)==selected_entry) break;
    if(!state || !rom || !m || !base || !before || !expected || !cases ||
       !fa18_is_context_input_command(selected_action)) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) { fprintf(stderr,"%s\n",error); return 1; }
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(row=0;row<sizeof source_bytes/sizeof source_bytes[0];++row)
        for(byte=0;byte<source_bytes[row].length;++byte)
            if(rd_u8(source_bytes[row].pc+byte)!=source_bytes[row].bytes[byte]) return 1;
    for(scenario=0;scenario<cases;++scenario) {
        NativeContext native; ContextTrace trace; uint32_t event,addresses[5],selected_record,a2;
        FA18ContextCommandPose entries[256]; FA18ContextCommandPoses poses={entries,256,-128};
        FA18ContextCommandOps ops={voices,NULL}; CommandRequest request;
        unsigned profile=scenario/32,i,column; int index=pose_indices[(profile/4)%7];
        uint16_t pose_word=(profile&32)?record_poses[profile%7]:0;
        uint32_t pose_address=SCENE_POSE_TABLE+(uint32_t)(int32_t)(index*16);
        uint32_t preset_address=0xc42a54u+(uint32_t)(int32_t)(index*16);
        int16_t values[6],grid_offset;
        memcpy(m,base,sizeof *m); fixture(scenario);
        for(i=0;i<6;++i) values[i]=rd_s16(0xc42a54u+(profile%4)*16+2*i);
        wr_u8(SCENE_POSE_ENTRY,(uint8_t)index); wr_u16(pose_address,pose_word);
        for(i=0;i<6;++i) wr_u16(preset_address+2*i,(uint16_t)values[i]);
        if(scenario&512) wr_u32(RECORDER_CURSOR,0x80000000);
        for(i=0;i<4;++i) addresses[i]=CONTROL_RECORDS+512*i;
        selected_record=CONTROL_RECORDS+(uint32_t)(int32_t)(int16_t)((pose_word&0x7fff)*512u);
        addresses[4]=selected_record;
        for(i=0;i<4;++i) if(addresses[4]==addresses[i]) addresses[4]=CONTROL_RECORDS+0x800;
        for(i=0;i<5;++i) {
            wr_u16(addresses[i]+0x68,(uint16_t)random_value());
            for(row=0;row<3;++row) {
                wr_u32(addresses[i]+0x14+4*row,random_value());
                for(column=0;column<3;++column) wr_u16(addresses[i]+0x92+6*row+2*column,(uint16_t)random_value());
            }
        }
        load(&native,addresses); memset(entries,0,sizeof entries); memset(&trace,0,sizeof trace);
        trace.state=&native; trace.scenario=scenario; active_trace=&trace;
        if(pose_word&0x8000) {
            entries[index+128].kind=FA18_CONTEXT_POSE_RECORD;
            for(i=0;i<5;++i) if(addresses[i]==selected_record) entries[index+128].record=&native.records[i];
            if(!entries[index+128].record) return 1;
        } else {
            entries[index+128].kind=FA18_CONTEXT_POSE_PRESET;
            memcpy(entries[index+128].preset,values,sizeof values);
            grid_offset=(int16_t)((uint16_t)values[2]*4u);
            entries[index+128].grid_pair[0]=rd_s16(GRID_ADJUST_WORDS+(uint32_t)(int32_t)grid_offset);
            entries[index+128].grid_pair[1]=rd_s16(GRID_ADJUST_WORDS+(uint32_t)(int32_t)grid_offset+2);
        }
        request=(CommandRequest){selected_action,REG_D[0],(uint8_t)REG_D[5],(uint8_t)REG_D[6],0,0}; a2=REG_A[2];
        memcpy(before,m->chip,FA18_CHIP_SIZE); memcpy(before+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        if(!fa18_apply_context_input_command(&native.context,&request,&poses,&ops,&event)) return 1;
        store(&native);
        for(row=0;row<trace.count;++row) {
            ContextChild *child=&trace.children[row]; uint32_t sp=0xc7ff00;
            if(child->child>=CONTEXT_COMMAND_MAP_VOICES) { sp-=2; wr_u16(sp,(uint16_t)request.raw_event); ++audio_children; }
            else ++real_children;
            sp-=4; wr_u32(sp,edges[child->child].ret);
            if(child->child==CONTEXT_COMMAND_LOCAL_TO_WORLD) {
                wr_u32(sp-4,a2); wr_u32(sp-8,addresses[record_index(&native,child->input.record)]);
            }
        }
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m->chip,before,FA18_CHIP_SIZE); memcpy(m->slow,before+FA18_CHIP_SIZE,FA18_SLOW_SIZE);
        if(!original(&trace) || REG_D[0]!=event || REG_A[7]!=0xc7ff00 ||
           memcmp(m->chip,expected,FA18_CHIP_SIZE) || memcmp(m->slow,expected+FA18_CHIP_SIZE,FA18_SLOW_SIZE)) {
            unsigned offset;
            fprintf(stderr,"native context %06X case %u: PC %06X event %08X/%08X SP %06X\n",selected_entry,scenario,REG_PC,REG_D[0],event,REG_A[7]);
            for(offset=0;offset<FA18_SLOW_SIZE;++offset) if(m->slow[offset]!=expected[FA18_CHIP_SIZE+offset]) {
                fprintf(stderr,"RAM %06X original %02X native %02X\n",FA18_SLOW_BASE+offset,m->slow[offset],expected[FA18_CHIP_SIZE+offset]); break;
            }
            return 1;
        }
    }
    printf("native context %06X: %u actions matched full RAM, event and ordered child inputs/state; %u actual geometry/observer, %u contracted audio children\n",selected_entry,cases,real_children,audio_children);
    printf("visited:"); for(row=0;row<sizeof visited;++row) if(visited[row]) printf(" %06X",action_base+row); putchar('\n');
    printf("control_visited:"); for(row=0;row<sizeof control_visited;++row) if(control_visited[row]) printf(" %06X",source_bytes[row].pc); putchar('\n');
    free(expected); free(before); free(base); free(m); return 0;
}
