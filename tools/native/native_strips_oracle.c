/* Original strip opcodes are validation-only; gameplay uses typed C. */
#define main records_fixture_main
#include "native_records_oracle.c"
#undef main
#include "model_strips.h"

static int original_strips(gaddr input,gaddr output,const int16_t origin[3],int shift) {
    memset(REG_DA,0,sizeof REG_DA);
    REG_D[0]=(uint32_t)(int32_t)origin[0];REG_D[1]=(uint32_t)(int32_t)origin[2];
    REG_A[1]=input;REG_A[3]=output;REG_A[5]=(uint32_t)(int32_t)origin[1];
    REG_A[6]=0x4800;REG_A[7]=0x4a00;
    wr_u16(REG_A[6]-8,(uint16_t)shift);
    m68k_set_reg(M68K_REG_SR,0x2700);REG_PC=0xc1f584u;
    fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned steps=0;steps<200000;++steps) {
        if(REG_PC==0xc1f6f8u) return 1;
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"strip source did not finish at %06X\n",REG_PC);return 0;
}
static void fixture_point(gaddr *input,unsigned variant,unsigned index) {
    for(unsigned k=0;k<3;++k) {
        int16_t value=(int16_t)(variant>=80?0x7000u+index*0x4321u+k*0x9876u:
                               (int)(variant*17u+index*103u+k*239u)-700);
        wr_u16(*input,(uint16_t)value);*input+=2;
    }
}
static void fixture_stream(unsigned variant) {
    gaddr input=0x6000;
    static const int16_t divisions[]={-3,0,1,2,3,5,8,11};
    if(variant<2) {wr_u16(input,variant?0x8000u:0xffffu);return;}
    for(unsigned group=0;group<(variant&8?2u:1u);++group) {
        unsigned segments=variant%3+1;
        wr_u16(input,variant&16?0:(uint16_t)segments);input+=2;
        fixture_point(&input,variant,group*9);
        if(variant&16) segments=1;
        for(unsigned j=0;j<segments;++j) {
            fixture_point(&input,variant,group*9+j+1);
            wr_u16(input,(uint16_t)divisions[(variant+j)%8]);input+=2;
        }
        unsigned reverse=variant&4?2:variant&2?1:0;
        wr_u16(input,(uint16_t)(reverse?reverse|(variant&1?0x8000u:0):0));input+=2;
        for(unsigned j=0;j<reverse;++j) {
            fixture_point(&input,variant,group*9+j+5);
            wr_u16(input,(uint16_t)divisions[(variant+j+3)%8]);input+=2;
        }
    }
    wr_u16(input,0xffffu);
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
    static const int shifts[]={0,1,4,8,15,16,63,-1};
    for(unsigned variant=0;variant<96;++variant) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        fixture_stream(variant);
        int16_t origin[3]={(int16_t)(variant*113u),(int16_t)(-700-(int)variant*57),
                           (int16_t)(variant*337u)};
        int shift=shifts[variant%8];
        if(variant>=80) for(int k=0;k<9;++k) wr_u16(VIEW_ANGLE_MATRIX+2*k,(uint16_t)(0x7fff-k*7919));
        memcpy(before,m,sizeof *m);
        if(!original_strips(0x6000,WORKSPACES+0x900,origin,shift)) return 1;
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);
        transform_model_strips(0x6000,WORKSPACES+0x900,origin,shift);
        for(unsigned i=0;i<0x100000;++i) {
            if((i>=0x4780 && i<0x4800)||(i>=0x49f0 && i<0x4a00)) continue;
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"strip case %u %06X: source %02X native %02X\n",variant,
                    i<0x80000?i:i-0x80000+0xc00000,expected[i],actual);return 1;
            }
        }
    }
    puts("96 paired-strip cases match original C1F584-C1F6F8 non-stack RAM");
    free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
