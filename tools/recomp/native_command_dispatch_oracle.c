/* Complete native parents versus sealed original instructions. Validation only. */
#define main reference_command_dispatch_validation_main
#include "command_dispatch_oracle.c"
#undef main
#include "glue_command_selection.h"
#include "../../port/command_dispatch.h"
#include "../../port/command_input.c"
#include "../../port/indexed_controls.c"
#include "../../port/flight_command_input.c"
#include "../../port/view_command_controls.c"
#include "../../port/context_command_controls.c"
#include "../../port/command_queue.c"
#include "../../port/input_callback_registration.c"
#include "../../port/command_dispatch_controls.c"
#include "native_command_dispatch_fields.h"
#include "../../build/recomp/native_command_dispatch_source.h"

typedef struct {
    FA18CommandInput c;
    FA18FlightCommandState f;
    FA18ViewCommandState v;
    FA18ContextCommandState context;
    FA18CommandQueue q;
    FA18NativeCommandDispatcher dispatch;
    FA18NativeInputDescriptor descriptor;
    FA18InputCallbackRegistration registration;
    FA18FlightCommandRecord aircraft[12];
    FA18ContextCommandRecord records[12];
    uint8_t recording[4];
} Native;

enum { CHILD_FLIGHT, CHILD_VIEW, CHILD_CONTEXT, CHILD_TONE, CHILD_SERVICE };
typedef struct {
    unsigned family,child;
    uint32_t entry,ret;
    int real;
    FA18FlightCommandChildInput flight;
    FA18ContextCommandChildInput context;
    uint32_t event;
    FlightCommandResult flight_result;
    FA18ContextCommandChildResult context_result;
} Child;
typedef struct {
    Native *native;
    unsigned scenario,count,record_count;
    uint32_t addresses[12],command_event,a2;
    int16_t carry;
    CommandRequest request;
    Child children[8];
} Trace;
static Trace *trace;
static uint8_t boundary_ram[8][FA18_CHIP_SIZE+FA18_SLOW_SIZE];
static char original_name[128];
/* Identity for the fixture descriptor, never executed as a game callback. */
static void fixture_callback(void *context) { (void)context; abort(); }

static const struct {uint32_t entry,ret;} flight_edges[]={
    {0xc1c214,0xc1b14a},{0xc33186,0xc1b1aa},{0xc33186,0xc1b1ce},
    {0xc0833e,0xc1b228},{0xc08394,0xc1b232},
    {0xc1b50c,0xc1b4f8},{0xc1b510,0xc1b500},{0xc1b514,0xc1b508},
    {0xc1b558,0xc1b544},{0xc1b55c,0xc1b54c},{0xc1b560,0xc1b554},
    {0xc1b602,0xc1b5d8},{0xc1b602,0xc1b5e0},{0xc33186,0xc1b5e6},
    {0xc33186,0xc1b630},{0xc25704,0xc1b65e},
    {0xc33186,0xc1bb9c},{0xc25704,0xc1bbb4},{0xc33186,0xc1bbc0},
    {0xc33186,0xc1bc4c},{0xc33186,0xc1c062},
    {0xc25704,0xc1c11c},{0xc17f8c,0xc1c16a},{0xc25704,0xc1c1aa},{0xc33186,0xc1c20e}
};
static const struct {uint32_t entry,ret;} context_edges[]={
    {0xc091e0,0xc1b720},{0xc0915a,0xc1b762},{0xc0f4a6,0xc1bfbc},{0xc0f4a6,0xc1c09a}
};

static unsigned find_record(uint32_t address) {
    unsigned i;
    for(i=0;i<trace->record_count;++i) if(trace->addresses[i]==address) return i;
    fprintf(stderr,"unresolved fixture record %06X\n",address); exit(1);
}
static unsigned native_record(const Native *s,const FA18ContextCommandRecord *record) {
    unsigned i;
    for(i=0;i<trace->record_count;++i) if(record==&s->records[i]) return i;
    abort();
}
static unsigned viewed_record(const Native *s) {
    unsigned i;
    for(i=0;i<trace->record_count;++i) if(s->f.viewed==&s->aircraft[i]) return i;
    abort();
}
static void load_native(Native *s) {
    unsigned i,r,col;
    uint8_t neighbors[FA18_COMMAND_QUEUE_NEIGHBORS],keys[FA18_COMMAND_KEY_TABLE_SIZE];
    memset(s,0,sizeof *s);
#define FIELD(name,address) s->name=rd_u8(address);
    DISPATCH_B_FIELDS(FIELD)
#undef FIELD
#define FIELD(name,address) s->name=rd_u16(address);
    DISPATCH_W_FIELDS(FIELD)
#undef FIELD
#define FIELD(name,address) s->name=rd_u32(address);
    DISPATCH_L_FIELDS(FIELD)
#undef FIELD
    for(i=0;i<8;++i) s->c.indexed.modes.available[i]=rd_u8(rd_u32(MODE_TABLE)+0x12+i);
    for(i=0;i<trace->record_count;++i) {
        uint32_t a=trace->addresses[i];
        s->aircraft[i].flags=rd_u16(a); s->aircraft[i].secondary_flags=rd_u16(a+2);
        s->aircraft[i].equipment_kind=rd_u8(a+0x62); s->aircraft[i].weapon_radar=rd_u8(a+0x63);
        s->aircraft[i].stick=rd_u8(a+0x65); s->records[i].angle=rd_u16(a+0x68);
        s->records[i].command_record=&s->aircraft[i];
        for(r=0;r<3;++r) {
            s->records[i].position[r]=rd_u32(a+0x14+4*r);
            for(col=0;col<3;++col) s->records[i].inverse[r][col]=rd_s16(a+0x92+6*r+2*col);
        }
    }
    s->f.commands=&s->c; s->f.player=&s->aircraft[0]; s->f.target=&s->aircraft[4];
    s->f.viewed=&s->aircraft[find_record(CONTROL_RECORDS+(uint32_t)(int32_t)rd_s16(VIEW_RECORD))];
    for(i=0;i<3;++i) s->f.spawn_slots[i]=&s->aircraft[i+1];
    s->v.flight=&s->f; s->context.view=&s->v; s->context.records=s->records;
    s->context.record_count=trace->record_count;
    for(i=0;i<4;++i) s->recording[i]=rd_u8(0xc61400+i);
    if((int32_t)rd_u32(RECORDER_CURSOR)>0) {
        s->context.recording_write=s->recording; s->context.recording_remaining=4;
    }
    for(i=0;i<sizeof neighbors;++i) neighbors[i]=rd_u8(KEY_RAW-128u+i);
    for(i=0;i<sizeof keys;++i) keys[i]=rd_u8(KEY_TABLE+i);
    if(!fa18_initialize_command_queue(&s->q,&s->context,neighbors,sizeof neighbors,keys,sizeof keys)) abort();
    s->descriptor.type=rd_u8(0xc1abf8); s->descriptor.priority=rd_s8(0xc1abf9);
    s->descriptor.name=original_name; s->descriptor.callback=fixture_callback;
    s->registration.descriptor=&s->descriptor; s->registration.name=original_name;
    s->registration.callback=fixture_callback;
    s->dispatch.context=&s->context; s->dispatch.queue=&s->q;
    s->dispatch.input_registration=&s->registration;
}
static void store_native(const Native *s) {
    unsigned i;
#define FIELD(name,address) wr_u8(address,s->name);
    DISPATCH_B_FIELDS(FIELD)
#undef FIELD
#define FIELD(name,address) wr_u16(address,(uint16_t)s->name);
    DISPATCH_W_FIELDS(FIELD)
#undef FIELD
#define FIELD(name,address) wr_u32(address,s->name);
    DISPATCH_L_FIELDS(FIELD)
#undef FIELD
    for(i=0;i<8;++i) wr_u8(rd_u32(MODE_TABLE)+0x12+i,s->c.indexed.modes.available[i]);
    for(i=0;i<trace->record_count;++i) {
        uint32_t a=trace->addresses[i];
        wr_u16(a,s->aircraft[i].flags); wr_u16(a+2,s->aircraft[i].secondary_flags);
        wr_u8(a+0x62,s->aircraft[i].equipment_kind); wr_u8(a+0x63,s->aircraft[i].weapon_radar);
        wr_u8(a+0x65,s->aircraft[i].stick);
    }
    wr_u16(VIEW_RECORD,(uint16_t)(trace->addresses[viewed_record(s)]-CONTROL_RECORDS));
    for(i=0;i<4;++i) wr_u8(0xc61400+i,s->recording[i]);
    for(i=0;i<FA18_COMMAND_QUEUE_NEIGHBORS;++i) {
        const FA18CommandQueueByte *slot=&s->q.slots[i];
        wr_u8(KEY_RAW-128u+i,slot->byte ? *slot->byte : (uint8_t)((uint16_t)*slot->word>>slot->shift));
    }
    wr_u8(0xc1abf8,s->descriptor.type); wr_u8(0xc1abf9,(uint8_t)s->descriptor.priority);
    wr_u32(0xc1abfa,0xc081b4); wr_u32(0xc1ac02,0xc1718e);
}

static Child *begin_child(unsigned family,unsigned child,uint32_t entry,uint32_t ret,int real) {
    Child *row;
    uint32_t sp=0xc7ff00;
    if(trace->count>=8) abort();
    row=&trace->children[trace->count]; memset(row,0,sizeof *row);
    row->family=family; row->child=child; row->entry=entry; row->ret=ret; row->real=real;
    store_native(trace->native);
    /* Original ABI writes are confined to validation, never the native API. */
    if(family==CHILD_CONTEXT && child>=CONTEXT_COMMAND_MAP_VOICES) {
        sp-=2; wr_u16(sp,(uint16_t)trace->command_event);
    }
    if(family==CHILD_FLIGHT && child==FLIGHT_FLARE_SPAWN) {
        sp-=2; wr_u16(sp,(uint16_t)trace->command_event);
        sp-=4; wr_u32(sp,0x30); sp-=4; wr_u32(sp,0x1c);
    }
    if(family==CHILD_SERVICE) {
        wr_u32(sp-4,child==FA18_INPUT_CALLBACK_REMOVE?0xc06bf6:0xc06c00);
        sp-=8; wr_u32(sp,0xc1abf0); sp-=4; wr_u32(sp,5);
    }
    sp-=4; wr_u32(sp,ret);
    memcpy(boundary_ram[trace->count],fa18_machine->chip,FA18_CHIP_SIZE);
    memcpy(boundary_ram[trace->count]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
    ++trace->count; return row;
}
static int native_flight(FA18NativeCommandDispatcher *s,enum FlightCommandChild child,
                          const FA18FlightCommandChildInput *input,FlightCommandResult *result) {
    Child *row=begin_child(CHILD_FLIGHT,child,flight_edges[child].entry,flight_edges[child].ret,1);
    row->flight=*input;
    if(!fa18_apply_native_flight_child(s,child,input,result)) return 0;
    row->flight_result=*result; return 1;
}
/* Explicit test contracts for still-unimplemented audio/space/spawn children. */
static void flight_effects(Native *s,unsigned child,unsigned scenario) {
    if(child==FLIGHT_RADAR_RANGE) s->f.viewed=&s->aircraft[(scenario&64)?2:0];
    if(child==FLIGHT_WEAPON_MODE) s->c.block_flags=(scenario&64)?0x0f:0;
    if(child==FLIGHT_ECM) s->f.ecm_enabled=(scenario&64)?0:0x80;
    if(child==FLIGHT_HOOK) s->f.player->secondary_flags^=0x8080;
}
static int flight_contract(void *context,FA18FlightCommandState *s,enum FlightCommandChild child,
                             const FA18FlightCommandChildInput *input,FlightCommandResult *result) {
    Child *row=begin_child(CHILD_FLIGHT,child,flight_edges[child].entry,flight_edges[child].ret,0);
    (void)context; if(s!=&trace->native->f) return 0;
    row->flight=*input; flight_effects(trace->native,child,trace->scenario);
    result->event=input->event^(0x13579bdfu+child*0x10001u);
    result->carried_event_word=(int16_t)((uint16_t)input->restore_event_word^0xa55a);
    row->flight_result=*result; return 1;
}
static int native_zoom(FA18ViewCommandState *s) {
    begin_child(CHILD_VIEW,0,0xc08324,0xc1b9e0,1);
    return fa18_set_native_zoom_maximum(s);
}
static int native_redraw(FA18ViewCommandState *s) {
    begin_child(CHILD_VIEW,1,0xc082b8,0xc1ba8c,1);
    return fa18_request_native_cockpit_redraw(s);
}
#define fa18_set_native_zoom_maximum native_zoom
#define fa18_request_native_cockpit_redraw native_redraw
#include "../../port/view_command_input.c"
#undef fa18_set_native_zoom_maximum
#undef fa18_request_native_cockpit_redraw

static int native_context(FA18ContextCommandState *s,enum ContextCommandChild child,
                           const FA18ContextCommandChildInput *input,FA18ContextCommandChildResult *result) {
    Child *row=begin_child(CHILD_CONTEXT,child,context_edges[child].entry,context_edges[child].ret,1);
    row->context=*input;
    if(!fa18_apply_context_control_child(s,child,input,result)) return 0;
    row->context_result=*result;
    if(child==CONTEXT_COMMAND_LOCAL_TO_WORLD) {
        /* MOVEM saves A2 then A1 inside the actual geometry child. */
        wr_u32(0xc7fef8,trace->a2);
        wr_u32(0xc7fef4,trace->addresses[native_record(trace->native,input->record)]);
    }
    return 1;
}
#define invoke context_invoke
#define fa18_apply_context_control_child native_context
#include "../../port/context_command_input.c"
#undef invoke
#undef fa18_apply_context_control_child

static void voice_effects(Native *s,unsigned child,unsigned scenario) {
    if(child==CONTEXT_COMMAND_MAP_VOICES) {
        s->context.map_middle_cache^=0x07654321;
        if(scenario&64) s->f.viewed=&s->aircraft[2];
    } else {
        s->c.indexed.recorder_mode=(scenario&1)?1:0;
        s->context.recorder_on=(scenario&2)?1:0;
    }
}
static int voice_contract(void *context,FA18ContextCommandState *s,enum ContextCommandChild child,
                            const FA18ContextCommandChildInput *input,FA18ContextCommandChildResult *result) {
    Child *row=begin_child(CHILD_CONTEXT,child,context_edges[child].entry,context_edges[child].ret,0);
    (void)context; if(s!=&trace->native->context) return 0;
    row->context=*input; voice_effects(trace->native,child,trace->scenario);
    memset(result,0,sizeof *result); result->event=input->event^0x13579bdf;
    row->context_result=*result; return 1;
}
static void tone_effects(Native *s) { s->c.indexed.pose_entry^=0x40; s->c.indexed.function_modifier=0x80; }
static uint32_t tone_contract(void *context,FA18IndexedControls *s) {
    Child *row=begin_child(CHILD_TONE,0,0xc3318e,0xc1bdf8,0);
    (void)context; if(s!=&trace->native->c.indexed) abort();
    tone_effects(trace->native); row->event=0xabcde024u^trace->scenario; return row->event;
}
static int registration_contract(void *context,FA18InputCallbackOperation operation,
                                  unsigned kind,FA18NativeInputDescriptor *descriptor) {
    Child *row=begin_child(CHILD_SERVICE,operation,operation==FA18_INPUT_CALLBACK_REMOVE?0xc53b18:0xc53b00,
                            operation==FA18_INPUT_CALLBACK_REMOVE?0xc1749c:0xc17488,0);
    (void)context; (void)row;
    return kind==5 && descriptor==&trace->native->descriptor;
}
static int select_keyboard(FA18CommandInput *s,uint32_t event,int16_t *carry,CommandRequest *r) {
    if(!fa18_select_keyboard_command_with_carry(s,event,carry,r)) return 0;
    trace->request=*r; trace->carry=*carry; trace->command_event=r->raw_event; return 1;
}
static int select_pending(FA18CommandInput *s,CommandRequest *r) {
    if(!fa18_select_pending_command(s,r)) return 0;
    trace->request=*r; trace->command_event=r->raw_event; return 1;
}
#define fa18_select_keyboard_command_with_carry select_keyboard
#define fa18_select_pending_command select_pending
#define fa18_apply_native_flight_child native_flight
#include "../../port/command_dispatch.c"
#undef fa18_select_keyboard_command_with_carry
#undef fa18_select_pending_command
#undef fa18_apply_native_flight_child

static int same_ram(const uint8_t *expected) {
    return !memcmp(expected,fa18_machine->chip,FA18_CHIP_SIZE) &&
           !memcmp(expected+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
}
static unsigned char extra_visited[sizeof source_bytes/sizeof source_bytes[0]];
static int original_parent(Trace *t) {
    unsigned step,next=0,i;
    int selected=0;
    for(step=0;step<10000;++step) {
        uint32_t pc=REG_PC;
        int owner=source_index(pc);
        if(pc==0xc70000 && REG_A[7]==expected_sp) return selected && next==t->count;
        if(!selected && !(selected_entry==0xc1ac28 ? pc>=0xc1ac28 && pc<0xc1ad70 : pc>=0xc1ad74 && pc<0xc1b126)) {
            if(pc!=glue_command_action_pc(t->request.action) || (int16_t)REG_D[4]!=t->carry) {
                fprintf(stderr,"selection PC/carry %06X/%06X %04X/%04X\n",pc,glue_command_action_pc(t->request.action),(uint16_t)REG_D[4],(uint16_t)t->carry); return 0;
            }
            if(t->request.action!=COMMAND_INVALID_WORD && REG_D[0]!=t->request.raw_event) return 0;
            selected=1;
        }
        if(next<t->count && pc==t->children[next].entry) {
            Child *row=&t->children[next];
            if(rd_u32(REG_A[7])!=row->ret || !same_ram(boundary_ram[next])) {
                fprintf(stderr,"case %u child %u family %u entry %06X state/return mismatch\n",t->scenario,next,row->family,pc); return 0;
            }
            if(row->family==CHILD_FLIGHT && (REG_D[0]!=row->flight.event ||
                ((row->child==FLIGHT_FLARE_SOUND || row->child==FLIGHT_CHAFF_SOUND) && (int16_t)REG_D[4]!=row->flight.restore_event_word) ||
                (row->child==FLIGHT_FLARE_SPAWN && (rd_u32(REG_A[7]+4)!=(uint32_t)row->flight.arguments[0] || rd_u32(REG_A[7]+8)!=(uint32_t)row->flight.arguments[1])))) return 0;
            if(row->family==CHILD_CONTEXT) {
                if(row->child==CONTEXT_COMMAND_LOCAL_TO_WORLD) {
                    if(REG_A[1]!=t->addresses[native_record(t->native,row->context.record)] || (int16_t)REG_D[3]!=row->context.local[0] ||
                       (int16_t)REG_D[4]!=row->context.local[1] || (int16_t)REG_D[5]!=row->context.local[2]) return 0;
                } else if(row->child==CONTEXT_COMMAND_SET_OBSERVER) {
                    for(i=0;i<3;++i) if(REG_D[i]!=(uint32_t)row->context.position[i]) return 0;
                } else if(REG_D[0]!=row->context.event) return 0;
            }
            ++next;
            if(!row->real) {
                Native current; load_native(&current);
                if(row->family==CHILD_FLIGHT) {
                    flight_effects(&current,row->child,t->scenario); REG_D[0]=row->flight_result.event;
                    REG_D[4]=(REG_D[4]&0xffff0000u)|(uint16_t)row->flight_result.carried_event_word;
                } else if(row->family==CHILD_CONTEXT) {
                    voice_effects(&current,row->child,t->scenario); REG_D[0]=row->context_result.event;
                } else if(row->family==CHILD_TONE) { tone_effects(&current); REG_D[0]=row->event; }
                else if(rd_u32(REG_A[7]+4)!=5 || rd_u32(REG_A[7]+8)!=0xc1abf0) return 0;
                store_native(&current); REG_PC=rd_u32(REG_A[7]); REG_A[7]+=4; continue;
            }
        }
        if(owner>=0) visited[owner]=1;
        else {
            for(i=0;i<sizeof source_bytes/sizeof source_bytes[0];++i) if(source_bytes[i].pc==pc) break;
            if(i==sizeof source_bytes/sizeof source_bytes[0]) { fprintf(stderr,"unexpected original PC %06X\n",pc); return 0; }
            extra_visited[i]=1;
        }
        { uint16_t opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
          m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]); }
    }
    return 0;
}

int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size),*rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base);
    uint8_t *before=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE),*expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,scenario,i,r,col;
    unsigned child_count=0; char error[256]; FA18ViewSpanOffsets spans;
    if(argc>2) selected_entry=(uint32_t)strtoul(argv[2],NULL,16);
    if(!state || !rom || !m || !base || !before || !expected || !cases ||
       !(selected_entry==0xc1ac28 || selected_entry==0xc1ad74)) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(i=0;i<sizeof source_bytes/sizeof source_bytes[0];++i)
        for(r=0;r<source_bytes[i].length;++r) if(rd_u8(source_bytes[i].pc+r)!=source_bytes[i].bytes[r]) return 1;
    for(i=0;i<sizeof original_name;++i) original_name[i]=(char)rd_u8(0xc081b4+i);
    for(i=0;i<256;++i) spans.values[i]=rd_s8(0xc1bad4-128+i);
    for(scenario=0;scenario<cases;++scenario) {
        Native native; Trace t={0}; FA18NativeCommandOutcome outcome;
        FA18ContextCommandPose poses[4]; int16_t indexed_values[64];
        FA18ContextCommandPoses context_poses={poses,4,0}; FA18IndexedControlPoses indexed_poses={indexed_values,64};
        FA18FlightCommandOps flight_ops={flight_contract,NULL}; FA18ContextCommandOps context_ops={voice_contract,NULL};
        FA18NativeCommandOwners owners={&indexed_poses,&spans,&context_poses,tone_contract,NULL,&flight_ops,&context_ops};
        memcpy(m,base,sizeof *m); fixture(scenario); trace=&t; t.native=&native; t.scenario=scenario; t.a2=REG_A[2]; t.carry=(int16_t)REG_D[4];
        wr_u8(PAUSE_A,(scenario&2048)?1:0); wr_u8(CONTEXT_STARTED,(scenario&4096)?1:0);
        wr_u8(BAR_E_FLAG,(scenario&512)?0x80:0);
        if(scenario&128) {
            wr_u8(KEY_WRITE,(uint8_t)(scenario*37u));
            wr_u8(KEY_TRANSLATED_WRITE,(uint8_t)(scenario*13u));
        }
        t.record_count=8; for(i=0;i<8;++i) t.addresses[i]=CONTROL_RECORDS+512*i;
        for(i=0;i<4;++i) wr_u16(SCENE_POSE_TABLE+16*i,(scenario&4096)?(uint16_t)(0x8000+i):0);
        wr_u32(RECORDER_CURSOR,(scenario&128)?0xc61400:0);
        wr_u8(0xc1abf8,7); wr_u8(0xc1abf9,0xfc); wr_u32(0xc1abfa,0xc081b4); wr_u32(0xc1ac02,0xc1718e);
        for(i=0;i<8;++i) {
            wr_u16(t.addresses[i]+0x68,(uint16_t)random_value());
            for(r=0;r<3;++r) {
                wr_u32(t.addresses[i]+0x14+4*r,random_value());
                for(col=0;col<3;++col) wr_u16(t.addresses[i]+0x92+6*r+2*col,(uint16_t)random_value());
            }
        }
        load_native(&native); native.registration.consume=registration_contract;
        memset(poses,0,sizeof poses);
        for(i=0;i<4;++i) {
            if(rd_u16(SCENE_POSE_TABLE+16*i)&0x8000) { poses[i].kind=FA18_CONTEXT_POSE_RECORD; poses[i].record=&native.records[i]; }
            else {
                int16_t offset;
                poses[i].kind=FA18_CONTEXT_POSE_PRESET;
                for(r=0;r<6;++r) poses[i].preset[r]=rd_s16(0xc42a54+16*i+2*r);
                offset=(int16_t)((uint16_t)poses[i].preset[2]*4u);
                poses[i].grid_pair[0]=rd_s16(GRID_ADJUST_WORDS+(uint32_t)(int32_t)offset);
                poses[i].grid_pair[1]=rd_s16(GRID_ADJUST_WORDS+(uint32_t)(int32_t)offset+2);
            }
        }
        for(i=0;i<64;++i) indexed_values[i]=rd_s16(SCENE_POSE_TABLE+16*i);
        memcpy(before,m->chip,FA18_CHIP_SIZE); memcpy(before+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        if(selected_entry==0xc1ad74) wr_u32(0xc7fefc,REG_A[6]);
        if(!(selected_entry==0xc1ad74 ? fa18_dispatch_native_keyboard_command(&native.dispatch,rd_u32(REG_A[7]+4),t.carry,&owners,&outcome) :
              fa18_dispatch_native_pending_command(&native.dispatch,t.carry,&owners,&outcome))) {
            fprintf(stderr,"native dispatch failed case %u action %u\n",scenario,t.request.action); return 1;
        }
        store_native(&native);
        if(outcome.completion==FA18_COMMAND_INVALID_PENDING) wr_u32(0xc7fefc,0xc1ac26);
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m->chip,before,FA18_CHIP_SIZE); memcpy(m->slow,before+FA18_CHIP_SIZE,FA18_SLOW_SIZE);
        if(!original_parent(&t) || (outcome.completion==FA18_COMMAND_PUBLISHED && outcome.event!=REG_D[0]) || !same_ram(expected)) {
            fprintf(stderr,"native dispatch %06X case %u PC %06X event %08X/%08X\n",selected_entry,scenario,REG_PC,REG_D[0],outcome.event);
            for(i=0;i<FA18_SLOW_SIZE;++i) if(m->slow[i]!=expected[FA18_CHIP_SIZE+i]) {
                fprintf(stderr,"RAM %06X source %02X native %02X\n",FA18_SLOW_BASE+i,m->slow[i],expected[FA18_CHIP_SIZE+i]); break;
            }
            return 1;
        }
        child_count+=t.count;
    }
    printf("native command dispatch %06X: %u complete parents matched full RAM, published events, selection carries and %u ordered child boundaries\n",selected_entry,cases,child_count);
    printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",owner_pcs[i]); putchar('\n');
    printf("control_visited:"); for(i=0;i<sizeof extra_visited;++i) if(extra_visited[i]) printf(" %06X",source_bytes[i].pc); putchar('\n');
    free(expected); free(before); free(base); free(m); return 0;
}
