/* Complete postflight-messages proof, including cold internal paths and real children. */
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

extern int fa18_write_log_active,fa18_write_log_hardware;
extern int fa18_structural_port_classified(uint32_t entry,int hardware);
extern void fa18_structural_reset_write_log(void);
extern int fa18_structural_port_matched(uint32_t entry);
extern int fa18_structural_port_unused(uint32_t entry);
extern int fa18_ports_enter(int function,int label,int via_call);
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
static uint32_t selected_entry=0xc110a4u;
static const uint32_t source_boundaries[]={0xC0F4D8u,0xC0F4DCu,0xC0F4E2u,0xC0F4E8u,0xC0F4EEu,0xC0F4F4u,0xC0F4F6u,0xC0F4F8u,0xC0F4FAu,0xC0F4FCu,0xC0F500u,0xC0F506u,0xC0F508u,0xC0F50Eu,0xC0F514u,0xC0F51Au,0xC0F520u,0xC0F522u,0xC0F524u,0xC0F52Au,0xC0F52Cu,0xC0F52Eu,0xC0F534u,0xC0F538u,0xC0F53Au,0xC0F53Cu,0xC0F542u,0xC0F548u,0xC0F54Au,0xC0F550u,0xC0F556u,0xC0F55Cu,0xC0F560u,0xC0F566u,0xC0F568u,0xC0F812u,0xC0F816u,0xC0F81Cu,0xC0F824u,0xC0F828u,0xC0F82Cu,0xC0F830u,0xC0F832u,0xC0F834u,0xC0F836u,0xC0F838u,0xC0F83Eu,0xC0F842u,0xC0F844u,0xC0F848u,0xC0F84Cu,0xC0F84Eu,0xC0F854u,0xC0F856u,0xC0F85Cu,0xC0F862u,0xC0F86Au,0xC0F874u,0xC0F87Au,0xC0F87Eu,0xC0F882u,0xC0F886u,0xC0F888u,0xC0F88Eu,0xC0F892u,0xC0F898u,0xC0F89Au,0xC0F89Eu,0xC0F8A2u,0xC0F8A4u,0xC0F8AAu,0xC0F8B0u,0xC0F8B2u,0xC0F8B6u,0xC0F8B8u,0xC0F8BCu,0xC0F8BEu,0xC0F8C2u,0xC0F8C4u,0xC0F8CAu,0xC0F8D0u,0xC0F8D2u,0xC0F8D6u,0xC0F8D8u,0xC0F8DCu,0xC0F8DEu,0xC0F8E2u,0xC0F8E6u,0xC0F8E8u,0xC0F8ECu,0xC0F8EEu,0xC0F8F0u,0xC0F8F2u,0xC0F8F6u,0xC0F8F8u,0xC0F8FCu,0xC0F900u,0xC0F904u,0xC0F90Eu,0xC0F910u,0xC0F912u,0xC0F914u,0xC0F91Au,0xC0F91Cu,0xC0F91Eu,0xC11078u,0xC1107Eu,0xC11080u,0xC11082u,0xC11084u,0xC1108Au,0xC11092u,0xC11096u,0xC1109Cu,0xC110A2u,0xC110A4u,0xC110A8u,0xC110AEu,0xC110B0u,0xC110B4u,0xC110BAu,0xC110BCu,0xC110C4u,0xC110CAu,0xC110CEu,0xC110D4u,0xC110D8u,0xC110DCu,0xC110E2u,0xC110E6u,0xC110EAu,0xC110EEu,0xC110F2u,0xC110F4u,0xC110F8u,0xC110FAu,0xC110FCu,0xC11100u,0xC11102u,0xC11104u,0xC11106u,0xC11108u,0xC1110Eu,0xC11112u,0xC11116u,0xC11118u,0xC1111Cu,0xC11122u,0xC11124u,0xC11128u,0xC1112Cu,0xC11130u,0xC11134u,0xC11138u,0xC1113Cu,0xC11140u,0xC11146u,0xC11148u,0xC1114Au,0xC11150u,0xC11152u,0xC11158u,0xC1115Au,0xC1115Eu,0xC11162u,0xC11166u,0xC1116Eu,0xC11172u,0xC11178u,0xC1117Cu,0xC11182u,0xC11186u,0xC1118Au,0xC1118Cu,0xC11190u,0xC11192u,0xC1119Au,0xC111A2u,0xC111A6u,0xC111AAu,0xC111AEu,0xC111B2u,0xC111B6u,0xC111BAu,0xC111C0u,0xC111C4u,0xC111C8u,0xC111CEu,0xC111D0u,0xC111D2u,0xC111D8u,0xC111DCu,0xC111E2u,0xC111E6u,0xC111E8u,0xC111ECu,0xC111EEu,0xC111F0u,0xC111F2u,0xC111F4u,0xC111FAu,0xC111FEu,0xC11200u,0xC11204u,0xC11208u,0xC1120Cu,0xC1120Eu,0xC11212u,0xC11216u,0xC1121Au,0xC1121Cu,0xC11220u,0xC11224u,0xC11228u,0xC11230u,0xC11232u,0xC11236u,0xC1123Au,0xC1123Eu,0xC11240u,0xC11244u,0xC11248u,0xC1124Au,0xC11250u,0xC11254u,0xC11258u,0xC1125Au,0xC1125Eu,0xC11262u,0xC1126Au,0xC1126Cu,0xC11270u,0xC11274u,0xC11278u,0xC1127Au,0xC1127Eu,0xC11282u,0xC11286u,0xC1128Cu,0xC11290u,0xC11292u,0xC11296u,0xC11298u,0xC1129Cu,0xC1129Eu,0xC112A2u,0xC112A4u,0xC112A8u,0xC112AAu,0xC112B0u,0xC112B4u,0xC112B6u,0xC112BCu,0xC112BEu,0xC112C0u,0xC112C6u,0xC112C8u,0xC112CCu,0xC112D0u,0xC112D4u,0xC112DCu,0xC112E2u,0xC112E6u,0xC112E8u,0xC112ECu,0xC112F0u,0xC112F2u,0xC112F6u,0xC112F8u,0xC112FCu,0xC11302u,0xC11308u,0xC1130Cu,0xC1130Eu,0xC11310u,0xC11350u,0xC11354u,0xC1135Au,0xC1135Cu,0xC1135Eu,0xC11364u,0xC11368u,0xC1136Au,0xC1136Eu,0xC11374u,0xC1137Au,0xC11382u,0xC11386u,0xC1138Cu,0xC11390u,0xC11396u,0xC1139Au,0xC1139Cu,0xC113A2u,0xC113A6u,0xC113ACu,0xC113AEu,0xC113B4u,0xC113B8u,0xC113BAu,0xC113C2u,0xC113C6u,0xC113C8u,0xC113CAu,0xC113CCu,0xC113D0u,0xC113D2u,0xC113D6u,0xC113E0u,0xC113E2u,0xC113E4u,0xC113EAu,0xC113ECu,0xC113EEu,0xC113F4u,0xC113F8u,0xC113FAu,0xC11402u,0xC1140Au,0xC1140Eu,0xC11414u,0xC11416u,0xC1141Cu,0xC1141Eu,0xC11424u,0xC11426u,0xC11428u,0xC1142Eu,0xC11430u,0xC11432u,0xC1143Au,0xC1143Eu,0xC11444u,0xC11446u,0xC1144Cu,0xC1144Eu,0xC11450u,0xC11456u,0xC1145Au,0xC1145Cu,0xC11464u,0xC11468u,0xC1146Eu,0xC11470u,0xC11476u,0xC11478u,0xC1147Eu,0xC11480u,0xC1148Au,0xC11492u,0xC11498u,0xC1149Cu,0xC1149Eu,0xC114A4u,0xC114AAu,0xC114B2u,0xC114BAu,0xC114C0u,0xC114C6u,0xC114CAu,0xC114D0u,0xC114D2u,0xC114D6u,0xC114DCu,0xC114E4u,0xC114EAu,0xC114ECu,0xC114EEu,0xC114F4u,0xC114FCu,0xC11500u,0xC11506u,0xC1150Au,0xC11510u,0xC11512u,0xC11514u,0xC1151Au,0xC1151Eu,0xC11524u,0xC11526u,0xC1152Cu,0xC11530u,0xC11532u,0xC11538u,0xC1153Eu,0xC11542u,0xC11544u,0xC11546u,0xC1154Au,0xC1154Eu,0xC11550u,0xC11556u,0xC11558u,0xC1155Au,0xC1155Eu,0xC11562u,0xC11566u,0xC11568u,0xC1156Eu,0xC11570u,0xC11572u,0xC11576u,0xC1157Au,0xC1157Eu,0xC11582u,0xC11586u,0xC1158Au,0xC1158Eu,0xC11590u,0xC11594u,0xC1159Au,0xC1159Cu,0xC1159Eu,0xC115A4u,0xC115A6u,0xC115A8u,0xC115AEu,0xC115B2u,0xC115B8u,0xC115BAu,0xC115BEu,0xC115C4u,0xC115C6u,0xC115CAu,0xC115D0u,0xC115D2u,0xC115D6u,0xC115DEu,0xC115E4u,0xC115E8u,0xC115EAu,0xC115ECu,0xC115F2u,0xC115F8u,0xC115FEu,0xC11602u,0xC11606u,0xC11608u,0xC11610u,0xC11614u,0xC1161Au,0xC1161Cu,0xC11624u,0xC1162Au,0xC1162Eu,0xC11632u,0xC11638u,0xC1163Cu,0xC11640u,0xC11644u,0xC11648u,0xC1164Eu,0xC11650u,0xC11654u,0xC11656u,0xC11658u,0xC1165Au,0xC1165Eu,0xC11662u,0xC11664u,0xC11668u,0xC1166Cu,0xC11670u,0xC11672u,0xC11676u,0xC1167Cu,0xC1167Eu,0xC11684u,0xC11686u,0xC1168Cu,0xC11696u,0xC11698u,0xC1169Au,0xC116A0u,0xC116A2u,0xC116A4u,0xC116A8u,0xC116AEu,0xC116B0u,0xC116B6u,0xC116B8u,0xC116BAu,0xC116C0u,0xC116C2u,0xC116C6u,0xC116CCu,0xC116CEu,0xC116D4u,0xC116DCu,0xC116E2u,0xC116E6u,0xC116EAu,0xC116ECu,0xC116F4u,0xC116FAu,0xC11702u,0xC1170Au,0xC1170Eu,0xC11714u,0xC11716u,0xC11720u,0xC11726u,0xC1172Au,0xC1172Cu,0xC11732u,0xC11736u,0xC11738u,0xC1173Eu,0xC11740u,0xC11742u,0xC11748u,0xC11750u,0xC1175Au};
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
    static const uint8_t modes[]={0,1,2,3,4,5,6,7,8,9,125,0x80,0xff};
    static const uint8_t phases[]={0xff,0xfe,0xfd,0xfc,0xf0,0xef,0,1};
    static const uint8_t bytes[]={0,1,2,3,0x7f,0x80,0xff,0xfe};
    static const uint16_t words[]={0,1,2,3,0x7fff,0x8000,0xffff,4};
    unsigned i,profile=scenario;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    wr_u32(MODE_TABLE,0xc60400u); wr_u32(LONG_TABLE,0xc60600u);
    for(i=0;i<128;++i) wr_u8(0xc60380u+i,(uint8_t)random_value());
    for(i=0;i<128;++i) wr_u8(0xc60400u+i,(uint8_t)random_value());
    wr_u16(0xc60404u,words[(profile/8u)%8u]);
    wr_u16(0xc60438u,words[(profile/3u)%8u]);
    for(i=0;i<24;++i) wr_u8(0xc6041eu+i,1+(uint8_t)(random_value()%254u));
    if(profile%4u) wr_u8(0xc6041eu+(profile%24u),0);
    for(i=0;i<64;++i) wr_u16(0xc60600u+2*i,(uint16_t)random_value());
    wr_u16(POST_INPUT_COUNTDOWN,(profile%9u)?0xffffu:0);
    wr_u8(MODE_SELECT,modes[profile%13u]);
    wr_u8(PLAYER_PHASE,phases[(profile/13u)%8u]);
    wr_u16(0xc458dau,(uint16_t)((profile/104u)&1u));
    wr_u8(MESSAGE_STATE_C,bytes[(profile/3u)%8u]);
    wr_u8(KEY_TAKEN,(uint8_t)(profile&1u));
    wr_u8(COMMAND_RETURN_STATE,(uint8_t)(profile&1u));
    wr_u8(SCENE_DISPATCH_LIMIT,bytes[(profile/5u)%8u]);
    wr_u16(MENU_TABLE_STATUS,words[(profile/2u)%8u]);
    /* This is the original file-service gate, separate from MODE_TABLE[0]. */
    if(selected_entry==0xc110a4u) wr_u16(MENU_TABLE_STATUS,1);
    wr_u16(0xc4564cu,(profile&1u)?0xc560u:0x1234u);
    wr_u16(0xc45650u,(profile&2u)?0x7e70u:0x5678u);
    wr_u16(0xc45654u,(profile&4u)?0x4de8u:0x9abcu);
    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; expected_sp=REG_A[7]+4; wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31u)); REG_PC=selected_entry;
    SET_CYCLES(1000000000); fa18_next_event=INT64_MAX; fa18_recomp_abort=0;
}
int main(int argc,char **argv) {
    FA18PortMode tested_mode=argc>3?(FA18PortMode)strtoul(argv[3],NULL,10):FA18_PORTS_ON;
    char selection[16];
    if(tested_mode<FA18_PORTS_ON || tested_mode>FA18_PORTS_SANDBOX) return 1;
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *reference=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    void *cpu=malloc(m68k_context_size()); char error[256];
    unsigned hardware_cases=0,matched_cases=0;
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,scenario;
    if(argc>2) selected_entry=(uint32_t)strtoul(argv[2],NULL,16);
    if(!state || !rom || !m || !base || !before || !reference || !cpu || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) {
        fprintf(stderr,"postflight-messages dispatch oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    snprintf(selection,sizeof selection,"%06X",selected_entry);
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i; int saved_cycles,source_hardware;
        fa18_ports_init(FA18_PORTS_OFF,NULL);
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        wr_u16(0xc70010u,0x4e71u);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); saved_cycles=GET_CYCLES(); fa18_write_log_active=2;
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"postflight-messages dispatch oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"source case %u D0=%08X sample=%08X request=%08X cycle=%lld credit=%d\n",scenario,REG_D[0],rd_u32(READOUT_SAMPLE),rd_u32(MENU_TIME_REQUEST+32),(long long)m->cycle,GET_CYCLES());
        source_hardware=fa18_write_log_hardware!=0; hardware_cases+=source_hardware; matched_cases+=!source_hardware;
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu); SET_CYCLES(saved_cycles);
        fa18_write_log_active=0;
        fa18_ports_init(tested_mode,selection);
        {
            uint32_t previous=REG_PPC;
            int result;
            wr_u16(0xc70010u,0x4e71u); REG_PPC=0xc70010u;
            if(fa18_ports_enter_source_only(0,&result)) { fputs("non-call source entry was accepted\n",stderr); return 1; }
            REG_PPC=previous;
            result=fa18_recomp_call_dynamic();
            if(tested_mode==FA18_PORTS_ON && (result!=FA18_EXIT_DISPATCH || fa18_ports_active_steps()!=1)) {
                fputs("postflight-messages ON entry did not start its native continuation\n",stderr); return 1;
            }
            if(result==FA18_EXIT_DISPATCH) result=fa18_recomp_resume(0xc70000u,expected_sp);
            if(result!=FA18_RET || fa18_ports_active_steps()) {
                fprintf(stderr,"postflight-messages dispatch case %u mode %u did not complete at %06X\n",scenario,tested_mode,REG_PC); return 1;
            }
            if(tested_mode!=FA18_PORTS_ON && !fa18_structural_port_classified(selected_entry,source_hardware)) {
                fprintf(stderr,"postflight-messages comparison case %u mode %u did not match\n",scenario,tested_mode);
                fa18_ports_report("build/recomp/postflight_messages_dispatch_failed_report.json"); return 1;
            }
        }
        fa18_write_log_active=0;
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"C case %u D0=%08X sample=%08X request=%08X cycle=%lld credit=%d\n",scenario,REG_D[0],rd_u32(READOUT_SAMPLE),rd_u32(MENU_TIME_REQUEST+32),(long long)m->cycle,GET_CYCLES());
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"postflight-messages dispatch oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"postflight-messages dispatch oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"postflight-messages dispatch oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    {
        int result;
        unsigned guard;
        for(guard=0;guard<3;++guard) {
            int function=-1,i;
            memcpy(m,base,sizeof *m); fixture(0);
            wr_u16(0xc70010u,0x4e71u); REG_PPC=0xc70010u;
            fa18_ports_init(guard==0?FA18_PORTS_OFF:tested_mode,guard==1?"FFFFFE":selection);
            for(i=0;i<fa18_recomp_function_count;++i)
                if(fa18_recomp_functions[i].entry==selected_entry) function=i;
            if(function>=0) {
                result=guard==2?fa18_ports_enter(function,0,0):fa18_recomp_call_dynamic();
                /* The unported OS child can leave its generated caller for
                 * runtime completion. It must never start a native step here. */
                if(result==FA18_EXIT_DISPATCH && !fa18_ports_active_steps())
                    result=fa18_recomp_resume(0xc70000u,expected_sp);
                if(result!=FA18_RET || fa18_ports_active_steps()) {
                    fputs("guarded translated entry started a native continuation\n",stderr); return 1;
                }
            } else if(fa18_ports_enter_source_only(guard!=2,&result)) {
                fputs("guarded source entry was accepted\n",stderr); return 1;
            }
            if(!fa18_structural_port_unused(selected_entry)) {
                fputs("guarded entry counted a port call\n",stderr); return 1;
            }
        }
        fa18_ports_init(tested_mode,selection);
        REG_PC=selected_entry;
        {
            uint32_t sp=REG_A[7];
            wr_u16(selected_entry,rd_u16(selected_entry));
            result=fa18_recomp_call_dynamic();
            if(result!=FA18_EXIT_DISPATCH || REG_PC!=selected_entry || REG_A[7]!=sp || fa18_ports_active_steps()) {
                fputs("changed source still dispatched\n",stderr); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("postflight-messages dispatch oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("classification: %u hardware-bearing source calls, %u hardware-free source calls; reference modes require exact hardware or matched classification respectively\n",hardware_cases,matched_cases);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
