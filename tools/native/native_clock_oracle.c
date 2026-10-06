/* C25312/C2548A: compare resumable host polling with original instructions.
 * Reuse the record fixture loader; CPU, ROM and opcodes exist only here. */
#define main records_fixture_main
#include "native_records_oracle.c"
#undef main
#include "main_loop_timers.h"

enum { SAMPLE_SECONDS=0xc45af2u,POLL_SECONDS=0xc45afau,PREVIOUS_SECONDS=0xc45b02u,
       ELAPSED_SAMPLE=0xc45b0au,TIMER_FLAGS=0xc458ceu };
static unsigned clock_start,requests;
static void request_sample(void) {
    /* First acquisition is the prelude; subsequent acquisitions poll at
     * successive real PAL clock samples, including the first unchanged one. */
    native_clock_set(clock_start+(requests?requests-1:0));
    ++requests;
    native_clock_request();
}
static MainTimerBounds timer_child(void *context,enum MainTimerChild child) {
    (void)context;
    switch(child) {
    case MT_SAMPLE_BEGIN: case MT_SAMPLE_POLL: {
        const MenuContextHooks hooks={.consume=request};
        native_clock_set(clock_start+(requests?requests-1:0)); ++requests;
        read_menu_time_sample(&hooks); break;
    }
    case MT_COUNT_FIRST: tick_timer(0xc45886u); break;
    case MT_COUNT_SECOND: tick_timer(0xc45891u); break;
    case MT_COUNT_THIRD: tick_timer(TONE_MUTE); break;
    default: abort();
    }
    return (MainTimerBounds){0};
}
static int original_timer(gaddr entry) {
    memset(REG_DA,0,sizeof REG_DA); REG_A[7]=0xc7ff00u; wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700); REG_PC=entry;
    fa18_next_event=INT64_MAX; SET_CYCLES(100000000);
    for(unsigned steps=0;steps<1000000;++steps) {
        if(REG_PC==0xc70000u && REG_A[7]==0xc7ff04u) return 1;
        if(REG_PC==0xc53c78u) {
            request_sample(); REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        uint16_t opcode=rd_u16(REG_PC); REG_PPC=REG_PC; REG_IR=opcode; REG_PC+=2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"original timer did not return at %06X\n",REG_PC); return 0;
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0; char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state || !rom || !data || nd!=0x100000 || !m || !before || !expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) { fputs(error,stderr);return 1; }
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    unsigned waiting_cases=0;
    for(unsigned variant=0;variant<36;++variant) {
        memcpy(m->chip,data,0x80000); memcpy(m->slow,data+0x80000,0x80000);
        clock_start=7000; requests=0;
        wr_u16(UPDATE_TICK,(uint16_t)variant);
        wr_u32(PREVIOUS_SECONDS,139); wr_u32(PREVIOUS_SECONDS+4,920000);
        wr_u32(POLL_SECONDS,140); wr_u32(POLL_SECONDS+4,0);
        wr_u16(TIMER_FLAGS,variant&16?0x100:0);
        wr_u16(0xc45b0eu,100); wr_u32(0xc45b10u,9980); wr_u32(0xc45b14u,0);
        wr_u8(0xc45884u,0); wr_u8(0xc45885u,0x80);
        wr_u8(0xc45886u,0); wr_u8(0xc45891u,0xff); wr_u8(TONE_MUTE,1);
        wr_u8(0xc45888u,(uint8_t)(variant&1)); wr_u8(0xc45889u,15);
        if(variant==32) wr_u32(POLL_SECONDS,0xffffffffu);
        if(variant==33) wr_u32(POLL_SECONDS,80); /* long elapsed readout sentinel */
        if(variant==34) wr_u32(PREVIOUS_SECONDS,0xffffffffu);
        if(variant==35) {
            wr_u8(0xc458beu,0); /* disk table's uncapped entry */
            wr_u32(POLL_SECONDS+4,980000); /* nonzero readout divisor */
            wr_u32(POLL_SECONDS,139);
        }
        memcpy(before,m,sizeof *m);
        if(!original_timer(0xc25312u)) { fprintf(stderr,"C25312 variant %u\n",variant);return 1; }
        const unsigned original_requests=requests;
        if(!original_timer(0xc2548au)) { fprintf(stderr,"C2548A variant %u\n",variant);return 1; }
        memcpy(expected,m->chip,0x80000); memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m); requests=0;
        const MainTimerHooks hooks={.consume=timer_child};
        begin_main_loop_timers(&hooks);
        unsigned yields=0;
        while(!poll_main_loop_timers(&hooks)) if(++yields>100) return 1;
        if(yields) ++waiting_cases;
        sample_main_loop_readout(&hooks);
        if(requests!=original_requests) {
            fprintf(stderr,"timer variant %u: source/native requests %u/%u\n",variant,original_requests,requests); return 1;
        }
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"timer variant %u %06X: source %02X native %02X\n",variant,
                    i<0x80000?i:i-0x80000+0xc00000,expected[i],actual);return 1;
            }
        }
    }
    printf("36 timer/readout cases match original non-stack RAM and poll counts (%u yielded)\n",waiting_cases);
    for(unsigned variant=0;variant<2;++variant) {
        memcpy(m->chip,data,0x80000); memcpy(m->slow,data+0x80000,0x80000);
        wr_u16(UPDATE_TICK,(uint16_t)(3+16*variant));
        memcpy(before,m,sizeof *m); requests=0;
        if(!original_timer(0xc1c63eu)) return 1;
        memcpy(expected,m->chip,0x80000); memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m); native_clock_set(clock_start);
        native_records_update();
        for(unsigned i=0;i<0xff000;++i) {
            if((i>=0x46fc && i<0x4718) || (i>=0x47ca && i<0x4800)) continue;
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"periodic record variant %u %06X: source %02X native %02X\n",variant,
                    i<0x80000?i:i-0x80000+0xc00000,expected[i],actual);return 1;
            }
        }
    }
    puts("Two complete record passes with C28996 periodic region work match original non-stack RAM");
    free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
