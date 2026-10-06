/* Compare the runtime C0FBE0 composition, split only at its platform pause. */
#define main records_fixture_main
#include "native_records_oracle.c"
#undef main
#include "../../port/game/native/menu_start.c"

static int source_menu(uint8_t *prefix) {
    memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00;
    wr_u32(REG_A[7],0xc70000);REG_PC=0xc0fbe0;
    m68k_set_reg(M68K_REG_SR,0x2700);fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    unsigned pauses=0;
    for(unsigned step=0;step<2000000;++step) {
        if(REG_PC==0xc70000 && REG_A[7]==0xc7ff04) return pauses==1;
        if(REG_PC==0xc0e78a) {
            if(rd_u32(REG_A[7]+4)!=0xc000 || pauses++) return 0;
            memcpy(prefix,fa18_machine->chip,0x80000);
            memcpy(prefix+0x80000,fa18_machine->slow,0x80000);
            /* Only the busy pause is supplied by the host in this fixture.
             * Source sound selection, buffer reset and script body execute. */
            REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"source menu did not return at %06X\n",REG_PC);return 0;
}
static int matches(const uint8_t *expected,unsigned test,const char *phase) {
    for(unsigned i=0;i<0xff000;++i) {
        uint8_t actual=i<0x80000?fa18_machine->chip[i]:fa18_machine->slow[i-0x80000];
        if(actual!=expected[i]) {
            fprintf(stderr,"menu setup %u %s at %06X: source %02X native %02X\n",
                    test,phase,i<0x80000?i:i-0x80000+0xc00000,expected[i],actual);return 0;
        }
    }
    return 1;
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);
    uint8_t *prefix=malloc(0x100000),*expected=malloc(0x100000);
    if(!state||!rom||!data||nd!=0x100000||!m||!before||!prefix||!expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    for(unsigned test=0;test<18;++test) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        wr_u8(SOUND_FLAGS,(uint8_t[]){0,0x80,0xf7}[test%3]);
        wr_u8(SOUND_FLAGS-1,test&1?4:0);wr_u8(VOLUME_FADING,(test/3)%3);
        wr_u8(CONTEXT_GATE,0xa5);wr_u8(FIFTH_BUFFER_USED,test&1);
        wr_u32(MASTER_VOLUME,0x3f0000);wr_u32(MASTER_VOLUME_TARGET,0);
        for(unsigned bank=0;bank<2;++bank) for(unsigned b=0;b<5;++b) {
            gaddr p=rd_u32((bank?RENDER_BUFFERS_B:RENDER_BUFFERS_A)+4*b);
            for(unsigned i=0;i<RENDER_BUFFER_LONGS;++i) wr_u32(p+4*i,0x74a80000+i);
        }
        memcpy(before,m,sizeof *m);
        if(!source_menu(prefix)) return 1;
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);NativeMenuSetup setup={0};
        native_menu_begin(&setup,7000);
        if(!setup.pending || setup.ready_tick!=7023 || !matches(prefix,test,"before pause")) return 1;
        if(native_menu_resume(&setup,7022) || !matches(prefix,test,"during pause")) return 1;
        if(!native_menu_resume(&setup,7023) || setup.pending || !matches(expected,test,"after pause")) return 1;
        if(!native_menu_resume(&setup,7024) || !matches(expected,test,"completed")) return 1;
    }
    puts("18 connected C0FBE0 menu setup cases match source non-stack RAM before/after the host pause");
    free(expected);free(prefix);free(before);free(m);free(data);free(state);free(rom);return 0;
}
