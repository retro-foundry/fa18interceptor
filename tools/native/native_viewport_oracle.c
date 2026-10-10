/* Complete C1718E and C17104/C1712C startup, with host samples/publication. */
#define main records_fixture_main
#include "native_records_oracle.c"
#undef main
#include "../../../fa18-interceptor-decomp/port/game/native/viewport.c"

static uint16_t source_palette[32];
static int source_call(gaddr entry) {
    memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00;wr_u32(REG_A[7],0xc70000);
    REG_PC=entry;m68k_set_reg(M68K_REG_SR,0x2700);
    fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned step=0;step<10000;++step) {
        if(REG_PC==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        if(REG_PC==0xc53b00) {
            if(rd_u32(REG_A[7]+4)!=5 || rd_u32(REG_A[7]+8)!=0xc1abf0) return 0;
            REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        if(REG_PC==0xc53ec0) {
            gaddr colors=rd_u32(REG_A[7]+8);
            if(rd_u32(REG_A[7]+4)!=0xc1822a || rd_u32(REG_A[7]+12)!=16) return 0;
            for(unsigned i=0;i<16;++i) source_palette[i]=rd_u16(colors+2*i);
            REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"source input callback did not complete at %06X\n",REG_PC);return 0;
}
static void source_counters(FA18Machine *m,uint16_t raw) {
    m->joy0dat=raw;m->mouse_x=raw&255;m->mouse_y=raw>>8;
}
static uint16_t counter_sample(unsigned test,unsigned tick) {
    static const int delta[]={0,1,-1,127,-128,128,-129,255};
    unsigned x=(test*31u+(unsigned)delta[(test+tick)&7]*tick)&255;
    unsigned y=(test*57u+(unsigned)delta[((test>>3)+tick)&7]*tick)&255;
    return (uint16_t)(y<<8|x);
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
        uint16_t raw=counter_sample(test,0);source_counters(m,raw);
        memcpy(before,m,sizeof *m);
        wr_s32(0xc7ff04,-960);wr_s32(0xc7ff08,-960);
        wr_s32(0xc7ff0c,960);wr_s32(0xc7ff10,960);
        if(!source_call(0xc17104) || !source_call(0xc1712c)) return 1;
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);
        game->input_server_installed=0;game->mouse_x_counter=raw&255;game->mouse_y_counter=raw>>8;
        native_viewport_initialize(game);
        if(!game->input_server_installed) return 1;
        for(unsigned i=0;i<0xffc00;++i) {
            if(i>=PALETTE_FRAME-22 && i<PALETTE_FRAME+24) continue;
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {fprintf(stderr,"input startup case %u at %06X\n",test,i);return 1;}
        }
        static const int16_t positions[]={-32768,-961,-960,-129,127,960,961,32767};
        wr_s16(0xc45776,positions[test&7]);wr_s16(0xc45778,positions[(test>>3)&7]);
        wr_u16(0xc45774,test&1?0xffff:(uint16_t)test);wr_u8(PLAYER_READY,test&1);
        /* Both ordinary limits and signed/inverted extremes preserve the
         * source's clamp ordering, even when a caller's bounds are inverted. */
        if(test&32) {
            wr_s16(0xc081ac,-32768);wr_s16(0xc081b0,-1);
            wr_s16(0xc081ae,test&64?32767:-128);wr_s16(0xc081b2,test&64?-32768:127);
        }
        wr_u8(VIEWPORT_MODE,test&1?15:0);wr_u8(VIEWPORT_TARGET,test&2?15:0);
        wr_u8(0xc458a3,(uint8_t[]){0,1,2,127,128,255,254,3}[(test>>2)&7]);
        wr_u8(TABLE_CLEAR_MODE,(test>>5)&3);wr_u16(DRAW_PAGE,test&1);
        wr_u8(VOLUME_FADING,test&32?1:0);wr_u32(MASTER_VOLUME,test&64?0x3f0000:0);
        wr_u32(MASTER_VOLUME_TARGET,test&16?0x1f0000:0x3f0000);
        for(unsigned i=0;i<32;++i) game->palette[i]=source_palette[i]=(uint16_t)(0x740+i);
        memcpy(before,m,sizeof *m);
        /* A whole 50-tick interval covers multi-step fades, both directions,
         * terminal palette copies and stable publication on either page. */
        for(unsigned tick=0;tick<50;++tick) {
            source_counters(m,counter_sample(test,tick));if(!source_call(0xc1718e)) return 1;
        }
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);
        for(unsigned tick=0;tick<50;++tick) {
            raw=counter_sample(test,tick);game->mouse_x_counter=raw&255;game->mouse_y_counter=raw>>8;
            native_viewport_tick(game);
        }
        for(unsigned i=0;i<0xffc00;++i) {
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
    puts("128 input bounds/counter/server startups and 6400 complete C1718E ticks match original non-stack RAM and RGB4 publication");
    free(expected);free(game);free(before);free(m);free(data);free(state);free(rom);return 0;
}
