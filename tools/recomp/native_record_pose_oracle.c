#define FA18_SCENE_SOURCE_HEADER "../../build/recomp/native_record_pose_source.h"
#define FA18_SCENE_PLAYER_HELPERS_ONLY
#include "native_scene_player_oracle.c"
#include "../../port/native_record_pose.c"

enum { HISTORY=0xc4fdd4,HISTORY_BYTES=256,HISTORY_ORIGIN=64,DAMAGE_COUNT=0xc65046 };
#define POSE_WORDS(X) \
 X(current_slot,0xc459b4) X(current_stride,0xc459b6) X(target_slot,0xc458dc) \
 X(selector_word,0xc458da) X(matrix_control,0xc458cc) X(shown_message,0xc45ae0) \
 X(grid_x,0xc4594c) X(grid_z,0xc4594e) X(error_word,0xc4599e) \
 X(collision_slot,0xc4fdd2) X(damage_count,DAMAGE_COUNT)
#define POSE_BYTES(X) \
 X(cell_only,0xc45788) X(post_input_event,0xc457ae) X(origin_enable,0xc45785) \
 X(activity_count,0xc458ab) X(scene_redraw,0xc45858) X(bar_redraw,0xc45845) \
 X(view_decay,0xc45847) X(collision_enable,0xc4578c) X(collision_inhibit,0xc4589a) \
 X(cockpit_a,0xc4584f) X(cockpit_b,0xc4584e) X(collision_report,0xc457c0) \
 X(mission_failure,0xc457c5) X(failure_view,0xc45798) X(request_flag,0xc4589f) \
 X(request_clear,0xc458b4) X(history_count,0xc4fdd0) X(history_index,0xc4fdd1)
typedef struct {
#define W(name,address) uint16_t name;
 POSE_WORDS(W)
#undef W
#define B(name,address) uint8_t name;
 POSE_BYTES(B)
#undef B
 uint32_t events;
} PoseGlobals;
static unsigned pose_case,source_count[14],native_count[14];
static FA18NativeRecordPoseInput expected_inputs[24];
static FA18NativeRecordPoseChild expected_kinds[24];
static unsigned source_inputs,native_inputs;
static const uint16_t outcomes[]={0,1,16,20,32,40,64,80,128,0xfffe};

static int native_child(void *context,FA18NativeRecordPose *s,FA18NativeRecordPoseChild child,
        const FA18NativeRecordPoseInput *in,FA18NativeRecordPoseResult *out) {
    FA18NativeSceneRecord *r=s->records->records+in->slot; unsigned i;
    const FA18NativeRecordPoseInput *expected;
    (void)context;
    if(native_inputs>=source_inputs || expected_kinds[native_inputs]!=child) return 0;
    expected=expected_inputs+native_inputs++;
    if(in->slot!=expected->slot) return 0;
    if(child==FA18_POSE_CELL_MATRIX && memcmp(in->angles,expected->angles,sizeof in->angles)) return 0;
    if(child==FA18_POSE_MOTION_CANDIDATE && memcmp(in->point,expected->point,sizeof in->point)) return 0;
    if(child==FA18_POSE_GROUND_PROJECTION && memcmp(in->velocity,expected->velocity,sizeof in->velocity)) return 0;
    if((child==FA18_POSE_MESSAGE || child==FA18_POSE_MOTION_SLOT) && in->choice!=expected->choice) return 0;
    if(child==FA18_POSE_SOUND && memcmp(in->sound_arguments,expected->sound_arguments,sizeof in->sound_arguments)) return 0;
    ++native_count[child];
    if(child==FA18_POSE_MOTION_CANDIDATE) {
        out->status=outcomes[(pose_case/4)%10]; out->clear=pose_case%4==0;
        if(pose_case%19==0) r->aircraft->flags|=0x400;
    } else if(child==FA18_POSE_GROUND_PROJECTION) {
        r->geometry->position[0]+=17; r->geometry->position[1]=0; r->geometry->position[2]+=33;
    } else if(child==FA18_POSE_REGION_PROBE) {
        if(pose_case%3==0) r->byte_04|=2; else r->byte_04&=0xfd;
    } else if(child==FA18_POSE_MOTION_SLOT) r->byte_71=(uint8_t)in->choice;
    else if(child!=FA18_POSE_MESSAGE && child!=FA18_POSE_SOUND && child!=FA18_POSE_FAULT) {
        r->word_78=(uint16_t)(r->word_78+1u+child);
        if(child==FA18_POSE_ROOT_FLIGHT && pose_case%7==0) r->long_42=0;
    }
    return 1;
}
static int source_child(uint32_t pc) {
    uint32_t ret=rd_u32(REG_A[7]),r=REG_A[1]; unsigned i;
    FA18NativeRecordPoseChild kind; FA18NativeRecordPoseInput *in;
    switch(ret) {
    case 0xc25b3a: kind=FA18_POSE_CELL_MATRIX; if(pc!=0xc2d970) return 0; break;
    case 0xc25bac: kind=FA18_POSE_SELECTED_RECORD; if(pc!=0xc28e28) return 0; break;
    case 0xc25c70: kind=FA18_POSE_RECORD_ACTION; if(pc!=0xc2c392) return 0; break;
    case 0xc25c76: kind=FA18_POSE_RECORD_CONTROLS; if(pc!=0xc1b27e) return 0; break;
    case 0xc25d20: case 0xc25d54: case 0xc2616a: kind=FA18_POSE_MESSAGE; if(pc!=0xc25704) return 0; break;
    case 0xc25d84: kind=FA18_POSE_RECORD_SELECTOR; if(pc!=0xc13d84) return 0; break;
    case 0xc25da4: kind=FA18_POSE_RECORD_MATRIX; if(pc!=0xc2d408) return 0; break;
    case 0xc25e2c: kind=FA18_POSE_ROOT_FLIGHT; if(pc!=0xc149be) return 0; break;
    case 0xc26014: kind=FA18_POSE_MOTION_CANDIDATE; if(pc!=0xc26ebe) return 0; break;
    case 0xc260fe: kind=FA18_POSE_SOUND; if(pc!=0xc17f8c) return 0; break;
    case 0xc261d6: kind=FA18_POSE_FAULT; if(pc!=0xc06c02 || rd_u16(0xc4599e)!=58) return 0; break;
    case 0xc26246: kind=FA18_POSE_GROUND_PROJECTION; if(pc!=0xc26322) return 0; break;
    case 0xc2624e: kind=FA18_POSE_REGION_PROBE; if(pc!=0xc2b05a) return 0; break;
    case 0xc2625e: kind=FA18_POSE_MOTION_SLOT; if(pc!=0xc26352) return 0; break;
    default: return 0;
    }
    if(r<CONTROL_RECORDS || r>=CONTROL_RECORDS+8192 || (r-CONTROL_RECORDS)%512 || source_inputs>=24) return 0;
    in=expected_inputs+source_inputs; memset(in,0,sizeof *in);
    in->slot=(r-CONTROL_RECORDS)/512; expected_kinds[source_inputs++]=kind;
    ++source_count[kind];
    if(kind==FA18_POSE_CELL_MATRIX) for(i=0;i<3;++i) in->angles[i]=(int16_t)REG_D[5+i];
    if(kind==FA18_POSE_MOTION_CANDIDATE) {
        for(i=0;i<3;++i) in->point[i]=REG_D[2+i];
        REG_D[0]=outcomes[(pose_case/4)%10]; FLAG_Z=pose_case%4==0?0:1;
        if(pose_case%19==0) wr_u16(r,rd_u16(r)|0x400);
    } else if(kind==FA18_POSE_GROUND_PROJECTION) {
        for(i=0;i<3;++i) in->velocity[i]=REG_D[5+i];
        wr_u32(r+20,rd_u32(r+20)+17); wr_u32(r+24,0); wr_u32(r+28,rd_u32(r+28)+33);
    } else if(kind==FA18_POSE_REGION_PROBE) {
        wr_u8(r+4,pose_case%3==0?rd_u8(r+4)|2:rd_u8(r+4)&0xfd);
    } else if(kind==FA18_POSE_MOTION_SLOT) {
        in->choice=(uint16_t)REG_D[1]; wr_u8(r+0x71,(uint8_t)in->choice);
    } else if(kind==FA18_POSE_MESSAGE) {
        in->choice=(uint16_t)REG_D[0]; SET_W(REG_D[0],(uint16_t)REG_D[0]&0xff00);
    } else if(kind==FA18_POSE_SOUND) {
        in->sound_arguments[0]=(uint16_t)rd_u32(REG_A[7]+4); in->sound_arguments[1]=(uint16_t)rd_u32(REG_A[7]+8);
        if(in->sound_arguments[0]!=0x1c || in->sound_arguments[1]!=0x30) return 0;
    } else if(kind!=FA18_POSE_FAULT) {
        wr_u16(r+0x78,(uint16_t)(rd_u16(r+0x78)+1u+kind));
        if(kind==FA18_POSE_ROOT_FLIGHT && pose_case%7==0) wr_u32(r+0x42,0);
    }
    REG_PC=m68ki_pull_32(); return 1;
}
static int original_pose(void) {
    unsigned step,i;
    for(step=0;step<5000;++step) {
        uint32_t pc=REG_PC; uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        if(pc==0xc2d970 || pc==0xc28e28 || pc==0xc2c392 || pc==0xc1b27e || pc==0xc25704 ||
           pc==0xc13d84 || pc==0xc2d408 || pc==0xc149be || pc==0xc26ebe || pc==0xc17f8c ||
           pc==0xc06c02 || pc==0xc26322 || pc==0xc2b05a || pc==0xc26352) {
            if(!source_child(pc)) return 0;
            continue;
        }
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            if(scene_source_bytes[i].pc==pc) break;
        if(i==sizeof scene_source_bytes/sizeof scene_source_bytes[0]) {
            fprintf(stderr,"unexpected pose PC %06X\n",pc); return 0;
        }
        scene_seen[i]=1; opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    return 0;
}
static void pose_fixture(unsigned slot) {
    static const uint16_t flags[]={0,0x1000,0x100,0x1400,0x8000,0x1c00,0x200,0x40};
    static const uint8_t kinds[]={0,0x10,21,0x20,0x30,0x40};
    static const uint8_t decay[]={0,1,5,6,7,8,0x10,0x50,0x80,0x90,0xe0};
    static const uint16_t times[]={0,1,15,16,0xffff};
    unsigned i; uint32_t r=CONTROL_RECORDS+slot*512;
    for(i=0;i<HISTORY_BYTES;++i) wr_u8(HISTORY-HISTORY_ORIGIN+i,(uint8_t)random_value());
    wr_u16(r,flags[(pose_case/16)%8]); wr_u8(r+0x62,kinds[(pose_case/128)%6]);
    wr_u16(r+2,(uint16_t)(((pose_case/5)%8)*0x20u)|((pose_case/7)%2?0x100:0)|
        ((pose_case/17)%2?1:0)|((pose_case/23)%2?2:0));
    wr_u8(r+5,(pose_case/4)%2?0:3); wr_u16(r+0x4c,times[(pose_case/64)%5]);
    wr_u8(r+0x20,(pose_case/9)%2?2:0); wr_u8(r+0x7c,decay[(pose_case/3)%11]);
    wr_u16(r+0x6e,(pose_case/64)%2?500:1000); wr_u8(r+0x3d,(uint8_t)(pose_case%7));
    wr_u8(0xc45788,pose_case%17==0); wr_u8(0xc457ae,pose_case%19==1); wr_u8(0xc45785,(pose_case/3)%2);
    wr_u16(0xc459b4,(uint16_t)slot); wr_u16(0xc459b6,(uint16_t)(slot*512));
    wr_u16(0xc458da,((pose_case/2)%2?slot:slot^1)|((pose_case/32)%2?2:0));
    wr_u16(0xc458dc,pose_case%3==0?slot:slot^1);
    wr_u16(0xc458cc,pose_case%3?0x40:0); wr_u16(0xc45ae0,pose_case%5?0:0x9c06);
    wr_u8(0xc4578c,(pose_case/11)%2); wr_u8(0xc4589a,(pose_case/13)%2);
    wr_u8(0xc4584e,(pose_case/15)%2?0:1); wr_u8(0xc4584f,0);
    wr_u8(0xc4589f,(pose_case/6)%2); wr_u8(0xc45847,decay[(pose_case/7)%11]);
    wr_u16(0xc4594c,(uint16_t)(coarse(rd_u32(r+20))+(pose_case%3==0?3:0)));
    wr_u16(0xc4594e,(uint16_t)(coarse(rd_u32(r+28))+(pose_case%3==1?3:0)));
    wr_u16(0xc4fdd2,pose_case%5==0?0x2000:slot*512);
    wr_u8(0xc4fdd0,(uint8_t)((pose_case/4)%8)); wr_u8(0xc4fdd1,(uint8_t)((pose_case/32)%7));
    if(pose_case%23==0) wr_u8(0xc4fdd1,0xff);
    if(pose_case%29==0) wr_u16(r+2,rd_u16(r+2)|0x1000);
    wr_u32(0xc1ab74,DAMAGE_COUNT-0x46); wr_u16(DAMAGE_COUNT,(uint16_t)random_value());
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0; char error[256];
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE); SceneState *native=malloc(sizeof *native);
    uint8_t history_bytes[HISTORY_BYTES]; PortFieldByte history_fields[HISTORY_BYTES];
    FA18NativeRecordMotionHistory history={history_fields,HISTORY_BYTES,HISTORY_ORIGIN};
    FA18NativeRecordPoseOps ops={native_child,NULL}; FA18NativeRecordPose pose; PoseGlobals g;
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,i,j,e;
    const uint32_t entries[]={0xc25b66,0xc2651e};
    if(!state || !rom || !m || !base || !before || !expected || !native || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(e=0;e<2;++e) for(pose_case=0;pose_case<cases;++pose_case) {
        unsigned slot=(pose_case/7)%16;
        if(pose_case%4==0) slot=0;
        selected_entry=entries[e]; memcpy(m,base,sizeof *m); scene_fixture(pose_case); pose_fixture(slot);
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            for(j=0;j<scene_source_bytes[i].length;++j)
                if(rd_u8(scene_source_bytes[i].pc+j)!=scene_source_bytes[i].bytes[j]) return 1;
        REG_A[1]=CONTROL_RECORDS+slot*512;
        if(!load_scene(native) || !verify_record_owners(native)) return 1;
        memset(&pose,0,sizeof pose); pose.records=&native->bank; pose.ops=&ops; pose.history=&history;
#define W(name,address) g.name=rd_u16(address); pose.name=&g.name;
        POSE_WORDS(W)
#undef W
#define B(name,address) g.name=rd_u8(address); pose.name=&g.name;
        POSE_BYTES(B)
#undef B
        g.events=rd_u32(0xc45b54); pose.events=&g.events;
        for(i=0;i<HISTORY_BYTES;++i) {
            history_bytes[i]=rd_u8(HISTORY-HISTORY_ORIGIN+i);
            history_fields[i]=(PortFieldByte){.byte=history_bytes+i};
        }
        history_fields[HISTORY_ORIGIN-4]=(PortFieldByte){.byte=&g.history_count};
        history_fields[HISTORY_ORIGIN-3]=(PortFieldByte){.byte=&g.history_index};
        history_fields[HISTORY_ORIGIN-2]=(PortFieldByte){.unsigned_word=&g.collision_slot,.shift=8};
        history_fields[HISTORY_ORIGIN-1]=(PortFieldByte){.unsigned_word=&g.collision_slot};
        memcpy(before,m,sizeof *m); source_inputs=native_inputs=0;
        if(!original_pose()) { fprintf(stderr,"source pose stopped %06X case %u\n",selected_entry,pose_case); return 1; }
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        if(!(e?fa18_update_native_record_motion_history(&pose):fa18_update_native_record_pose(&pose,slot))) {
            fprintf(stderr,"native pose failed %06X case %u; child %u/%u\n",selected_entry,pose_case,native_inputs,source_inputs); return 1;
        }
        if(source_inputs!=native_inputs) return 1;
        store_scene(native);
        for(i=0;i<HISTORY_BYTES;++i) { uint8_t b; if(!port_read_field_byte(history_fields+i,&b)) return 1; wr_u8(HISTORY-HISTORY_ORIGIN+i,b); }
#define W(name,address) wr_u16(address,g.name);
        POSE_WORDS(W)
#undef W
#define B(name,address) wr_u8(address,g.name);
        POSE_BYTES(B)
#undef B
        wr_u32(0xc45b54,g.events);
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            uint32_t a=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
            if(a>=0xc7fd00 && a<0xc7ff00) continue;
            if(rd_u8(a)!=expected[i]) {
                fprintf(stderr,"pose %06X case %u byte %06X source %02X native %02X\n",selected_entry,pose_case,a,expected[i],rd_u8(a)); return 1;
            }
        }
        if(!verify_record_owners(native)) return 1;
    }
    for(i=0;i<14;++i) if(source_count[i]!=native_count[i]) return 1;
    printf("native record pose: %u complete calls match all game RAM; child counts",cases*2);
    for(i=0;i<14;++i) printf(" %u",source_count[i]); puts("");
    printf("visited:"); for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
        if(scene_seen[i]) printf(" %06X",scene_source_bytes[i].pc);
    puts(""); free(native); free(expected); free(before); free(base); free(m); return 0;
}
