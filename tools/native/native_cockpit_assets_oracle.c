/* Compare the actual C16982 cache/mask owner; allocations are shared external
 * boundaries. No reference memory is loaded into the native game runtime. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine.h"
#include "bus.h"
#include "m68kcpu.h"
#include "m68kops.h"
#include "recomp_runtime.h"
#include "recomp_ports.h"
#include "../../port/amiga/ilbm.c"
#include "../../port/game/native/cockpit_assets.c"
extern int64_t fa18_next_event;
uint8_t *native_storage_range(uint32_t address,size_t bytes) {
    if(address<0x80000 && bytes<=0x80000-address) return fa18_machine->chip+address;
    if(address>=0xc00000 && address<0xc80000 && bytes<=0xc80000-address) return fa18_machine->slow+address-0xc00000;
    abort();
}
static uint8_t *read_bytes(const char *path,size_t *size) {
    FILE *file=fopen(path,"rb");long length;uint8_t *bytes;
    if(!file || fseek(file,0,SEEK_END) || (length=ftell(file))<0 || fseek(file,0,SEEK_SET)) return NULL;
    bytes=malloc((size_t)length);
    if(!bytes || fread(bytes,1,(size_t)length,file)!=(size_t)length || fclose(file)) return NULL;
    *size=(size_t)length;return bytes;
}
static int run_original(gaddr workspace,gaddr mask) {
    unsigned allocations=0;
    memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00;
    wr_u32(REG_A[7],0xc70000);REG_PC=0xc16982;m68k_set_reg(M68K_REG_SR,0x2700);
    fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned step=0;step<100000;++step) {
        if(REG_PC==0xc70000 && REG_A[7]==0xc7ff04) return allocations==2;
        if(REG_PC==0xc53b30) {
            uint32_t expected=allocations?432:32;
            if(allocations>=2 || rd_u32(REG_A[7]+4)!=expected || rd_u32(REG_A[7]+8)!=0x10002) return 0;
            REG_D[0]=allocations++?mask:workspace;
            REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"cockpit cache source stopped at %06X\n",REG_PC);return 0;
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=read_bytes("captures/native/demo01/state.bin",&ns),*rom=read_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?read_bytes(argv[1],&nd):NULL;
    FA18Machine *machine=calloc(1,sizeof *machine),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state || !rom || !data || nd!=0x100000 || !machine || !before || !expected) return 1;
    if(!fa18_machine_load_state(machine,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    for(unsigned variant=0;variant<4;++variant) {
        memcpy(machine->chip,data,0x80000);memcpy(machine->slow,data+0x80000,0x80000);
        gaddr workspace=rd_u32(PANEL_WORKSPACE),mask=rd_u32(PANEL_MASK);
        for(unsigned p=0;p<4;++p) {
            wr_u32(INSTRUMENT_PLANES+4*p,0xdeadbeef);wr_u32(FRAME_PLANES+4*p,0xdeadbeef);
            wr_u32(PANEL_SUBIMAGES+4*p,0xdeadbeef);
        }
        wr_u32(PANEL_WORKSPACE,0xdeadbeef);wr_u32(PANEL_MASK,0xdeadbeef);
        memset(native_storage_range(mask,432),variant*0x55,432);
        if(variant) { /* Isolate each contributing plane, including a zero first plane. */
            for(unsigned p=0;p<4;++p) if(p!=variant)
                memset(native_storage_range(rd_u32(rd_u32(FRAME_OBJECT)+8+4*p),432),0,432);
        }
        memcpy(before,machine,sizeof *before);
        if(!run_original(workspace,mask)) return 1;
        memcpy(expected,machine->chip,0x80000);memcpy(expected+0x80000,machine->slow,0x80000);
        memcpy(machine,before,sizeof *before);native_cockpit_prepare(workspace,mask);
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?machine->chip[i]:machine->slow[i-0x80000];
            if(actual!=expected[i]) {fprintf(stderr,"cockpit cache variant %u offset %06X source %02X native %02X\n",variant,i,expected[i],actual);return 1;}
        }
    }
    puts("4 populated cockpit cache/mask cases match original C16982 non-stack RAM");
    free(expected);free(before);free(machine);free(data);free(state);free(rom);return 0;
}
