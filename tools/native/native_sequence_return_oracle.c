/* Compare the connected C0F920/C08F26 reset, including dirty work banks. */
#define FA18_QUALIFICATION_ORACLE_LIBRARY
#include "native_qualification_oracle.c"
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
    native_clock_set(7000);
    for(unsigned variant=0;variant<4;++variant) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        wr_u8(MODE_SELECT,9);wr_u8(RECORDER_MODE,variant&1);wr_u8(CONTEXT_REQUEST,1);
        wr_u8(FIFTH_BUFFER_USED,variant&2);
        for(unsigned bank=0;bank<2;++bank) for(unsigned buffer=0;buffer<5;++buffer) {
            gaddr pointer=rd_u32((bank?RENDER_BUFFERS_B:RENDER_BUFFERS_A)+buffer*4);
            for(unsigned word=0;word<RENDER_BUFFER_LONGS;++word) wr_u32(pointer+word*4,0x71a40000+word);
        }
        memcpy(before,m,sizeof *m);
        if(!original_parent(0xc0f920)) return 1;
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);NativeFrontend game={0};
        stage(&game,0xc0f920);
        for(unsigned i=0;i<0xff000;++i) {
            if((i>=0x46fc && i<0x4718) || (i>=0x47ca && i<0x4800)) continue;
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"sequence reset %u %06X: source %02X native %02X\n",
                        variant,i<0x80000?i:0xc00000+i-0x80000,expected[i],actual);return 1;
            }
        }
        if(rd_u32(STAGE_CALLBACK)!=0xc0fbe0 || rd_u8(MODE_SELECT) || rd_u8(RECORDER_MODE) ||
           rd_u8(CONTEXT_REQUEST) || game.record_updates!=1) return 1;
    }
    puts("Four native C0F920/C08F26 sequence resets match original non-stack RAM, including dirty work banks");
    free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
