/* Validation only: compare ordinary C state to original CPU-executed bytes.
 * $C3318E is a controlled child boundary; this does not prove that child.
 * The native implementation is also built separately with no machine sources. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "m68kcpu.h"
#include "m68kops.h"
#include "machine.h"
#include "bus.h"
#include "recomp_runtime.h"
#include "globals.h"
#include "memory.h"
#include "../../port/indexed_controls.c"
#include "../../build/recomp/native_indexed_controls_source.h"

#define BYTE_FIELDS(X) \
 X(mode,MODE_SELECT) X(mode_request,0xc45792u) X(mode_gate,COMMAND_MODE_GATE) \
 X(enable_gate,COMMAND_ENABLE_GATE) X(enable_selection,0xc45849u) \
 X(function_modifier,KEY_STATE+1) X(recorder_mode,RECORDER_MODE) \
 X(cockpit_high_byte,COCKPIT_FLAGS) X(pose_entry,SCENE_POSE_ENTRY) \
 X(pose_inhibit,0xc458adu) X(origin_detail,ORIGIN_DETAIL_MODE) \
 X(origin_gate_a,ORIGIN_GATE_A) X(origin_gate_b,ORIGIN_GATE_B) \
 X(function_level,FUNCTION_KEY_LEVEL) X(control_record_level,CONTROL_RECORDS+0x2bu) \
 X(player_ready,PLAYER_READY)

static uint32_t seed=0x19688000u;
static uint32_t random_value(void) {
    seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed;
}
static uint8_t *read_file(const char *path,size_t *size) {
    FILE *f=fopen(path,"rb"); long n; uint8_t *data;
    if(!f) return NULL;
    if(fseek(f,0,SEEK_END) || (n=ftell(f))<0 || fseek(f,0,SEEK_SET)) { fclose(f); return NULL; }
    data=malloc((size_t)n);
    if(!data) { fclose(f); return NULL; }
    if(fread(data,1,(size_t)n,f)!=(size_t)n || fclose(f)) { free(data); return NULL; }
    *size=(size_t)n; return data;
}
static void store(const FA18IndexedControls *s) {
    unsigned i;
#define FIELD(name,address) wr_u8(address,s->name);
    BYTE_FIELDS(FIELD)
#undef FIELD
    wr_u32(PLAYBACK_BYTES,s->playback_bytes);
    wr_u16(CONTROL_ACCUMULATOR_Y,(uint16_t)s->throttle);
    wr_u16(CONTROL_ACCUMULATOR_COMPANION,(uint16_t)s->throttle_companion);
    wr_u32(MODE_TABLE,0xc61000u);
    wr_u16(0xc61000u,s->modes.status); wr_u8(0xc61006u,s->modes.level);
    for(i=0;i<8;++i) wr_u8(0xc61012u+i,s->modes.available[i]);
}
static FA18IndexedControls load(void) {
    FA18IndexedControls s; unsigned i;
    memset(&s,0,sizeof s);
#define FIELD(name,address) s.name=rd_u8(address);
    BYTE_FIELDS(FIELD)
#undef FIELD
    s.playback_bytes=rd_u32(PLAYBACK_BYTES);
    s.throttle=rd_s16(CONTROL_ACCUMULATOR_Y);
    s.throttle_companion=rd_s16(CONTROL_ACCUMULATOR_COMPANION);
    s.modes.status=rd_u16(0xc61000u); s.modes.level=rd_u8(0xc61006u);
    for(i=0;i<8;++i) s.modes.available[i]=rd_u8(0xc61012u+i);
    return s;
}
typedef struct {
    unsigned calls;
    uint32_t event;
    FA18IndexedControls input;
} Child;
static void child_effects(FA18IndexedControls *s) {
    s->mode^=0x80; s->mode_request=(uint8_t)(s->mode_request+3);
    s->function_level^=0x5a;
    s->throttle=(int16_t)((uint16_t)s->throttle+0x1234u);
}
static uint32_t changed(void *context,FA18IndexedControls *s) {
    Child *c=context;
    c->input=*s; ++c->calls; child_effects(s); return c->event;
}

static unsigned char visited[0x1c23c-0x1bc50];
static int original(Child *expected,unsigned *calls) {
    unsigned steps;
    for(steps=0;steps<10000;++steps) {
        uint32_t pc=REG_PC;
        uint16_t op;
        if(pc==0xc1c23cu) return 1;
        if(pc==0xc3318eu) {
            FA18IndexedControls input=load();
            ++*calls;
            if(*calls!=expected->calls || memcmp(&input,&expected->input,sizeof input)) return 0;
            child_effects(&input); store(&input);
            REG_D[0]=expected->event;
            REG_PC=rd_u32(REG_A[7]); REG_A[7]+=4;
            continue;
        }
        if(!((pc>=0xc1bc50u && pc<=0xc1bee4u) || (pc>=0xc1c214u && pc<=0xc1c222u))) {
            fprintf(stderr,"unexpected source PC %06X\n",pc); return 0;
        }
        visited[pc-0xc1bc50u]=1;
        op=rd_u16(pc); REG_PPC=pc; REG_IR=op; REG_PC=pc+2;
        m68ki_instruction_jump_table[op](); USE_CYCLES(CYC_INSTRUCTION[op]);
    }
    return 0;
}
static uint8_t boundary_byte(unsigned i) {
    static const uint8_t values[]={0,1,2,3,7,8,12,0x7f,0x80,0xfd,0xfe,0xff};
    return values[i%(sizeof values/sizeof values[0])];
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m);
    uint8_t *before=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    uint8_t *expected_ram=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):65536,scenario;
    unsigned transitions=0,poses_changed=0,throttles_changed=0;
    char error[256];
    if(!state || !rom || !m || !before || !expected_ram || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) {
        fprintf(stderr,"native indexed controls: %s\n",error); return 1;
    }
    free(state); free(rom); fa18_bus_timing=0;
    {
        unsigned row,byte;
        for(row=0;row<sizeof source_bytes/sizeof source_bytes[0];++row)
            for(byte=0;byte<source_bytes[row].length;++byte)
                if(rd_u8(source_bytes[row].pc+byte)!=source_bytes[row].bytes[byte]) {
                    fprintf(stderr,"original bytes differ at %06X\n",source_bytes[row].pc+byte); return 1;
                }
    }
    for(scenario=0;scenario<cases;++scenario) {
        FA18IndexedControls s; FA18IndexedControlRequest r;
        int16_t pose_values[17],carry=(int16_t)random_value();
        FA18IndexedControlPoses poses={pose_values,17};
        Child child; uint32_t event; unsigned i,source_calls=0,profile=(scenario>>8)%32;
        memset(&s,0,sizeof s); memset(&child,0,sizeof child);
#define FIELD(name,address) s.name=(uint8_t)random_value();
        BYTE_FIELDS(FIELD)
#undef FIELD
        s.mode=0; s.mode_gate=1; s.enable_gate=0; s.origin_gate_a=0;
        s.recorder_mode=0; s.cockpit_high_byte=0; s.origin_detail=0; s.pose_inhibit=0;
        s.player_ready=1; s.function_modifier=0;
        s.playback_bytes=(scenario&1)?random_value():0;
        s.throttle=(int16_t)random_value(); s.throttle_companion=(int16_t)random_value();
        s.modes.status=(scenario&8)?0:1; s.modes.level=boundary_byte(scenario/16);
        for(i=0;i<8;++i) s.modes.available[i]=(scenario&(1u<<i))?1:0;
        switch(profile) {
        case 0: s.enable_gate=1; break;
        case 1: s.mode_gate=0; break;
        case 2: s.mode=1; break;
        case 3: s.mode=1; s.function_modifier=1; break;
        case 4: s.mode=1; s.cockpit_high_byte=8; s.recorder_mode=boundary_byte(scenario); break;
        case 5: s.mode=1; s.origin_detail=0x80; break;
        case 6: s.mode=1; s.pose_inhibit=1; break;
        case 7: s.mode=1; s.origin_gate_a=1; break;
        case 8: s.mode=1; s.recorder_mode=boundary_byte(scenario/3); break;
        case 9: s.mode=1; s.player_ready=0; break;
        case 10: s.mode=1; s.function_level=0; s.control_record_level=12; break;
        case 11: s.mode=1; s.function_level=12; break;
        default: break;
        }
        r.kind=(FA18IndexedControlKind)(scenario%3);
        r.event=(random_value()&0xffff0000u)|(scenario&0xffu);
        if(profile>=16) r.event=(random_value()&0xffff0000u)|(uint16_t)random_value();
        r.modifier=boundary_byte(scenario/4);
        r.selection=(int16_t)((random_value()&0xff00u)|(scenario&0xffu));
        if(profile<2) r.selection=(int16_t)((scenario%13)-2);
        for(i=0;i<17;++i) pose_values[i]=i==16?((scenario&1)?-1:-2):0;
        store(&s);
        for(i=0;i<17;++i) wr_u16(SCENE_POSE_TABLE+16*i,(uint16_t)pose_values[i]);
        REG_A[7]=0xc7ff00u;
        memcpy(before,m->chip,FA18_CHIP_SIZE); memcpy(before+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        child.event=random_value();
        if(!fa18_apply_indexed_control(&s,&r,carry,&poses,changed,&child,&event)) {
            fprintf(stderr,"native indexed controls: case %u refused valid fixture\n",scenario); return 1;
        }
        transitions+=child.calls;
        store(&s);
        if(child.calls) wr_u32(REG_A[7]-4,0xc1bdf8u); /* source JSR stack write */
        memcpy(expected_ram,m->chip,FA18_CHIP_SIZE); memcpy(expected_ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m->chip,before,FA18_CHIP_SIZE); memcpy(m->slow,before+FA18_CHIP_SIZE,FA18_SLOW_SIZE);
        for(i=0;i<16;++i) REG_DA[i]=random_value();
        REG_D[0]=r.event; REG_D[4]=(random_value()&0xffff0000u)|(uint16_t)carry; REG_D[6]=r.modifier;
        REG_A[7]=0xc7ff00u;
        REG_PC=r.kind==FA18_INDEXED_FUNCTION_KEY?0xc1bc50u:
               r.kind==FA18_INDEXED_LOW_KEY?0xc1bc72u:0xc1bc78u;
        if(r.kind==FA18_INDEXED_SELECTION) REG_D[4]=(random_value()&0xffff0000u)|(uint16_t)r.selection;
        m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31)); SET_CYCLES(100000000);
        if(!original(&child,&source_calls) || source_calls!=child.calls || REG_D[0]!=event ||
           memcmp(m->chip,expected_ram,FA18_CHIP_SIZE) ||
           memcmp(m->slow,expected_ram+FA18_CHIP_SIZE,FA18_SLOW_SIZE)) {
            unsigned offset;
            fprintf(stderr,"native indexed controls: case %u kind %u profile %u event %08X selection %04X PC %06X\n",
                    scenario,r.kind,profile,r.event,(uint16_t)r.selection,REG_PC);
            for(offset=0;offset<FA18_SLOW_SIZE;++offset) if(m->slow[offset]!=expected_ram[FA18_CHIP_SIZE+offset]) {
                fprintf(stderr,"RAM %06X original %02X native %02X\n",FA18_SLOW_BASE+offset,m->slow[offset],expected_ram[FA18_CHIP_SIZE+offset]); break;
            }
            return 1;
        }
        poses_changed+=m->slow[SCENE_POSE_ENTRY-FA18_SLOW_BASE]!=before[FA18_CHIP_SIZE+SCENE_POSE_ENTRY-FA18_SLOW_BASE];
        throttles_changed+=m->slow[CONTROL_ACCUMULATOR_Y-FA18_SLOW_BASE]!=before[FA18_CHIP_SIZE+CONTROL_ACCUMULATOR_Y-FA18_SLOW_BASE];
    }
    printf("native indexed controls: %u cases matched original instructions, full Chip/Slow RAM, published events and child inputs; %u transitions, %u pose writes, %u throttle changes\n",
           cases,transitions,poses_changed,throttles_changed);
    printf("visited:"); for(scenario=0;scenario<sizeof visited;++scenario) if(visited[scenario]) printf(" %06X",0xc1bc50u+scenario); putchar('\n');
    free(expected_ram); free(before); free(m); return 0;
}
