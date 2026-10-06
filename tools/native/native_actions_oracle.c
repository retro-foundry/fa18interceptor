/* Compare typed C12950 action state and exact sound calls with original bytes. */
#define main records_fixture_main
#include "native_records_oracle.c"
#undef main
#include "control_actions.h"

typedef struct { unsigned count; ControlSoundCall calls[16]; } SoundTrace;
static SoundTrace source_trace,native_trace;
static const struct { gaddr entry;enum ControlSoundKind kind;unsigned count; } sound_sites[]={
    {0xc18096,CONTROL_SCRIPT,1},{0xc180fc,CONTROL_STOP_SCRIPT,0},
    {0xc17ef2,CONTROL_PROGRAM,9},{0xc17b08,CONTROL_FREE_VOICE,1},
    {0xc17f8c,CONTROL_EVENT_SIX,2},{0xc13176,CONTROL_EVENT_DISPATCH,2},
    {0xc17cf6,CONTROL_ENGINE,2},{0xc17c62,CONTROL_MAIN_ENGINE,2},
    {0xc17daa,CONTROL_ENGINE_SLIDE,3},{0xc17d6e,CONTROL_MAIN_ENGINE_SLIDE,3},
    {0xc17e4a,CONTROL_NOISE,1}
};
static void trace_native(void *context,const ControlSoundCall *call) {
    SoundTrace *trace=context;
    if(trace->count>=16) abort();
    trace->calls[trace->count++]=*call;
    consume_control_sound(call);
}
static int original_actions(void) {
    memset(REG_DA,0,sizeof REG_DA); REG_A[7]=0xc7ff00u;wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700);REG_PC=0xc12950u;
    fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned steps=0;steps<100000;++steps) {
        if(REG_PC==0xc70000u && REG_A[7]==0xc7ff04u) return 1;
        gaddr ret=rd_u32(REG_A[7]);
        if(ret>=0xc12950u && ret<0xc13134u) {
            for(unsigned i=0;i<sizeof sound_sites/sizeof sound_sites[0];++i) if(REG_PC==sound_sites[i].entry) {
                if(source_trace.count>=16) return 0;
                ControlSoundCall *call=&source_trace.calls[source_trace.count++];
                call->kind=sound_sites[i].kind;call->count=sound_sites[i].count;
                for(unsigned j=0;j<call->count;++j) call->argument[j]=rd_s32(REG_A[7]+4+4*j);
            }
        }
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"original action selector did not return at %06X\n",REG_PC);return 0;
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state || !rom || !data || nd!=0x100000 || !m || !before || !expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    static const uint8_t actions[]={0,0xfe,0xfd,0xfc,0xfb,0xfa,0x17};
    static const uint8_t phases[]={0,1,8,9,60,120,0x80,0xff};
    static const int16_t divisors[]={-16,-1,1,16,32};
    static const uint32_t pending[]={0x40,0x80,1,4,0x100,0x10,2,0x20,0x800,8,0xffff};
    unsigned calls=0;
    for(unsigned variant=0;variant<720;++variant) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        wr_u16(TARGET_RECORD,0);wr_u8(CONTEXT_STATE,0);wr_u8(PAUSE_A,0);wr_u8(POST_INPUT_EVENT,0);
        wr_u8(0xc45795u,1);wr_u8(0xc45785u,(uint8_t)((variant/56)%6));
        wr_u8(0xc457b5u,variant<336?0:1);wr_u8(0xc458b2u,(uint8_t)((int)(variant%13)-2));
        wr_u8(FIRE_STATE,actions[variant%7]);wr_u8(CONTROL_RECORDS+0x2b,phases[(variant/7)%8]);
        wr_u16(CONTROL_RECORDS+0x56,(uint16_t)((variant&1)?-127:208));
        wr_u16(CONTROL_RECORDS+0x5a,(uint16_t)((variant&2)?-96:16));
        wr_u16(CONTROL_RECORDS+0x6e,(uint16_t)((variant&4)?-32768:0x4000));
        wr_u16(CONTROL_RECORDS+2,(uint16_t)((variant&8)?0x88:0x80));
        wr_u8(CONTROL_RECORDS+0x7c,0);wr_u8(CONTROL_RECORDS+0x62,0x11);
        wr_u8(0xc45889u,variant&16?8:0);wr_u8(0xc45885u,(uint8_t)(variant%3==0?0xff:variant%3));
        wr_u8(FIRE_ALERT_COUNTDOWN,(uint8_t)(variant%3));wr_u8(0xc457b8u,variant&32?1:0);
        wr_u16(0xc45b42u,(uint16_t)divisors[(variant/7)%5]);
        wr_u8(0xc457c1u,variant%11?0:3);wr_u32(0xc45b54u,0);wr_u8(TONE_MUTE,0);
        if(variant>=672) {
            wr_u32(0xc45b54u,pending[(variant-672)%11]);
            if(variant&1) wr_u8(CONTROL_RECORDS+0x7c,3);
            if(variant&2) wr_u8(CONTROL_RECORDS+0x62,0x30);
            if(variant>=716) {
                if(variant==716) wr_u8(CONTEXT_STATE,1);
                if(variant==717) wr_u8(PAUSE_A,1);
                if(variant==718) wr_u8(POST_INPUT_EVENT,1);
                if(variant==719) wr_u8(0xc45795u,0);
            }
        }
        memcpy(before,m,sizeof *m);memset(&source_trace,0,sizeof source_trace);
        if(!original_actions()) {fprintf(stderr,"variant %u\n",variant);return 1;}
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);memset(&native_trace,0,sizeof native_trace);
        update_control_actions(trace_native,&native_trace);
        if(source_trace.count!=native_trace.count || memcmp(source_trace.calls,native_trace.calls,sizeof source_trace.calls)) {
            fprintf(stderr,"action variant %u: sound calls differ (%u/%u)\n",variant,source_trace.count,native_trace.count);
            for(unsigned i=0;i<source_trace.count || i<native_trace.count;++i) {
                fprintf(stderr,"call %u source/native kind %u/%u args",i,source_trace.calls[i].kind,native_trace.calls[i].kind);
                for(unsigned j=0;j<9;++j) fprintf(stderr," %d/%d",source_trace.calls[i].argument[j],native_trace.calls[i].argument[j]);
                fputc('\n',stderr);
            }
            return 1;
        }
        calls+=source_trace.count;
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"action variant %u %06X: source %02X native %02X\n",variant,
                    i<0x80000?i:i-0x80000+0xc00000,expected[i],actual);return 1;
            }
        }
    }
    printf("720 control-action cases match original non-stack RAM and %u exact sound calls\n",calls);
    free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
