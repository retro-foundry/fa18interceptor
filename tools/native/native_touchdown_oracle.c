/* Compare the native C149BE touchdown child with original C083E2. */
#define main records_fixture_main
#include "native_records_oracle.c"
#undef main
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
    const uint8_t phases[]={0,1,2,0xff};
    for(unsigned variant=0;variant<8;++variant) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        wr_u8(MODE_SELECT,variant<4?6:9);wr_u8(PLAYER_PHASE,phases[variant&3]);
        wr_u8(ATTEMPTS_LEFT,(uint8_t)(0x12+variant));
        /* Make every reset field and the three complete records observable. */
        for(unsigned slot=1;slot<=3;++slot) for(unsigned i=0;i<164;++i)
            wr_u8(CONTROL_RECORDS+512*slot+i,(uint8_t)(i+slot));
        memcpy(before,m,sizeof *m);
        memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00;
        wr_u32(REG_A[7],0xc70000);REG_PC=0xc083e2;
        m68k_set_reg(M68K_REG_SR,0x2700);fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
        if(!original()) return 1;
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);
        root_control_child(NULL,FC_RESET_CONTROL,(FlightWorking){0});
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"touchdown case %u %06X: source %02X native %02X\n",
                        variant,i<0x80000?i:0xc00000+i-0x80000,expected[i],actual);return 1;
            }
        }
    }
    puts("Eight native touchdown mission-reset cases match original C083E2 non-stack RAM");
    free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
