/* Complete aircraft C30A00 and its real C0F138-C0F2DC HUD caller. */
#define FA18_HUD_ORACLE_LIBRARY
#include "native_hud_oracle.c"
#include "../../port/game/cockpit.h"

static int stores_source(int full,uint16_t tick,uint32_t inherited,unsigned test) {
    memset(REG_DA,0,sizeof REG_DA);REG_D[4]=inherited;REG_D[5]=test*37;
    REG_A[4]=rd_u16(LINE_LAST_ROW);REG_A[6]=0xc7ff70;REG_A[7]=0xc7ff00;
    wr_u16(REG_A[6]-2,tick);wr_u32(REG_A[7],0xc70000);
    m68k_set_reg(M68K_REG_SR,0x2700);REG_PC=full?0xc0f138:0xc30a00;
    fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned step=0;step<2000000;++step) {
        if(REG_PC==(full?0xc0f2dc:0xc70000) && REG_A[7]==(full?0xc7ff00:0xc7ff04)) {
            wait_blitter();return 1;
        }
        if(REG_PC==0xc53f4c) {
            REG_A[7]-=4;wr_u32(REG_A[7],0xc53f50);REG_PC=0xfc5a58;continue;
        }
        int cycles=GET_CYCLES();uint16_t opcode=rd_u16(REG_PC);
        REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
        fa18_machine->cycle+=cycles-GET_CYCLES();
    }
    fprintf(stderr,"source stores/HUD stopped at %06X\n",REG_PC);return 0;
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
    for(unsigned group=0;group<3;++group) {
        unsigned count=group==0?1400:group==1?280:84;
        for(unsigned test=0;test<count;++test) {
            memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
            int16_t span=rd_s8(0xc1bad4+test%14);
            uint16_t tick=(uint16_t)(test&31);
            wr_u16(SPAN_ORIGIN,(uint16_t)span);wr_u16(SPAN_ORIGIN_Y,(uint16_t)(span*16));
            int16_t row=(int16_t[]){0,55,-55}[test/14%3];
            wr_s16(REDRAW_STATE_WORD,row);wr_s32(REDRAW_STATE_LONG,row*40);
            wr_u16(VIEW_RECORD,0);wr_u16(TARGET_RECORD,0);
            wr_u8(CONTROL_RECORDS+0x62,0x11);
            wr_u8(CONTROL_RECORDS+0x63,(uint8_t)(test/350%4*16|13));
            wr_u8(CONTROL_RECORDS+0x5f,(uint8_t)((test/14%5)<<4|test/70%5));
            wr_u8(STORES_REDRAWS,3);wr_u8(WEAPON_REDRAWS,3);
            if(group==1) {
                wr_u8(ORIGIN_ENABLE,0);
                request_cockpit_redraw();
                wr_u8(UPDATE_ACTIVITY,(uint8_t[]){3,0,255}[test/14%3]);
                /* Restore the explicit source slide values after redraw. */
                wr_s16(REDRAW_STATE_WORD,row);wr_s32(REDRAW_STATE_LONG,row*40);
                wr_u8(CONTROL_RECORDS+0x63,(uint8_t)(test/14%4*16|13));
                unsigned stock=test/56;
                wr_u8(CONTROL_RECORDS+0x5f,(uint8_t)(stock<<4|(4-stock)));
                /* C1538E can dirty stores without dirtying the weapon text. */
                wr_u8(WEAPON_REDRAWS,(uint8_t)(test&1?0:3));
            } else if(group==2) {
                wr_u8(STORES_REDRAWS,(uint8_t[]){0,255,128,1,3,3}[test/14]);
                if(test/14==5) wr_u8(CONTROL_RECORDS+0x62,0);
            }
            memcpy(before,m,sizeof *m);
            if(group==1) native_hud_draw(tick);else native_hud_draw_stores();
            memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
            memcpy(m,before,sizeof *m);
            uint32_t inherited=(uint32_t[]){0,0xc3d790,0xffffffff,0x80000000}[test%4];
            if(!stores_source(group==1,tick,inherited,test)) return 1;
            unsigned differences=0;
            for(unsigned i=0;i<0xffc00;++i) {
                uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
                if(actual!=expected[i]) {
                    if(differences<10) fprintf(stderr,"stores group %u case %u at %06X: source %02X native %02X\n",
                        group,test,i<0x80000?i:i-0x80000+0xc00000,actual,expected[i]);
                    ++differences;
                }
            }
            if(differences) {fprintf(stderr,"%u stores/HUD differences\n",differences);return 1;}
        }
    }
    puts("1484 source stores cases and 280 complete normal HUD cases match all non-stack RAM");
    free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
