/* Complete record-control-actions proof, including cold internal paths and real children. */
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
extern void fa18_record_control_actions_fixture_begin(const char *phase);
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
static uint32_t selected_entry=0xc153fcu;
static const uint32_t source_boundaries[]={0xC15138u,0xC1513Cu,0xC15140u,0xC15144u,0xC15148u,0xC1514Cu,0xC1514Eu,0xC15150u,0xC15154u,0xC1515Au,0xC1515Cu,0xC15160u,0xC15162u,0xC15166u,0xC15168u,0xC1516Eu,0xC15170u,0xC15174u,0xC15176u,0xC1517Au,0xC1517Eu,0xC15182u,0xC15186u,0xC15188u,0xC1518Au,0xC153FCu,0xC15400u,0xC15404u,0xC1540Au,0xC1540Eu,0xC15414u,0xC15418u,0xC1541Cu,0xC1541Eu,0xC15422u,0xC15426u,0xC15428u,0xC1542Eu,0xC15430u,0xC15432u,0xC15436u,0xC1543Au,0xC1543Eu,0xC15442u,0xC15446u,0xC15448u,0xC1544Au,0xC15450u,0xC15452u,0xC15454u,0xC15456u,0xC1545Au,0xC1545Cu,0xC15460u,0xC15464u,0xC15466u,0xC1546Cu,0xC15472u,0xC15476u,0xC1547Au,0xC1547Cu,0xC1547Eu,0xC15482u,0xC15484u,0xC15488u,0xC1548Cu,0xC1548Eu,0xC15494u,0xC1549Au,0xC154A0u,0xC154A4u,0xC154A6u,0xC154ACu,0xC154AEu,0xC154B2u,0xC154B6u,0xC154B8u,0xC154BAu,0xC154C0u,0xC154C2u,0xC154C4u,0xC154C8u,0xC154CCu,0xC154CEu,0xC154D2u,0xC154D4u,0xC154D8u,0xC154DCu,0xC154DEu,0xC154E0u,0xC154E6u,0xC154EAu,0xC154ECu,0xC154F2u,0xC154F6u,0xC154F8u,0xC154FAu,0xC15500u,0xC15504u,0xC1550Au,0xC1550Cu,0xC15510u,0xC15516u,0xC15518u,0xC1551Eu,0xC15522u,0xC15524u,0xC1552Au,0xC1552Eu,0xC15532u,0xC15534u,0xC15538u,0xC1553Cu,0xC15542u,0xC15546u,0xC15548u,0xC1554Au,0xC15550u,0xC15554u,0xC15556u,0xC1555Cu,0xC15560u,0xC15562u,0xC15564u,0xC1556Au,0xC1556Eu,0xC15574u,0xC15576u,0xC1557Au,0xC15580u,0xC15582u,0xC15588u,0xC1558Cu,0xC1558Eu,0xC15594u,0xC15598u,0xC1559Cu,0xC1559Eu,0xC155A2u,0xC155A6u,0xC155ACu,0xC155AEu,0xC155B2u,0xC155B4u,0xC155B8u,0xC155BAu,0xC155C0u,0xC155C2u,0xC155C6u,0xC155C8u,0xC155CEu,0xC155D0u,0xC155D4u,0xC155D6u,0xC155DCu,0xC155DEu,0xC155E2u,0xC155E4u,0xC155E8u,0xC155EAu,0xC155EEu,0xC155F0u,0xC155F2u,0xC155F6u,0xC155F8u,0xC155FAu,0xC15600u,0xC15602u,0xC15604u,0xC15608u,0xC1560Au,0xC1560Eu,0xC15610u,0xC15614u,0xC15616u,0xC15618u,0xC1561Eu,0xC15620u,0xC15622u,0xC15626u,0xC15628u,0xC1562Au,0xC15630u,0xC15632u,0xC1563Au,0xC1563Cu,0xC15640u,0xC15642u,0xC15646u,0xC15648u,0xC1564Cu,0xC1564Eu,0xC15650u,0xC15656u,0xC15658u,0xC1565Au,0xC1565Cu,0xC1565Eu,0xC15664u,0xC15668u,0xC1566Au,0xC1566Cu,0xC15672u,0xC15674u,0xC15676u,0xC15678u,0xC1567Au,0xC15680u,0xC15684u,0xC15686u,0xC15688u,0xC1568Cu,0xC15690u,0xC15696u,0xC1569Au,0xC1569Cu,0xC156A0u,0xC156A4u,0xC156A6u,0xC156A8u,0xC156AEu,0xC156B4u,0xC156BAu,0xC156BEu,0xC156C2u,0xC156C4u,0xC156C8u,0xC156CAu,0xC156CEu,0xC156D0u,0xC156D4u,0xC156DAu,0xC156E0u,0xC156E6u,0xC156ECu,0xC156EEu,0xC156F4u,0xC156F8u,0xC156FAu,0xC156FCu,0xC15702u,0xC15706u,0xC1570Au,0xC1570Cu,0xC15712u,0xC15718u,0xC1571Eu,0xC15724u,0xC1572Au,0xC15730u,0xC15734u,0xC15736u,0xC1573Cu,0xC15740u,0xC15742u,0xC15744u,0xC1574Au,0xC1574Eu,0xC15750u,0xC15752u,0xC15754u,0xC1575Au,0xC15760u,0xC15762u,0xC15768u,0xC1576Au,0xC1576Cu,0xC15772u,0xC15774u,0xC15776u,0xC15778u,0xC1577Eu,0xC15784u,0xC15786u,0xC1578Cu,0xC1578Eu,0xC15790u,0xC15796u,0xC15798u,0xC1579Au,0xC1579Cu,0xC157A2u,0xC157A8u,0xC157B0u,0xC157B6u,0xC157BEu,0xC157C4u,0xC157CAu,0xC157D0u,0xC157D2u,0xC157D8u,0xC157DAu,0xC157E0u,0xC157E6u,0xC157ECu,0xC157F0u,0xC157F6u,0xC157FCu,0xC15802u,0xC15804u,0xC1580Au,0xC1580Eu,0xC15812u,0xC15814u,0xC15818u,0xC1581Au,0xC1581Cu,0xC15822u,0xC15826u,0xC15828u,0xC1582Eu,0xC15834u,0xC1583Au,0xC1583Cu,0xC15842u,0xC15848u,0xC1584Au,0xC15850u,0xC15852u,0xC15854u,0xC15856u,0xC1585Au,0xC1585Cu,0xC15860u,0xC15862u,0xC15866u,0xC1586Cu,0xC15870u,0xC15872u,0xC15878u,0xC1587Cu,0xC1587Eu,0xC15880u,0xC15882u,0xC15888u,0xC1588Eu,0xC15890u,0xC15896u,0xC15898u,0xC1589Au,0xC1589Cu,0xC158A2u,0xC158A8u,0xC158AAu,0xC158B0u,0xC158B2u,0xC158B4u,0xC158B6u,0xC158BCu,0xC158C2u,0xC158C6u,0xC158C8u,0xC158CAu,0xC158CEu,0xC158D0u,0xC158D4u,0xC158D6u,0xC159AEu,0xC159B2u,0xC159B6u,0xC159BCu,0xC159BEu,0xC159C4u,0xC159CAu,0xC159CCu,0xC159D2u,0xC159D4u,0xC159D6u,0xC159D8u,0xC159DCu,0xC159DEu,0xC159E4u,0xC159E6u,0xC159EAu,0xC159ECu,0xC159F2u,0xC159F8u,0xC159FAu,0xC159FCu,0xC159FEu,0xC15A00u,0xC15A06u,0xC15A0Au,0xC15A0Cu,0xC15A12u,0xC15A16u,0xC15A1Au,0xC15A1Eu,0xC15A20u,0xC15A26u,0xC15A28u,0xC15A2Eu,0xC15A32u,0xC15A36u,0xC15A38u,0xC15A3Eu,0xC15A40u,0xC15A44u,0xC15A48u,0xC15A4Cu,0xC15A50u,0xC15A56u,0xC15A5Cu,0xC15A62u,0xC15A64u,0xC15A6Au,0xC15A6Cu,0xC15A72u,0xC15A78u,0xC15A7Cu,0xC15A82u,0xC15A88u,0xC15A8Eu,0xC15A90u,0xC15A94u,0xC15A98u,0xC15A9Au,0xC15A9Eu,0xC15AA0u,0xC15AA4u,0xC15AA6u,0xC15AAAu,0xC15AACu,0xC15AB0u,0xC15AB2u,0xC15AB6u,0xC15AB8u,0xC15ABCu,0xC15ABEu,0xC15AC0u,0xC15AC2u,0xC15AC4u,0xC15AC6u,0xC15AC8u,0xC15ACCu,0xC15AD0u,0xC15AD2u,0xC15AD4u,0xC15AD8u,0xC15ADCu,0xC15AE0u,0xC15AE2u,0xC15AE6u,0xC15AE8u,0xC15AECu,0xC15AEEu,0xC15AF2u,0xC15AF4u,0xC15AF6u,0xC15AF8u,0xC15AFAu,0xC15AFCu,0xC15B02u,0xC15B06u,0xC15B0Cu,0xC15B0Eu,0xC15B12u,0xC15B14u,0xC15B1Au,0xC15B1Eu,0xC15B24u,0xC15B26u,0xC15B2Au,0xC15B2Cu,0xC15B32u,0xC15B36u,0xC15B3Cu,0xC15B3Eu,0xC15B42u,0xC15B44u,0xC15B4Au,0xC15B4Eu,0xC15B52u,0xC15B58u,0xC15B5Cu,0xC15B60u,0xC15B66u,0xC15B6Au,0xC15B6Eu,0xC15B74u,0xC15B78u,0xC15B7Cu,0xC15B82u,0xC15B86u,0xC15B8Au,0xC15B8Cu,0xC15B8Eu,0xC15B94u,0xC15B98u,0xC15B9Eu,0xC15BA4u,0xC15BA8u,0xC15BAEu,0xC15BB4u,0xC15BB8u,0xC15BBEu,0xC15BC0u,0xC15BC6u,0xC15BCAu,0xC15BD0u,0xC15BD2u,0xC15BD8u,0xC15BDCu,0xC15BE2u,0xC15BE4u,0xC15BEAu,0xC15BEEu,0xC15BF2u,0xC15BF4u,0xC181A0u,0xC181A4u,0xC181AAu,0xC181ACu,0xC181AEu,0xC181B0u,0xC181B4u,0xC181B6u,0xC181BCu,0xC181BEu,0xC181C2u,0xC181CAu,0xC181CCu,0xC181D0u,0xC181D2u,0xC181D8u,0xC181DAu,0xC181DEu,0xC181E0u,0xC181E4u,0xC181E6u,0xC181E8u,0xC181EAu,0xC181ECu,0xC181EEu,0xC181F2u,0xC181F6u,0xC181F8u};
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
    static const uint16_t flags[]={0,2,0x1000,0x2000,0x100,0x102,0x1002,0x3000};
    static const uint32_t positions[]={0,1,0xffffffffu,0x400000,0x7fffffff,0x80000000,0x3fffff,0x800001};
    static const uint16_t cells[]={0,1,127,128,0x8000,0xffff};
    static const uint16_t sums[]={0,1,2,3,4,5,0xffff,0xfffe,0xfffd,0xfffc,0xfffb,0x8000,0x7fff};
    unsigned i,profile=scenario;
    for(i=0;i<15;++i) REG_DA[i]=random_value(); REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    for(i=0;i<64;++i) wr_u32(0xc60400u+4*i,random_value());
    wr_u32(0xc18214u,0xc60400u);
    wr_u16(0xc60426u,flags[profile%8u]); wr_u16(0xc60428u,(uint16_t)((profile&8u)?2:((profile&128u)?0xffff:0)));
    wr_u32(0xc60400u,positions[(profile>>5u)%8u]); wr_u32(0xc60404u,(profile&16u)?0xffffffffu:1); wr_u32(0xc60408u,positions[(profile>>8u)%8u]);
    wr_u16(0xc60430u,cells[(profile>>2u)%6u]); wr_u16(0xc60432u,cells[(profile>>4u)%6u]);
    wr_u32(0xc60410u,profile&256u?256:0); wr_u8(0xc4588cu,(uint8_t)(profile%4u)); wr_u8(0xc4588du,(uint8_t)((profile>>2u)%4u));
    wr_u8(0xc461e6u,(uint8_t)((profile&16u)?17:0)); wr_u16(0xc458dau,(uint16_t)((profile>>3u)%4u)); wr_u16(0xc459b8u,(uint16_t)(profile%2u));
    for(i=0;i<9;++i) wr_u16(0xc46216u+2*i,(uint16_t)random_value());
    for(i=0;i<3;++i) { wr_u32(0xc46198u+4*i,random_value()); wr_u32(0xc45a52u+4*i,random_value()); wr_u32(0xc461c2u+4*i,random_value()); wr_u16(0xc45a4cu+2*i,(uint16_t)random_value()); }
    wr_u16(0xc461f0u,(uint16_t)random_value()); wr_u16(0xc463f0u,(uint16_t)random_value());
    wr_u32(0xc0a460u,(profile&1u)?0xc60c00u:0);
    wr_u32(0xc0a460u,0); if(selected_entry==0xc153fcu) wr_u16(0xc60428u,0);
    if(selected_entry==0xc15688u || selected_entry==0xc159aeu) {
        wr_u16(0xc60426u,0x1000); wr_u16(0xc459b8u,0);
        wr_u32(0xc46198u,256); wr_u32(0xc4619cu,512); wr_u32(0xc461a0u,768);
        wr_u32(0xc45a52u,1024); wr_u32(0xc45a56u,2048); wr_u32(0xc45a5au,4096);
        wr_u32(0xc60400u,0); wr_u32(0xc60404u,0); wr_u32(0xc60408u,0);
    }
    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; expected_sp=REG_A[7]+4; wr_u32(REG_A[7],0xc70000u);
    for(i=0;i<4;++i) wr_u32(REG_A[7]+4+4*i,random_value());
    if(selected_entry==0xc15138u) {
        uint16_t y=(uint16_t)random_value(),sum=sums[profile%13u];
        wr_u16(REG_A[7]+6,(uint16_t)(sum-y)); wr_u16(REG_A[7]+10,y);
    }
    if(1 && selected_entry==0xc15ad4u) { wr_u32(REG_A[7]+4,13); wr_u32(REG_A[7]+8,10); wr_u32(REG_A[7]+12,20); wr_u32(REG_A[7]+16,30); }
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
        fprintf(stderr,"record-control-actions oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=2;
        fa18_record_control_actions_fixture_begin("original");
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"record-control-actions oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        fa18_record_control_actions_fixture_begin("C");
        switch(selected_entry) {
        case 0xc153fcu: glue_C153FC(); break;
        case 0xc15688u: glue_C15688(); break;
        case 0xc159aeu: glue_C159AE(); break;
        case 0xc15ad4u: glue_C15AD4(); break;
        case 0xc181a0u: glue_C181A0(); break;
        case 0xc15138u: glue_C15138(); break;
        default: return 1;
        }
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"record-control-actions oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"record-control-actions oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"record-control-actions oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("record-control-actions oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
