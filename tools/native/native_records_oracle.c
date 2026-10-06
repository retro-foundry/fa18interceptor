/* Validate the connected native record composition against original bytes.
 * CPU/ROM and exported native data are test inputs only. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "m68kcpu.h"
#include "m68kops.h"
#include "machine.h"
#include "bus.h"
#include "recomp_runtime.h"
#include "recomp_ports.h"
#include "memory.h"
#include "../../port/game/native/records.c"
#include "stages.h"
extern int64_t fa18_next_event;
static uint8_t *file_bytes(const char *name,size_t *size) {
    FILE *f=fopen(name,"rb"); long n; uint8_t *p;
    if(!f || fseek(f,0,SEEK_END) || (n=ftell(f))<0 || fseek(f,0,SEEK_SET)) return NULL;
    p=malloc((size_t)n); if(!p || fread(p,1,(size_t)n,f)!=(size_t)n || fclose(f)) return NULL;
    *size=(size_t)n; return p;
}
static int original(void) {
    unsigned steps;
    for(steps=0;steps<2000000;++steps) {
        uint16_t opcode;
        if(REG_PC==0xc70000u && REG_A[7]==0xc7ff04u) return 1;
        /* C53C78 is the external timer request. Supply the same host clock
         * to both paths; execute the surrounding C16D04 game code normally. */
        if(REG_PC==0xc53c78u) {
            clock_child(NULL,MC_TIMER_REQUEST);
            REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        opcode=rd_u16(REG_PC); REG_PPC=REG_PC; REG_IR=opcode; REG_PC+=2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"original record update did not return at %06X\n",REG_PC); return 0;
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0; char error[256]; unsigned i,differences=0,phase;
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=(argc==2 || argc==3)?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state || !rom || !data || (nd!=0x100000 && nd!=0x100048) || !m || !before || !expected) {
        fputs("Expected a native data export or reference RAM dump and local validation state/ROM\n",stderr);
        return 1;
    }
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) { fputs(error,stderr); return 1; }
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    memcpy(m->chip,data,0x80000); memcpy(m->slow,data+0x80000,0x80000);
    native_records_set_clock(rd_u32(MENU_TIME_REQUEST+32)*50u+rd_u32(MENU_TIME_REQUEST+36)/20000u);
    if(argc==3) native_records_set_clock((unsigned)strtoul(argv[2],NULL,10));
    for(phase=0;phase<2;++phase) {
    memset(REG_DA,0,sizeof REG_DA); REG_A[7]=0xc7ff00u; wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700); REG_PC=phase?0xc1c63eu:0xc12098u;
    fa18_next_event=INT64_MAX; SET_CYCLES(100000000);
    memcpy(before,m,sizeof *m);
    if(!original()) return 1;
    memcpy(expected,m->chip,0x80000); memcpy(expected+0x80000,m->slow,0x80000);
    memcpy(m,before,sizeof *m);
    if(phase) native_records_update(); else update_view_controls();
    /* Source stack temporaries have no native equivalent. Compare every other
     * byte, including control/workspace records, coordinates and game caches. */
    for(i=0;i<0xff000;++i) {
        uint8_t actual;
        if((i>=0x46fc && i<0x4718) || (i>=0x47ca && i<0x4800)) continue;
        actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
        if(actual!=expected[i]) {
            if(differences<20) fprintf(stderr,"%06X: source %02X native %02X\n",i<0x80000?i:i-0x80000+0xc00000,expected[i],actual);
            ++differences;
        }
    }
    if(differences) { fprintf(stderr,"%u non-stack differences at %s\n",differences,phase?"C1C63E":"C12098"); return 1; }
    }
    puts("Native view/control and record composition match original C12098/C1C63E non-stack RAM for this checkpoint");
    free(expected); free(before); free(m); free(data); free(state); free(rom); return 0;
}
