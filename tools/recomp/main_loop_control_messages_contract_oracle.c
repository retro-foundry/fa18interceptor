/* Complete main-loop-control-messages proof, including complete child-entry CPU/RAM contracts. */
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
static uint32_t selected_entry=0xc1518cu;
extern void main_loop_control_messages_contract_reset(unsigned scenario,int source);
extern void main_loop_control_messages_contract_enter(uint32_t entry,uint32_t ret);
extern void main_loop_control_messages_contract_finish(void);
static const uint32_t source_boundaries[]={0xC1518Cu,0xC15190u,0xC15192u,0xC15196u,0xC1519Au,0xC1519Eu,0xC151A4u,0xC151ACu,0xC151B4u,0xC151B8u,0xC151BAu,0xC151BCu,0xC151BEu,0xC151C2u,0xC151C6u,0xC151CAu,0xC151CEu,0xC151D0u,0xC151D2u,0xC151D4u,0xC151DAu,0xC151E0u,0xC151E6u,0xC151E8u,0xC151EEu,0xC151F0u,0xC151F4u,0xC151F6u,0xC151FCu,0xC15200u,0xC15206u,0xC1520Au,0xC15210u,0xC15214u,0xC15216u,0xC15218u,0xC1521Eu,0xC15222u,0xC15224u,0xC1522Au,0xC1522Eu,0xC15232u,0xC15234u,0xC15238u,0xC1523Cu,0xC15240u,0xC15244u,0xC15248u,0xC1524Au,0xC1524Eu,0xC15250u,0xC15254u,0xC1525Au,0xC1525Eu,0xC15262u,0xC15266u,0xC15268u,0xC1526Cu,0xC15270u,0xC15274u,0xC15278u,0xC1527Cu,0xC1527Eu,0xC15280u,0xC15284u,0xC15288u,0xC1528Eu,0xC15292u,0xC15294u,0xC1529Au,0xC1529Eu,0xC152A6u,0xC152AAu,0xC152AEu,0xC152B4u,0xC152B8u,0xC152BCu,0xC152C2u,0xC152C4u,0xC152CAu,0xC152CCu,0xC152CEu,0xC152D0u,0xC152D2u,0xC152D4u,0xC152D6u,0xC152DCu,0xC152DEu,0xC152E4u,0xC152E6u,0xC152E8u,0xC152ECu,0xC152EEu,0xC152F2u,0xC152F4u,0xC152F8u,0xC152FAu,0xC152FCu,0xC15302u,0xC15304u,0xC15308u,0xC1530Au,0xC1530Eu,0xC15310u,0xC15314u,0xC15316u,0xC15318u,0xC1531Eu,0xC15324u,0xC15326u,0xC1532Cu,0xC1532Eu,0xC15330u,0xC15338u,0xC1533Au,0xC15340u,0xC15342u,0xC15344u,0xC15346u,0xC15348u,0xC1534Au,0xC15350u,0xC15352u,0xC15354u,0xC15356u,0xC15358u,0xC1535Au,0xC1535Cu,0xC15362u,0xC15364u,0xC15366u,0xC15368u,0xC1536Au,0xC1536Cu,0xC1536Eu,0xC15374u,0xC15376u,0xC1537Cu,0xC1537Eu,0xC15384u,0xC15386u,0xC15388u,0xC1538Eu,0xC15396u,0xC1539Au,0xC1539Eu,0xC153A2u,0xC153A4u,0xC153A6u,0xC153AAu,0xC153ACu,0xC153AEu,0xC153B2u,0xC153B4u,0xC153B6u,0xC153B8u,0xC153BCu,0xC153C0u,0xC153C2u,0xC153C6u,0xC153C8u,0xC153CCu,0xC153CEu,0xC153D0u,0xC153D2u,0xC153D4u,0xC153D8u,0xC153DCu,0xC153E0u,0xC153E2u,0xC153E6u,0xC153E8u,0xC153ECu,0xC153EEu,0xC153F2u,0xC153F6u,0xC153F8u,0xC153FAu,0xC32BD2u,0xC32BD8u,0xC32BDEu,0xC32BE0u,0xC32BE4u,0xC32BE6u,0xC32BECu,0xC32BF0u,0xC32BF8u,0xC32C00u,0xC32C04u,0xC32C0Cu,0xC32C0Eu,0xC32C14u,0xC32C16u,0xC32C1Eu,0xC32C26u,0xC32C2Cu,0xC32C2Eu,0xC32C30u,0xC32C32u,0xC32C34u,0xC32C38u,0xC32C3Au,0xC32C3Cu,0xC32C3Eu,0xC32C40u,0xC32C42u,0xC32C48u,0xC32C4Eu,0xC32C50u,0xC32C52u,0xC32C5Au,0xC32C5Cu,0xC32C5Eu,0xC32C62u,0xC32C64u,0xC32C6Au,0xC32C72u,0xC32C78u,0xC32C7Cu,0xC32C82u,0xC32C88u,0xC32C8Au,0xC32C8Eu,0xC32C92u,0xC32C94u,0xC32C9Cu,0xC32C9Eu,0xC32CA4u,0xC32CAAu,0xC32CB2u,0xC32CB6u,0xC32CBCu,0xC32CC2u,0xC32CC4u,0xC32CCEu,0xC32CD4u,0xC32CD6u,0xC32CDCu,0xC32CDEu,0xC32CE6u,0xC32CECu,0xC32CEEu,0xC32CF4u,0xC32CF6u,0xC32CFCu,0xC32D02u,0xC32D04u,0xC32D08u,0xC32D0Au,0xC32D0Cu,0xC32D0Eu,0xC32D14u,0xC32D1Au,0xC32D1Eu,0xC32D24u,0xC32D2Au,0xC32D30u,0xC32D32u,0xC32D34u,0xC32D3Au,0xC32D40u,0xC32D44u,0xC32D46u,0xC32D48u,0xC32D4Cu,0xC32D50u,0xC32D52u,0xC32D54u,0xC32D56u,0xC32D58u,0xC32D5Au,0xC32D5Cu,0xC32D5Eu,0xC32D60u,0xC32D62u,0xC32D64u,0xC32D66u,0xC32D68u,0xC32D6Au,0xC32D6Cu,0xC32D6Eu,0xC32D74u,0xC32D7Au,0xC32D80u,0xC32D88u,0xC32D8Au,0xC32D90u,0xC32D92u,0xC32D98u,0xC32D9Eu,0xC32DA4u,0xC32DAAu,0xC32DB0u,0xC32DB2u,0xC32DB4u,0xC32DB6u,0xC32DBAu,0xC32DBCu,0xC32DC2u,0xC32DC4u,0xC32DCCu,0xC32DD4u,0xC32DDCu,0xC32DE2u,0xC32DE4u,0xC32DE6u,0xC32DEAu,0xC32DECu,0xC32DF2u,0xC32DF6u,0xC32DF8u,0xC32E00u,0xC32E04u,0xC32E08u,0xC32E0Eu,0xC32E16u,0xC32E1Au,0xC32E20u,0xC32E24u,0xC32E2Cu,0xC32E2Eu,0xC32E34u,0xC32E38u,0xC32E40u,0xC32E42u,0xC32E4Au,0xC32E4Eu,0xC32E54u,0xC32E58u,0xC32E5Cu,0xC32E62u,0xC32E66u,0xC32E6Cu,0xC32E72u,0xC32E74u,0xC32E78u,0xC32E7Cu,0xC32E80u,0xC32E82u,0xC32E88u,0xC32E8Au,0xC32E90u,0xC32E96u,0xC32E9Au,0xC32E9Cu,0xC32EA0u,0xC32EA2u,0xC32EA4u,0xC32EAAu,0xC32EAEu,0xC32EB6u,0xC32EB8u,0xC32EBEu,0xC32EC6u,0xC32ECAu,0xC32ED2u,0xC32ED8u,0xC32EE0u,0xC32EE4u,0xC32EEAu,0xC32EEEu,0xC32EF4u,0xC32EF6u,0xC32EFCu,0xC32EFEu,0xC32F04u,0xC32F0Cu,0xC32F10u,0xC32F12u,0xC32F18u,0xC32F1Au,0xC32F20u,0xC32F26u,0xC32F2Cu,0xC32F30u,0xC32F36u,0xC32F38u,0xC32F3Cu,0xC32F42u,0xC32F44u,0xC32F46u,0xC32F48u,0xC32F50u,0xC32F52u,0xC32F54u,0xC32F5Cu,0xC32F62u,0xC32F68u,0xC32F6Cu,0xC32F74u,0xC32F76u,0xC32F78u,0xC32F7Cu,0xC32F80u,0xC32F86u,0xC32F8Au,0xC32F8Eu,0xC32F92u,0xC32F94u,0xC32F9Au,0xC32F9Cu,0xC32FA4u,0xC32FA6u,0xC32FACu,0xC32FB0u,0xC32FB6u,0xC32FB8u,0xC32FBCu,0xC32FC2u,0xC32FC8u,0xC32FCAu,0xC32FCCu,0xC32FCEu,0xC32FD0u,0xC32FD4u,0xC32FD8u,0xC32FDAu,0xC32FDCu,0xC32FDEu,0xC32FE2u,0xC32FE8u,0xC32FECu,0xC32FF0u,0xC32FF2u,0xC32FF8u,0xC32FFAu,0xC32FFCu,0xC32FFEu,0xC33002u,0xC33006u,0xC3300Eu,0xC33010u,0xC33012u,0xC33014u,0xC3301Cu,0xC3301Eu,0xC33024u,0xC33028u,0xC3302Au,0xC3302Cu,0xC3302Eu,0xC33036u,0xC3303Au,0xC33040u,0xC33042u,0xC33048u,0xC3304Au,0xC3304Cu,0xC3304Eu,0xC33050u,0xC33052u,0xC33054u,0xC33058u,0xC3305Au,0xC3305Eu,0xC33060u,0xC33064u,0xC33066u,0xC3306Au,0xC3306Cu,0xC33070u,0xC33072u,0xC33074u,0xC33076u,0xC3307Au,0xC3307Eu,0xC33080u,0xC33084u,0xC33086u,0xC3308Au,0xC3308Cu,0xC33090u,0xC33092u,0xC33096u,0xC3309Au,0xC3309Cu,0xC330A0u,0xC330A2u,0xC330A6u,0xC330A8u,0xC330ACu,0xC330AEu,0xC330B2u,0xC330B6u,0xC330B8u,0xC330BCu,0xC330BEu,0xC330C2u,0xC330C4u,0xC330C8u,0xC330CAu,0xC330CEu,0xC330D6u,0xC330D8u,0xC330DAu,0xC330DCu,0xC330E4u,0xC330E8u,0xC330EEu,0xC330F0u,0xC330F2u,0xC330F4u,0xC330FCu};
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
                main_loop_control_messages_contract_enter(REG_PC,rd_u32(REG_A[7]));
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
    unsigned i,profile=scenario;
    for(i=0;i<15;++i) REG_DA[i]=random_value(); REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    wr_u32(0xc1ab74u,0xc60400u);
    for(i=0;i<10;++i) {
        gaddr a=0xc45c72u+128*i;
        wr_u16(a+38,(uint16_t)((profile&1u)?1:0)); wr_u16(a+40,(uint16_t)random_value()); wr_u16(a+46,(uint16_t)random_value());
    }
    wr_u8(0xc461bfu,bytes[(profile>>4u)%8u]); wr_u8(0xc46201u,(uint8_t)((profile&2u)?0x50:0));
    wr_u16(0xc461e4u,(uint16_t)((profile&4u)?4:0));
    wr_u8(0xc457bdu,(uint8_t)((profile>>3u)&1u)); wr_u8(0xc457aeu,(uint8_t)((profile>>4u)&1u));
    wr_u16(0xc458c6u,(uint16_t)((profile&32u)?8:0)); wr_u8(0xc4588bu,(uint8_t)((profile>>6u)%3u));
    wr_u8(0xc457adu,(uint8_t)((profile>>8u)&1u)); wr_u8(0xc45785u,(uint8_t)((profile>>9u)&1u));
    wr_u8(0xc45b5bu,(uint8_t)((profile&128u)?32:0));
    wr_u8(0xc45871u,(uint8_t)((profile&1024u)?1:0));
    wr_u8(0xc457c6u,(uint8_t)((profile&512u)?2:0));
    wr_u16(0xc4574au,(uint16_t)((profile&256u)?0:1)); wr_u16(0xc4574cu,(uint16_t)((profile&128u)?2:0));
    if(profile&2048u) wr_u16(0xc4574au,0x8001);
    if(profile&4096u) wr_u16(0xc4574au,0xc001);
    wr_u8(0xc457c3u,(uint8_t)((profile&1u)?1:0));
    wr_u8(0xc457e0u,(uint8_t)((profile>>1u)%4u));
    wr_u16(0xc45744u,(uint16_t)((profile&8u)?((profile&16u)?1:2):0));
    wr_u16(0xc45746u,(uint16_t)((profile&16u)?2:0)); wr_u16(0xc45748u,(uint16_t)((profile&32u)?2:0));
    wr_u8(0xc457dcu,(uint8_t)((profile>>6u)&3u)); wr_u8(0xc457dbu,(uint8_t)((profile%3u)+1));
    wr_u8(0xc457deu,(uint8_t)((profile&128u)?0:1)); wr_u8(0xc457dfu,bytes[(profile>>7u)%8u]);
    wr_u8(0xc457d7u,(uint8_t)((profile>>4u)%3u)); wr_u8(0xc457f5u,bytes[(profile>>5u)%8u]);
    wr_u8(0xc457f6u,(uint8_t)((profile%4u)+1)); wr_u8(0xc457f8u,(uint8_t)(profile%10u));
    for(i=0;i<10;++i) { static const uint8_t events[]={0,0x44,0x20,0x40,0x41}; wr_u8(0xc457e1u+i,events[(profile>>4u)%5u]); }
    for(i=0;i<10;++i) wr_u8(0xc457ebu+i,(uint8_t)random_value());
    wr_u32(0xc456feu,0xc60800u); wr_u32(0xc45702u,0xc60600u); wr_u32(0xc45706u,440);
    wr_u32(0xc4570au,0xc60820u); wr_u32(0xc4570eu,0xc60620u); wr_u32(0xc45712u,880);
    wr_u32(0xc456b6u,0xc60a00u); wr_u32(0xc4573eu,440); wr_u16(0xc45952u,(uint16_t)((profile>>3u)%16u));
    for(i=0;i<4;++i) { wr_u32(0xc60a00u+4*i,0x10000u+0x5000*i); wr_u16(0xc60800u+4*i,(uint16_t)(i*8)); wr_u16(0xc60802u+4*i,(uint16_t)random_value()); }
    wr_u16(0xc60820u,12); wr_u16(0xc60822u,0);
    for(i=0;i<8;++i) wr_u8(0xc60600u+i,0xff);
    { static const uint8_t text[]={0,0xff,8,0x41,0x20}; wr_u8(0xc60600u,text[(profile>>8u)%5u]); }
    wr_u8(0xc60602u,2); wr_u8(0xc60603u,(uint8_t)((profile&256u)?0x80:1)); wr_u8(0xc60604u,(uint8_t)(profile%256u));
    wr_u8(0xc60620u,0x41); wr_u8(0xc60621u,0xff);
    REG_D[4]=(REG_D[4]&0xffffff00u)|((profile&128u)?0x41u:0x80u);

    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; expected_sp=REG_A[7]+4; wr_u32(REG_A[7],0xc70000u);
    wr_u32(REG_A[7]+4,random_value());
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
        fprintf(stderr,"main-loop-control-messages oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1;
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u source\n",scenario);
        main_loop_control_messages_contract_reset(scenario,1);
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"main-loop-control-messages oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        main_loop_control_messages_contract_reset(scenario,0);
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u C\n",scenario);
        switch(selected_entry) {
        case 0xc1518cu: glue_C1518C(); break;
        case 0xc32ceeu: glue_C32CEE(); break;
        default: return 1;
        }
        main_loop_control_messages_contract_finish();
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"main-loop-control-messages oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"main-loop-control-messages oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"main-loop-control-messages oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("main-loop-control-messages oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
