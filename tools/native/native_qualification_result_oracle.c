/* Original result/restart parents with a shared native config-write boundary.
 * File I/O itself is the existing frontend save/reload contract, not this oracle. */
#define FA18_QUALIFICATION_ORACLE_LIBRARY
#define FA18_QUALIFICATION_SAVE_BACKEND
#include "native_qualification_oracle.c"
static unsigned saves;
static uint8_t saved_log[78];
void native_frontend_save_log(NativeFrontend *game) {
    (void)game;++saves;
    for(unsigned i=0;i<78;++i) saved_log[i]=rd_u8(rd_u32(MODE_TABLE)+i);
    wr_u8(MODE_TABLE_CHANGED,0);
}
static int source_result(gaddr entry) {
    memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00;REG_A[6]=0xc7ff70;
    wr_u32(REG_A[7],0xc70000);REG_PC=entry;
    m68k_set_reg(M68K_REG_SR,0x2700);fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned step=0;step<2000000;++step) {
        if(REG_PC==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        if(REG_PC==0xc1643a) {
            /* Both parents use the same external 78-byte config writer. */
            native_frontend_save_log(NULL);REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"result source did not return at %06X\n",REG_PC);return 0;
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns),*rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);uint8_t *expected=malloc(0x100000);
    if(!state||!rom||!data||nd!=0x100000||!m||!before||!expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;native_clock_set(12358);
    const gaddr entries[]={0xc11078,0xc110a4,0xc0f946,0xc0f974,0xc0f992};
    for(unsigned test=0;test<63;++test) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        unsigned owner=test%5,variant=test/5;gaddr entry=entries[owner];
        wr_u8(MODE_SELECT,9);wr_u8(RECORDER_MODE,0);wr_u8(PLAYER_PHASE,variant==2?0xef:0xff);
        wr_u16(POST_INPUT_COUNTDOWN,variant==0?2:0xffff);
        wr_u8(VIEWPORT_MODE,0);wr_u8(VIEWPORT_TARGET,variant==2?15:0);wr_u8(MODE_TABLE_CHANGED,1);
        if(test>=15) {
            owner=1;entry=entries[owner];wr_u16(POST_INPUT_COUNTDOWN,0xffff);
            wr_u8(MODE_SELECT,(uint8_t)(4+(test-15)%4));
            wr_u8(PLAYER_PHASE,(uint8_t[]){0xfe,0xfd,2}[((test-15)/4)%3]);
            wr_u16(0xc458da,(uint16_t)((test-15)/12));
        }
        if(test>=39) {
            owner=1;entry=entries[owner];
            const unsigned variant=(test-39)/4;
            const uint8_t levels[]={0,1,2,3,0x7f,0xff};
            const uint8_t attempts[]={0,2,3,0xfe,0xff,0x80};
            wr_u8(MODE_SELECT,(uint8_t)(4+(test-39)%4));wr_u8(PLAYER_PHASE,0xfc);
            wr_u16(POST_INPUT_COUNTDOWN,0xffff);
            wr_u8(SCENE_DISPATCH_LIMIT,levels[variant]);
            wr_u8(rd_u32(MODE_TABLE)+18+rd_u8(MODE_SELECT),attempts[variant]);
            wr_u16(rd_u32(MODE_TABLE)+0x38,variant&1?0xffff:0);
        }
        memcpy(before,m,sizeof *m);saves=0;
        if(!source_result(entry)) return 1;
        unsigned expected_saves=saves;uint8_t expected_log[78];memcpy(expected_log,saved_log,78);
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);saves=0;NativeFrontend game={.screen=NATIVE_SCENE_SETUP};
        stage(&game,entry);
        if(saves!=expected_saves || (saves && memcmp(saved_log,expected_log,78))) {
            fprintf(stderr,"result case %u config-write contract differs\n",test);return 1;
        }
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"result case %u parent %06X address %06X: source %02X native %02X\n",
                        test,entry,i<0x80000?i:0xc00000+i-0x80000,expected[i],actual);return 1;
            }
        }
        if(owner==4 && (game.screen!=NATIVE_MENU || game.record_updates!=1)) return 1;
    }
    puts("63 qualification/mission result/viewport/restart parents match original RAM with the shared config-write boundary (C1643A excluded)");
    free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
