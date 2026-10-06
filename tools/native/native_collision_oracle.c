/* Original collision children in the connected native voice-gate contract.
 * Sample voices are not loaded by this runtime; no fixture feeds game state. */
#define main records_fixture_main
#include "native_records_oracle.c"
#undef main
static int source_child(gaddr entry) {
    memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00;
    for(unsigned i=0;i<8;++i) REG_D[i]=0x51ab0020+i;
    wr_u32(REG_A[7],0xc70000);wr_u32(REG_A[7]+4,0x1c);wr_u32(REG_A[7]+8,0x30);
    REG_PC=entry;m68k_set_reg(M68K_REG_SR,0x2700);
    fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned step=0;step<10000;++step) {
        if(REG_PC==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"collision source did not return at %06X\n",REG_PC);return 0;
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state||!rom||!data||nd!=0x100000||!m||!before||!expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    for(unsigned variant=0;variant<6;++variant) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        if(rd_u32(0xc0a450)) {fputs("expected native unloaded sample voice\n",stderr);return 1;}
        wr_u8(SOUND_FLAGS-1,variant&3);
        memcpy(before,m,sizeof *m);
        if(!source_child(variant<4?0xc17f8c:0xc06c02)) return 1;
        for(unsigned i=0;i<8;++i) if(REG_D[i]!=0x51ab0020+i) {
            fputs("collision child changed a carried gameplay value\n",stderr);return 1;
        }
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);
        if(variant<4) start_sound_6(0x1c,0x30);else fault_hook();
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"collision child %u %06X: source %02X native %02X\n",
                        variant,i<0x80000?i:0xc00000+i-0x80000,expected[i],actual);return 1;
            }
        }
    }
    puts("Six original collision sound/fault cases match non-stack RAM and preserve carried gameplay values");
    free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
