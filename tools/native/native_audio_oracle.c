/* C50158 with its complete C50212/C501E0 children. The machine is only
 * an oracle; production publication stores ordinary native channel levels. */
#define main records_fixture_main
#include "native_records_oracle.c"
#undef main
#include "../../../fa18-interceptor-decomp/port/game/native/audio.c"

static int source_voice_tick(void) {
    memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00;
    wr_u32(REG_A[7],0xc70000);REG_PC=0xc50158;
    m68k_set_reg(M68K_REG_SR,0x2700);fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned step=0;step<10000;++step) {
        if(REG_PC==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"source voice tick did not return at %06X\n",REG_PC);return 0;
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
    /* Five original program streams, idle voices, wrap/expiration of slides,
     * signed output clamping, empty slots and aliased slot pointers. */
    static const gaddr programs[]={0,0xc50b78,0xc50bd8,0xc50c00,0xc50c30,0xc50c70};
    static const uint32_t periods[]={0,0x7b0000,0x7c0000,0x136ffff,0x80000000,0x7fffffff};
    static const uint32_t volumes[]={0,0x3f0000,0x400000,0xffff0000,0x7fffffff,0x80000000};
    static const uint32_t durations[]={0,1,2,0xffffffff};
    for(unsigned test=0;test<145;++test) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        NativeAudio audio={0};
        if(test>=128 && test<144) {
            gaddr records[4];
            for(unsigned c=0;c<4;++c) records[c]=rd_u32(VOICE_TABLE+4*c);
            for(unsigned c=0;c<4;++c) wr_u32(VOICE_TABLE+4*c,records[3-c]);
        }
        for(unsigned channel=0;channel<4;++channel) {
            unsigned c=(test+channel)%6;
            m->custom[(0xa6+16*channel)/2]=137+channel;
            m->custom[(0xa8+16*channel)/2]=11+channel;
            audio.channels[channel]=(VoiceOutput){(int16_t)(137+channel),(int16_t)(11+channel)};
            if(test==144) continue; /* Unmodified real runner checkpoint. */
            gaddr record=rd_u32(VOICE_TABLE+4*channel),voice=0x6000+64*channel;
            memset(m->chip+voice,0,64);
            gaddr slot=VOICE_SLOTS+4*(test&64?channel/2:channel);
            wr_u32(record+4,slot);
            wr_u32(slot,test&32 && channel==2?0:voice);
            wr_u32(voice+VOICE_PERIOD,periods[c]);wr_u32(voice+VOICE_VOLUME,volumes[c]);
            wr_u32(voice+VOICE_PROGRAM,programs[c]);
            wr_u32(voice+VOICE_DELAY,programs[c]?1+(test&1):0);
            wr_u32(voice+VOICE_LOOP_COUNTERS,(test>>1)%3);
            wr_u32(voice+VOICE_LOOP_COUNTERS+4,(test>>2)%3);
            wr_u32(voice+VOICE_PERIOD_SLIDE,0xfffe4567);
            wr_u32(voice+VOICE_VOLUME_SLIDE,0x1abcd);
            wr_u32(voice+VOICE_PERIOD_TICKS,durations[(test+channel)%4]);
            wr_u32(voice+VOICE_VOLUME_TICKS,durations[(test+channel+1)%4]);
        }
        if(test!=144) wr_u32(MASTER_VOLUME,(uint32_t[]){0,0x1f0000,0x3f0000,0xffff0000}[test%4]);
        for(unsigned tick=0;tick<50;++tick) {
            memcpy(before,m,sizeof *m);
            if(!source_voice_tick()) return 1;
            VoiceOutput outputs[4];
            for(unsigned c=0;c<4;++c) outputs[c]=(VoiceOutput){
                (int16_t)m->custom[(0xa6+16*c)/2],(int16_t)m->custom[(0xa8+16*c)/2]};
            memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
            memcpy(m,before,sizeof *m);native_audio_tick(&audio);
            for(unsigned i=0;i<0xffc00;++i) {
                uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
                if(actual!=expected[i]) {
                    fprintf(stderr,"voice case %u tick %u at %06X: source %02X native %02X\n",
                            test,tick,i<0x80000?i:i-0x80000+0xc00000,expected[i],actual);return 1;
                }
            }
            if(memcmp(outputs,audio.channels,sizeof outputs)) {
                fprintf(stderr,"voice output differs in case %u tick %u\n",test,tick);return 1;
            }
            /* Carry the native result forward, including outputs retained
             * for empty channels. No sample/DMA callback executes here. */
            for(unsigned c=0;c<4;++c) {
                m->custom[(0xa6+16*c)/2]=(uint16_t)audio.channels[c].period;
                m->custom[(0xa8+16*c)/2]=(uint16_t)audio.channels[c].volume;
            }
        }
    }
    puts("145 voice sequences (7250 callbacks) match complete C50158 RAM and period/volume output");
    free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
