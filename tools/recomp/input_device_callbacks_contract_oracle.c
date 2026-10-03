/* Complete input-device-callbacks proof, including complete child-entry CPU/RAM contracts. */
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
#include "ports_glue.h"
#include "globals.h"

extern int fa18_write_log_active;
extern void fa18_structural_reset_write_log(void);
extern int64_t fa18_next_event;
static uint32_t seed=0xc0f5f8u;
static uint32_t random_value(void) {
    seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed;
}
static uint8_t *read_file(const char *path,size_t *size) {
    FILE *file=fopen(path,"rb"); long length; uint8_t *bytes;
    if(!file) return NULL;
    if(fseek(file,0,SEEK_END) || (length=ftell(file))<0 || fseek(file,0,SEEK_SET)) return NULL;
    bytes=malloc((size_t)length);
    if(!bytes || fread(bytes,1,(size_t)length,file)!=(size_t)length || fclose(file)) return NULL;
    *size=(size_t)length; return bytes;
}
static uint32_t selected_entry=0xc1718eu;
extern void input_device_callbacks_contract_reset(unsigned scenario,int source);
extern void input_device_callbacks_contract_enter(uint32_t entry,uint32_t ret);
extern void input_device_callbacks_contract_finish(void);
static const uint32_t source_boundaries[]={0xC16B8Cu,0xC16B8Eu,0xC16B90u,0xC16B96u,0xC16B98u,0xC16B9Eu,0xC16BA4u,0xC16BA8u,0xC16BAEu,0xC16BB4u,0xC16BBAu,0xC16BBCu,0xC16BC2u,0xC16BCAu,0xC16BCCu,0xC16BD0u,0xC16BD6u,0xC16BDAu,0xC16BE0u,0xC16BE2u,0xC16BE8u,0xC16BEEu,0xC16BF0u,0xC16CD8u,0xC16CDCu,0xC16CDEu,0xC16CE0u,0xC16CE6u,0xC16CE8u,0xC16CEEu,0xC16CF4u,0xC16CF8u,0xC16CFCu,0xC16CFEu,0xC16D00u,0xC16D02u,0xC17104u,0xC17108u,0xC17110u,0xC17118u,0xC17120u,0xC17128u,0xC1712Au,0xC1712Cu,0xC17130u,0xC17136u,0xC1713Au,0xC1713Eu,0xC17142u,0xC17144u,0xC17148u,0xC1714Eu,0xC17154u,0xC17158u,0xC1715Au,0xC1718Eu,0xC17192u,0xC17196u,0xC1719Cu,0xC171A0u,0xC171A4u,0xC171A8u,0xC171AAu,0xC171AEu,0xC171B2u,0xC171B8u,0xC171BCu,0xC171C2u,0xC171C6u,0xC171CAu,0xC171CEu,0xC171D0u,0xC171D6u,0xC171D8u,0xC171DCu,0xC171E0u,0xC171E2u,0xC171E8u,0xC171ECu,0xC171F0u,0xC171F2u,0xC171F8u,0xC171FAu,0xC171FEu,0xC17202u,0xC17204u,0xC1720Au,0xC17210u,0xC17212u,0xC17214u,0xC17218u,0xC1721Au,0xC1721Eu,0xC17220u,0xC17224u,0xC1722Au,0xC1722Eu,0xC17234u,0xC1723Au,0xC1723Eu,0xC17244u,0xC1724Au,0xC17250u,0xC17252u,0xC17254u,0xC1725Au,0xC17260u,0xC17262u,0xC17264u,0xC1726Au,0xC17270u,0xC17272u,0xC17274u,0xC1727Au,0xC1727Cu,0xC17282u,0xC17288u,0xC1728Eu,0xC17290u,0xC17292u,0xC17298u,0xC1729Eu,0xC172A0u,0xC172A2u,0xC172A8u,0xC172AEu,0xC172B0u,0xC172B2u,0xC172B8u,0xC172BAu,0xC172C0u,0xC172C8u,0xC172D0u,0xC172D6u,0xC172D8u,0xC172DEu,0xC172E4u,0xC172EAu,0xC172ECu,0xC172F0u,0xC172F6u,0xC172F8u,0xC172FEu,0xC17300u,0xC17304u,0xC17306u,0xC17308u,0xC1730Au,0xC17310u,0xC17318u,0xC1731Au,0xC17320u,0xC17322u,0xC17328u,0xC17330u,0xC17336u,0xC1733Cu,0xC1733Eu,0xC17340u,0xC17342u,0xC17344u,0xC17346u,0xC17348u,0xC1734Au,0xC1734Cu,0xC1734Eu,0xC17350u,0xC17356u,0xC1735Au,0xC17360u,0xC17364u,0xC1736Au,0xC1736Cu,0xC1736Eu,0xC17372u,0xC17374u,0xC17376u,0xC17378u,0xC1737Eu,0xC17384u,0xC17388u,0xC1738Au,0xC1738Cu,0xC1738Eu,0xC17394u,0xC1739Au,0xC1739Cu,0xC1739Eu,0xC173A2u,0xC173A8u,0xC173AEu,0xC173B2u,0xC173B6u,0xC173B8u,0xC173BAu,0xC173BCu,0xC173C2u,0xC173C8u,0xC173CCu,0xC173CEu,0xC173D0u,0xC173D2u,0xC173D8u,0xC173DEu,0xC173E4u,0xC173EAu,0xC173ECu,0xC173EEu,0xC173F6u,0xC173FAu,0xC17400u,0xC17402u,0xC17406u,0xC1740Au,0xC1740Cu,0xC17410u,0xC17414u,0xC17418u,0xC1741Au,0xC17422u,0xC17424u,0xC1742Au,0xC1742Cu,0xC1742Eu,0xC17430u,0xC17436u,0xC1743Cu,0xC17442u,0xC17446u,0xC1744Cu,0xC1744Eu,0xC17452u,0xC17454u,0xC17456u,0xC1745Eu,0xC17464u,0xC1746Eu,0xC17472u,0xC17478u,0xC1747Eu,0xC17480u,0xC17482u,0xC17488u,0xC1748Au,0xC1748Cu,0xC17492u,0xC17494u,0xC17496u,0xC1749Cu,0xC1749Eu,0xC174A0u,0xC174A4u,0xC174A6u,0xC174AAu,0xC174AEu,0xC174B2u,0xC174B6u,0xC174BCu,0xC174BEu,0xC174C0u,0xC174C6u,0xC174C8u,0xC174CCu,0xC174D0u,0xC174D2u,0xC174D8u,0xC174DAu,0xC174DEu,0xC174E2u,0xC174E6u,0xC174E8u,0xC174EEu,0xC174F0u,0xC174F2u};
static unsigned char visited[FA18_SLOW_SIZE/2];
static int source_owned(uint32_t pc) { unsigned i; for(i=0;i<sizeof source_boundaries/sizeof source_boundaries[0];++i) if(source_boundaries[i]==pc) return 1; return 0; }
static unsigned source_slot(uint32_t pc) { return (pc-FA18_SLOW_BASE)/2; }
static uint32_t source_pc(unsigned slot) { return FA18_SLOW_BASE+2*slot; }
static int source_call(uint32_t ret,uint32_t sp) {
    unsigned dispatch;
    uint32_t child_ret=0,child_sp=0;
    for(dispatch=0;dispatch<1000000;++dispatch) {
        int lo=0,hi=fa18_recomp_entry_count,result;
        if(REG_PC==ret && REG_A[7]==sp) return FA18_RET;
        if(source_owned(REG_PC)) {
            uint32_t pc=REG_PC;
            uint16_t opcode=m68k_read_memory_16(pc);
            visited[source_slot(pc)]=1;
            child_ret=0;
            REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
            m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
            if((opcode&0xff00u)==0x6100u || (opcode&0xffc0u)==0x4e80u)
                input_device_callbacks_contract_enter(REG_PC,rd_u32(REG_A[7]));
            continue;
        }
        if(!child_ret) { child_ret=rd_u32(REG_A[7]); child_sp=REG_A[7]+4; }
        while(lo<hi) {
            int mid=lo+(hi-lo)/2;
            if(fa18_recomp_entries[mid].pc<REG_PC) lo=mid+1; else hi=mid;
        }
        if(lo==fa18_recomp_entry_count || fa18_recomp_entries[lo].pc!=REG_PC)
            result=fa18_recomp_resume(child_ret,child_sp);
        else result=fa18_recomp_functions[fa18_recomp_entries[lo].function].fn((int)fa18_recomp_entries[lo].label);
        if(result==FA18_EXIT_INTERP) return result;
    }
    return FA18_EXIT_INTERP;
}
static uint32_t expected_sp=0xc7ff04u;
static void fixture(unsigned scenario) {
    static const uint8_t bytes[]={0,1,2,3,15,0x7f,0x80,0xff};
    static const uint16_t previous[]={0,1,127,128,129,255,0x7fff,0x8000};
    static const uint16_t positions[]={0,1,127,128,0x7fff,0x8000,0xffff,0xff80};
    unsigned i,profile=scenario/32u; uint16_t raw;
    for(i=0;i<15;++i) REG_DA[i]=random_value(); REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    for(i=0;i<256;++i) wr_u32(0xc60400u+4*i,random_value());
    wr_u32(0xc08134u,0xc60400u); wr_u32(0xc0815cu,0xc60500u); wr_u32(LONG_TABLE,0xc60600u);
    raw=(uint16_t)(bytes[(profile/8u)%8u]<<8|bytes[profile%8u]);
    fa18_machine->joy0dat=raw; fa18_machine->mouse_x=raw&255u; fa18_machine->mouse_y=raw>>8;
    fa18_machine->mouse_dx=fa18_machine->mouse_dy=0;
    wr_u16(0xc1ac06u,previous[(profile/8u)%8u]); wr_u16(0xc1ac08u,previous[profile%8u]);
    wr_u16(0xc45776u,positions[(profile/4u)%8u]); wr_u16(0xc45778u,positions[(profile/3u)%8u]);
    wr_u16(0xc45774u,(uint16_t)random_value()); wr_u8(PLAYER_READY,(uint8_t)(profile&1u));
    wr_u16(0xc081acu,(profile&2u)?0x8000u:0); wr_u16(0xc081b0u,(profile&4u)?0xffffu:0x7fffu);
    wr_u16(0xc081aeu,(profile&8u)?0x7fffu:0xff80u); wr_u16(0xc081b2u,(profile&16u)?0x8000u:127);
    wr_u8(VIEWPORT_MODE,bytes[(profile/2u)%8u]);
    wr_u8(VIEWPORT_TARGET,(profile&1u)?bytes[(profile/2u)%8u]:bytes[(profile/2u+1u)%8u]);
    wr_u8(0xc458a3u,bytes[(profile/16u)%8u]); wr_u8(TABLE_CLEAR_MODE,(uint8_t)(profile&2u));
    wr_u16(DRAW_PAGE,(uint16_t)(profile&1u)); wr_u8(VOLUME_FADING,0);
    /* Original saved View pointers from the sealed machine remain intact. */
    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; expected_sp=REG_A[7]+4; wr_u32(REG_A[7],0xc70000u);
    wr_u32(REG_A[7]+4,0xc60500u); wr_u32(REG_A[7]+8,random_value());
    wr_u32(REG_A[7]+12,random_value()); wr_u32(REG_A[7]+16,random_value());
    m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31u)); REG_PC=selected_entry;
    SET_CYCLES(1000000000); fa18_next_event=INT64_MAX; fa18_recomp_abort=0;
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *reference=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    void *cpu=malloc(m68k_context_size()); char error[256];
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,scenario;
    if(argc>2) selected_entry=(uint32_t)strtoul(argv[2],NULL,16);
    if(!state || !rom || !m || !base || !before || !reference || !cpu || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) {
        fprintf(stderr,"input-device-callbacks oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1;
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u source\n",scenario);
        input_device_callbacks_contract_reset(scenario,1);
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"input-device-callbacks oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        input_device_callbacks_contract_reset(scenario,0);
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u C\n",scenario);
        switch(selected_entry) {
        case 0xc1718eu: glue_C1718E(); break;
        case 0xc17456u: glue_C17456(); break;
        case 0xc1748cu: glue_C1748C(); break;
        case 0xc174a0u: glue_C174A0(); break;
        case 0xc16cd8u: glue_C16CD8(); break;
        case 0xc16b8cu: glue_C16B8C(); break;
        case 0xc17104u: glue_C17104(); break;
        case 0xc1712cu: glue_C1712C(); break;
        default: return 1;
        }
        input_device_callbacks_contract_finish();
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"input-device-callbacks oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"input-device-callbacks oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"input-device-callbacks oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("input-device-callbacks oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
