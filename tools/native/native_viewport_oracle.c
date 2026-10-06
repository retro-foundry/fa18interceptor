/* C1718E's viewport/fade tail, with RGB4 publication as the host boundary. */
#define main records_fixture_main
#include "native_records_oracle.c"
#undef main
#include "../../port/game/native/viewport.c"

static uint16_t source_palette[32];
static int source_tick(void) {
    memset(REG_DA,0,sizeof REG_DA);REG_A[6]=0xc7ff00;REG_A[7]=REG_A[6]-26;
    REG_PC=0xc172de;m68k_set_reg(M68K_REG_SR,0x2700);
    fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned step=0;step<10000;++step) {
        if(REG_PC==0xc1744c) return 1;
        if(REG_PC==0xc53ec0) {
            gaddr colors=rd_u32(REG_A[7]+8);
            if(rd_u32(REG_A[7]+4)!=0xc1822a || rd_u32(REG_A[7]+12)!=16) return 0;
            for(unsigned i=0;i<16;++i) source_palette[i]=rd_u16(colors+2*i);
            REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"source viewport did not complete at %06X\n",REG_PC);return 0;
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);
    NativeFrontend *game=calloc(1,sizeof *game);uint8_t *expected=malloc(0x100000);
    if(!state||!rom||!data||nd!=0x100000||!m||!before||!game||!expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    for(unsigned test=0;test<128;++test) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        wr_u8(VIEWPORT_MODE,test&1?15:0);wr_u8(VIEWPORT_TARGET,test&2?15:0);
        wr_u8(0xc458a3,(uint8_t[]){0,1,2,127,128,255,254,3}[(test>>2)&7]);
        wr_u8(TABLE_CLEAR_MODE,(test>>5)&3);wr_u16(DRAW_PAGE,test&1);
        wr_u8(VOLUME_FADING,test&32?1:0);wr_u32(MASTER_VOLUME,test&64?0x3f0000:0);
        wr_u32(MASTER_VOLUME_TARGET,test&16?0x1f0000:0x3f0000);
        for(unsigned i=0;i<32;++i) game->palette[i]=source_palette[i]=(uint16_t)(0x740+i);
        memcpy(before,m,sizeof *m);
        /* A whole 50-tick interval covers multi-step fades, both directions,
         * terminal palette copies and stable publication on either page. */
        for(unsigned tick=0;tick<50;++tick) if(!source_tick()) return 1;
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);
        for(unsigned tick=0;tick<50;++tick) native_viewport_tick(game);
        for(unsigned i=0;i<0xff000;++i) {
            if(i>=PALETTE_FRAME-22 && i<PALETTE_FRAME) continue;
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"viewport case %u at %06X: source %02X native %02X\n",test,
                        i<0x80000?i:i-0x80000+0xc00000,expected[i],actual);return 1;
            }
        }
        if(memcmp(game->palette,source_palette,sizeof source_palette)) {
            fprintf(stderr,"viewport case %u host RGB4 publication differs\n",test);return 1;
        }
    }
    puts("128 viewport/fade sequences (6400 ticks) match original non-stack RAM and RGB4 publication");
    free(expected);free(game);free(before);free(m);free(data);free(state);free(rom);return 0;
}
