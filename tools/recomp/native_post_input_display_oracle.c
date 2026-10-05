/* Original CPU instructions are an oracle only; production is ordinary C. */
#define main reference_publication_validation_main
#include "command_publication_oracle.c"
#undef main
#include "../../port/command_queue.c"
#include "../../port/renderer_clear.c"
#include "../../port/post_input_display_stages.c"
#include "../../build/recomp/native_post_input_display_source.h"

enum { REGION_COUNT=11, REGION_BYTES=8048, ABI_FIRST=0xc7fd00, ABI_END=0xc7ff00 };
typedef struct {
    uint8_t bytes[REGION_COUNT][REGION_BYTES],gate,auxiliary;
    FA18NativeGraphicsPlane planes[10];
    FA18NativeGraphicsSetup graphics;
    FA18NativeRendererClear clear;
    FA18CommandInput commands;
    FA18ViewportModeState viewport;
    uint16_t countdown;
    FA18StageCallback callback;
    FA18NativePostInputDisplayStages stage;
} DisplayState;
static unsigned scenario_number,source_children,native_children;
static uint8_t seen[sizeof display_source_bytes/sizeof display_source_bytes[0]];
static uint8_t child_before[FA18_CHIP_SIZE+FA18_SLOW_SIZE];
static uint32_t region_address(unsigned i) { return i==10?0xc457ce:0x10000+0x2200*i; }
static uint32_t callback_address(FA18StageCallback callback) {
    switch(callback) {
    case FA18_STAGE_C0FA04: return 0xc0fa04;
    case FA18_STAGE_C0FA4C: return 0xc0fa4c;
    case FA18_STAGE_C0FA80: return 0xc0fa80;
    case FA18_STAGE_C10C08: return 0xc10c08;
    default: abort();
    }
}
static FA18StageCallback callback_identity(uint32_t address) {
    switch(address) {
    case 0xc0fa04: return FA18_STAGE_C0FA04;
    case 0xc0fa4c: return FA18_STAGE_C0FA4C;
    case 0xc0fa80: return FA18_STAGE_C0FA80;
    case 0xc10c08: return FA18_STAGE_C10C08;
    default: abort();
    }
}
static int alias_fixture(void) { return selected_entry==0xc2fd22 && scenario_number%4!=0; }
static int equal_ram(const uint8_t *expected) {
    unsigned i;
    for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
        uint32_t address=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
        uint8_t actual=i<FA18_CHIP_SIZE?fa18_machine->chip[i]:fa18_machine->slow[i-FA18_CHIP_SIZE];
        if(address>=ABI_FIRST && address<ABI_END) continue;
        if(actual!=expected[i]) {
            fprintf(stderr,"display %06X case %u RAM %06X source %02X native %02X\n",
                    selected_entry,scenario_number,address,expected[i],actual); return 0;
        }
    }
    return 1;
}
static void store(DisplayState *s) {
    unsigned i,j;
    for(i=0;i<(alias_fixture()?REGION_COUNT:REGION_COUNT-1);++i)
        for(j=0;j<REGION_BYTES;++j) wr_u8(region_address(i)+j,s->bytes[i][j]);
    /* An alias fixture's Slow span IS the actual owner of all bytes it covers.
     * Do not overwrite its clear effects with duplicated scalar snapshots. */
    if(alias_fixture()) return;
    wr_u8(FIFTH_BUFFER_USED,*s->clear.fifth_buffer_used);
    wr_u8(RECORDER_MODE,s->commands.indexed.recorder_mode);
    wr_u8(ORIGIN_ENABLE,s->commands.origin_mode);
    wr_u8(POST_INPUT_EVENT,s->commands.indexed.origin_gate_a);
    wr_u8(POST_INPUT_AUX,s->auxiliary); wr_u16(POST_INPUT_COUNTDOWN,s->countdown);
    wr_u8(0xc458a0,s->viewport.current); wr_u8(VIEWPORT_TARGET,s->viewport.target);
    wr_u32(STAGE_CALLBACK,callback_address(s->callback));
}
static void load(DisplayState *s) {
    unsigned i,j;
    memset(s,0,sizeof *s);
    for(i=0;i<REGION_COUNT;++i)
        for(j=0;j<REGION_BYTES;++j) s->bytes[i][j]=rd_u8(region_address(i)+j);
    for(i=0;i<10;++i) {
        uint32_t address=rd_u32(RENDER_BUFFERS_A+4*i);
        if(!address) continue;
        for(j=0;j<REGION_COUNT;++j)
            if(address>=region_address(j) && address+8000<=region_address(j)+REGION_BYTES) break;
        if(j==REGION_COUNT) abort();
        s->planes[i]=(FA18NativeGraphicsPlane){s->bytes[j]+address-region_address(j),
            REGION_BYTES-address+region_address(j),0};
    }
    for(i=0;i<9;++i) s->graphics.source[i]=s->planes[i].bytes?&s->planes[i]:NULL;
    s->gate=rd_u8(FIFTH_BUFFER_USED);
    s->clear=(FA18NativeRendererClear){&s->graphics,&s->planes[9],
        alias_fixture()?s->bytes[10]+FIFTH_BUFFER_USED-region_address(10):&s->gate};
    s->commands.indexed.recorder_mode=rd_u8(RECORDER_MODE);
    s->commands.origin_mode=rd_u8(ORIGIN_ENABLE);
    s->commands.indexed.origin_gate_a=rd_u8(POST_INPUT_EVENT);
    s->auxiliary=rd_u8(POST_INPUT_AUX); s->countdown=rd_u16(POST_INPUT_COUNTDOWN);
    s->viewport.current=rd_u8(0xc458a0); s->viewport.target=rd_u8(VIEWPORT_TARGET);
    s->callback=callback_identity(rd_u32(STAGE_CALLBACK));
    s->stage=(FA18NativePostInputDisplayStages){&s->commands,&s->viewport,&s->countdown,
        &s->callback,&s->auxiliary,&s->clear};
}
/* Only the actual C0FAA4 scene child is contracted; the real original clear
 * body executes to return. Changes are computed independently in both runs. */
static void child_effect(DisplayState *s) {
    uint8_t event=(uint8_t)(scenario_number^0xa5),aux=(uint8_t)(scenario_number^0x5a);
    if(s) {
        s->commands.indexed.recorder_mode=0x81; s->commands.origin_mode=0x42;
        s->countdown=0x7777; s->viewport.current=7; s->viewport.target=8;
        s->commands.indexed.origin_gate_a=event; s->auxiliary=aux;
        s->callback=FA18_STAGE_C10C08;
    } else {
        wr_u8(RECORDER_MODE,0x81); wr_u8(ORIGIN_ENABLE,0x42);
        wr_u16(POST_INPUT_COUNTDOWN,0x7777); wr_u8(0xc458a0,7); wr_u8(VIEWPORT_TARGET,8);
        wr_u8(POST_INPUT_EVENT,event); wr_u8(POST_INPUT_AUX,aux); wr_u32(STAGE_CALLBACK,0xc10c08);
    }
}
static int initialize_scene(void *context) {
    DisplayState *s=context;
    store(s);
    if(native_children++ || !source_children || !equal_ram(child_before)) {
        fprintf(stderr,"native scene entry differs\n"); exit(1);
    }
    child_effect(s); return 1;
}
static void setup_fixture(unsigned scenario) {
    static const uint16_t counts[]={0,1,0x7fff,0x8000,0xffff};
    static const uint8_t gates[]={0,1,0x80,0xff};
    unsigned i,j;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    SET_CYCLES(100000000); REG_A[7]=0xc7ff00; wr_u32(REG_A[7],0xc70000);
    m68k_set_reg(M68K_REG_SR,0x2700|(scenario&31)); REG_PC=selected_entry;
    fa18_recomp_abort=0; fa18_next_event=INT64_MAX;
    for(i=0;i<(alias_fixture()?REGION_COUNT:REGION_COUNT-1);++i)
        for(j=0;j<REGION_BYTES;++j) wr_u8(region_address(i)+j,(uint8_t)(random_value()|1));
    for(i=0;i<10;++i) {
        unsigned region=i;
        uint32_t offset=(scenario&8)?4*(i%3):0;
        if(i>=5 && i<9 && !alias_fixture()) region=(scenario&16)?(i+scenario)%5:i-5;
        if(alias_fixture() && ((scenario%4==1 && i==0) || (scenario%4>=2 && i==4))) {
            region=10; offset=scenario%4==3?6:0;
        }
        wr_u32(RENDER_BUFFERS_A+4*i,region_address(region)+offset);
    }
    wr_u8(FIFTH_BUFFER_USED,gates[(scenario/4)%4]);
    if(!(scenario&32) && !alias_fixture() && !rd_u8(FIFTH_BUFFER_USED)) wr_u32(RENDER_BUFFERS_A+16,0);
    wr_u8(RECORDER_MODE,(uint8_t)random_value()); wr_u8(ORIGIN_ENABLE,(uint8_t)random_value());
    wr_u8(POST_INPUT_EVENT,(uint8_t)random_value()); wr_u8(POST_INPUT_AUX,(uint8_t)random_value());
    wr_u16(POST_INPUT_COUNTDOWN,counts[scenario%5]);
    wr_u8(0xc458a0,(uint8_t)(scenario/5));
    wr_u8(VIEWPORT_TARGET,(uint8_t)((scenario/5)+((scenario&1)?1:0)));
    wr_u32(STAGE_CALLBACK,selected_entry==0xc2fd22?0xc0fa04:selected_entry);
}
static int original(void) {
    unsigned step,i;
    for(step=0;step<40000;++step) {
        uint32_t pc=REG_PC;
        uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        if(pc==0xc0faa4) {
            if(source_children++) return 0;
            memcpy(child_before,fa18_machine->chip,FA18_CHIP_SIZE);
            memcpy(child_before+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
            child_effect(NULL);
            REG_D[0]=scenario_number; REG_D[1]=0xdeadbeef;
            REG_A[0]=0xc68000; REG_A[1]=0xc68100;
            REG_PC=rd_u32(REG_A[7]); REG_A[7]+=4; continue;
        }
        for(i=0;i<sizeof display_source_bytes/sizeof display_source_bytes[0];++i)
            if(display_source_bytes[i].pc==pc) break;
        if(i==sizeof display_source_bytes/sizeof display_source_bytes[0]) {
            fprintf(stderr,"unexpected display PC %06X\n",pc); return 0;
        }
        seen[i]=1; opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    return 0;
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    DisplayState *native=malloc(sizeof *native);
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):4096,i,j,total_children=0;
    char error[256];
    selected_entry=argc>2?(uint32_t)strtoul(argv[2],NULL,16):0xc2fd22;
    if(!state || !rom || !m || !base || !before || !expected || !native || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) {
        fprintf(stderr,"%s\n",error); return 1;
    }
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(i=0;i<sizeof display_source_bytes/sizeof display_source_bytes[0];++i)
        for(j=0;j<display_source_bytes[i].length;++j)
            if(rd_u8(display_source_bytes[i].pc+j)!=display_source_bytes[i].bytes[j]) return 1;
    for(scenario_number=0;scenario_number<cases;++scenario_number) {
        FA18NativePostInputDisplayOps ops={initialize_scene,native}; int ok;
        source_children=native_children=0;
        memcpy(m,base,sizeof *m); setup_fixture(scenario_number); load(native); memcpy(before,m,sizeof *m);
        if(!original()) { fprintf(stderr,"original display failed case %u at %06X\n",scenario_number,REG_PC); return 1; }
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        switch(selected_entry) {
        case 0xc2fd22: ok=fa18_clear_native_renderer(&native->clear); break;
        case 0xc0fa04: ok=fa18_finish_native_post_input_display(&native->stage,&ops); break;
        case 0xc0fa4c: ok=fa18_match_native_post_input_display(&native->stage); break;
        case 0xc0fa80: ok=fa18_complete_native_post_input_display(&native->stage); break;
        default: return 1;
        }
        store(native);
        if(!ok || source_children!=native_children || !equal_ram(expected)) {
            fprintf(stderr,"native display failed %06X case %u\n",selected_entry,scenario_number); return 1;
        }
        total_children+=native_children;
    }
    printf("native display %06X: %u complete calls and %u contracted scene entries match full RAM outside the CPU ABI stack\n",selected_entry,cases,total_children);
    printf("visited:");
    for(i=0;i<sizeof display_source_bytes/sizeof display_source_bytes[0];++i)
        if(seen[i]) printf(" %06X",display_source_bytes[i].pc);
    puts(""); free(native); free(expected); free(before); free(base); free(m); return 0;
}
