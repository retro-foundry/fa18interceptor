/* Compare actual native frame assembly C0EFEA-C0F3C0 against original bytes.
 * Before/after exports come from the actual runner; no native body is copied
 * into this oracle. CPU/chipset/ROM belong only to the reference process. */
#define main records_fixture_main
#include "native_records_oracle.c"
#undef main
#include "hardware.h"

/* Supply the native PAL interval through the complete original input callback.
 * RGB4 publication is a host boundary and excluded from drawing
 * acceptance; its game-side control stores still execute original bytes. */
static int palette_tick(void) {
    uint32_t saved[16],pc=REG_PC;memcpy(saved,REG_DA,sizeof saved);
    uint16_t sr=(uint16_t)m68k_get_reg(NULL,M68K_REG_SR);
    REG_A[7]=0xc7fd80;wr_u32(REG_A[7],0xc70000);REG_PC=0xc1718e;
    for(unsigned step=0;step<10000;++step) {
        if(REG_PC==0xc70000 && REG_A[7]==0xc7fd84) {
            memcpy(REG_DA,saved,sizeof saved);REG_PC=pc;m68k_set_reg(M68K_REG_SR,sr);return 1;
        }
        if(REG_PC==0xc53ec0) {REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;}
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fputs("Original palette callback did not finish\n",stderr);return 0;
}

int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0,ne=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    const int owner_exit=argc==8 && !strcmp(argv[7],"owner-exit");
    uint8_t *data=(argc==6 || argc==7 || owner_exit)?file_bytes(argv[1],&nd):NULL;
    uint8_t *expected=(argc==6 || argc==7 || owner_exit)?file_bytes(argv[2],&ne):NULL;
    FA18Machine *m=calloc(1,sizeof *m);
    if(!state||!rom||!data||!expected||nd!=0x100000||ne!=nd||!m) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) return 1;
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
    /* These keyboard-only paths supply no intervening mouse movement. */
    m->joy0dat=(uint16_t)(rd_u16(0xc1ac08)<<8|rd_u16(0xc1ac06));
    m->mouse_x=m->joy0dat&255;m->mouse_y=m->joy0dat>>8;
    unsigned first_tick=(unsigned)strtoul(argv[3],NULL,10),last_tick=(unsigned)strtoul(argv[4],NULL,10);
    native_clock_set(first_tick);unsigned in_timer=0,timer_samples=0;
    memset(REG_DA,0,sizeof REG_DA);REG_A[6]=0xc7ff80;REG_A[7]=0xc7ff00;
    wr_u32(REG_A[6],0);wr_u32(REG_A[6]+4,0xc70000);
    wr_u16(REG_A[6]-2,(uint16_t)strtoul(argv[5],NULL,10));REG_A[4]=rd_u16(LINE_LAST_ROW);
    REG_PC=0xc0efea;m68k_set_reg(M68K_REG_SR,0x2700);
    fa18_next_event=INT64_MAX;SET_CYCLES(1000000000);
    unsigned step;
    for(step=0;step<10000000;++step) {
        if((!owner_exit && REG_PC==0xc0f3c0 && REG_A[7]==0xc7ff00) ||
           (owner_exit && REG_PC==0xc70000 && REG_A[7]==0xc7ff88)) {wait_blitter();break;}
        if(REG_PC==0xc25312) in_timer=1;
        if(REG_PC==0xc53c78) {
            if(in_timer && timer_samples++==1) {
                for(unsigned tick=first_tick;tick<last_tick;++tick) if(!palette_tick()) return 1;
                native_clock_set(last_tick);
            }
            native_clock_request();REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        if(REG_PC==0xc53f9c) {REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;}
        if(REG_PC==0xc53f4c) {
            REG_A[7]-=4;wr_u32(REG_A[7],0xc53f50);REG_PC=0xfc5a58;continue;
        }
        int cycles=GET_CYCLES();uint16_t opcode=rd_u16(REG_PC);
        REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
        m->cycle+=cycles-GET_CYCLES();
    }
    if(step==10000000) {fprintf(stderr,"Frame body did not return at %06X\n",REG_PC);return 1;}
    unsigned differences=0,plane_differences=0,scratch_differences=0,voice_differences=0,busy_differences=0;
    uint8_t *voice_mask=calloc(0x100000,1);
    if(!voice_mask) return 1;
    /* Current native resource owners populate slots 0..36. */
    for(unsigned slot=0;slot<37;++slot) {
        gaddr record=rd_u32(SOUND_VOICES+4*slot);
        if(!record) continue;
        unsigned offset=record<0x80000?record:record-0xc00000+0x80000;
        if(offset>0x100000-64) return 1;
        memset(voice_mask+offset,1,64);
    }
    memset(voice_mask+VOICE_SLOTS-0xc00000+0x80000,1,16);
    for(unsigned i=0;i<0xffc00;++i) {
        uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
        if(actual==expected[i]) continue;
        /* Ordinary native locals have no original ABI-stack equivalent.
         * Samples are asynchronous outside this frame-body fixture; their
         * callbacks and PCM streams have separate complete source checks.
         * Synchronous native raster submission has no blitter busy polls. */
        if((i>=0x3f30 && i<0x4b10) || (i>=0x306a && i<0x3080)) {++scratch_differences;continue;}
        if(voice_mask[i]) {++voice_differences;continue;}
        if(i>=ACTIVE_PLANE_BUSY_1-0xc00000+0x80000 && i<ACTIVE_PLANE_BUSY_3-0xc00000+0x80000+4) {
            ++busy_differences;continue;
        }
        if(differences<70) fprintf(stderr,"frame %06X source %02X native %02X\n",
            i<0x80000?i:i-0x80000+0xc00000,actual,expected[i]);
        ++differences;if(i>=0x34000 && i<0x4a000) ++plane_differences;
    }
    printf("Frame body: %u gameplay differences, %u display bytes; %u instructions, %u timer samples; excluded scratch=%u voice=%u busy=%u\n",
        differences,plane_differences,step,timer_samples,scratch_differences,voice_differences,busy_differences);
    if(argc>=7) {
        FILE *out=fopen(argv[6],"wb");
        if(!out || fwrite(m->chip,1,0x80000,out)!=0x80000 || fwrite(m->slow,1,0x80000,out)!=0x80000 || fclose(out)) return 1;
    }
    free(voice_mask);free(m);free(expected);free(data);free(state);free(rom);
    return differences?1:0;
}
