/* Validate actual native setup children and existing reset owner. */
#define MH_DIVIDE HEADING_DIVIDE
#include "target_heading.h"
#undef MH_DIVIDE
#define main records_fixture_main
#include "native_records_oracle.c"
#undef main
#include "../../port/game/native/setup.c"
#include "postflight_completion.h"

/* Aircraft selection is outside these three bounded entry/reset cases.
 * Treat an unexpected jump into it as a failed test, never a fake consumer. */
void native_flight_reset_aircraft(NativeFrontend *game) {
    (void)game;fputs("unexpected aircraft-refresh boundary in postflight test\n",stderr);abort();
}
void native_flight_cancel_context(NativeFrontend *game) {
    (void)game;fputs("unexpected context-cancel boundary in postflight test\n",stderr);abort();
}

static int original_entry(gaddr entry) {
    memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00u;wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700);REG_PC=entry;
    fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned steps=0;steps<200000;++steps) {
        if(REG_PC==0xc70000u && REG_A[7]==0xc7ff04u) return 1;
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"postflight source did not return at %06X\n",REG_PC);return 0;
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state || !rom || !data || nd!=0x100000 || !m || !before || !expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    for(unsigned variant=0;variant<3;++variant) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        gaddr entry=variant?0xc11788:0xc10dae;
        if(!variant) {
            wr_u16(CONTROL_RECORDS,(uint16_t)(rd_u16(CONTROL_RECORDS)|0x200));
            wr_u8(COMMAND_BLOCK_FLAGS,0);wr_u8(MENU_CONTEXT_FLAG,1);wr_u8(CONTEXT_SELECT,0);
        } else {
            wr_u8(CONTEXT_SELECT,0);wr_u8(PLAYER_FLAGS_A,variant==1?0x28:0);
        }
        memcpy(before,m,sizeof *m);
        if(!original_entry(entry)) return 1;
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);
        NativeFrontend game={0};
        if(!variant) native_setup_stage(&game,entry);
        else advance_postflight_completion(NULL);
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"postflight case %u %06X: source %02X native %02X\n",variant,
                    i<0x80000?i:i-0x80000+0xc00000,expected[i],actual);return 1;
            }
        }
    }
    puts("Native postflight entry, reset wait and reset release match original non-stack RAM");
    free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
