/* Complete flight-record-actions proof, including complete child-entry CPU/RAM contracts. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
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
static uint32_t selected_entry=0xc230e8u;
extern void flight_record_actions_contract_reset(unsigned scenario,int source);
extern void flight_record_actions_contract_enter(uint32_t entry,uint32_t ret);
extern void flight_record_actions_contract_finish(void);
extern int fa18_test_flight_record_append(void);
extern int fa18_test_flight_record_motion(void);
static int fault_profile;
static jmp_buf fault_stop;
void flight_record_actions_fault_boundary_observed(void) { longjmp(fault_stop,1); }
static const uint32_t source_boundaries[]={0xC230E8u,0xC230EAu,0xC230F0u,0xC230F2u,0xC230F8u,0xC230FCu,0xC230FEu,0xC23102u,0xC23106u,0xC23108u,0xC2310Cu,0xC23110u,0xC23114u,0xC23116u,0xC23118u,0xC2311Eu,0xC23120u,0xC23126u,0xC2312Au,0xC2312Eu,0xC23132u,0xC23134u,0xC2313Au,0xC2313Cu,0xC23140u,0xC23142u,0xC23146u,0xC23148u,0xC23150u,0xC23158u,0xC23160u,0xC23162u,0xC23164u,0xC23168u,0xC2316Au,0xC2316Eu,0xC23170u,0xC23172u,0xC23186u,0xC23188u,0xC2318Cu,0xC23190u,0xC23194u,0xC2319Au,0xC2319Eu,0xC231A0u,0xC23228u,0xC2322Cu,0xC2322Eu,0xC23230u,0xC23232u,0xC23238u,0xC2323Au,0xC2323Cu,0xC2323Eu,0xC23242u,0xC23244u,0xC2324Au,0xC2324Cu,0xC23254u,0xC23256u,0xC2325Cu,0xC2325Eu,0xC23264u,0xC2326Cu,0xC23270u,0xC23276u,0xC2327Au,0xC23280u,0xC23288u,0xC2328Au,0xC2328Cu,0xC23290u,0xC23292u,0xC23296u,0xC2329Cu,0xC2329Eu,0xC232A2u,0xC232A4u,0xC232A8u,0xC232AAu,0xC232ACu,0xC232B2u,0xC232B6u,0xC232BCu,0xC232C4u,0xC232C8u,0xC232CAu,0xC232CCu,0xC232CEu,0xC232D0u,0xC232D4u,0xC232D8u,0xC232DCu,0xC232E2u,0xC232E8u,0xC232ECu,0xC232F0u,0xC232F2u,0xC232F4u,0xC232F8u,0xC232FAu,0xC23300u,0xC23302u,0xC23306u,0xC2330Au,0xC2330Eu,0xC23310u,0xC23312u,0xC23314u,0xC23316u,0xC2331Au,0xC2331Eu,0xC23320u,0xC23322u,0xC23324u,0xC23326u,0xC2332Cu,0xC23332u,0xC23334u,0xC23338u,0xC23340u,0xC23346u,0xC23348u,0xC2334Eu,0xC23350u,0xC23354u,0xC2335Au,0xC2335Cu,0xC2335Eu,0xC23364u,0xC23368u,0xC2336Au,0xC2336Eu,0xC23374u,0xC23376u,0xC2337Au,0xC2337Cu,0xC23380u,0xC23382u,0xC23386u,0xC2338Eu,0xC23394u,0xC23396u,0xC23398u,0xC2339Eu,0xC233A2u,0xC233A4u,0xC233A8u,0xC233AAu,0xC233B2u,0xC233B4u,0xC233BAu,0xC233BCu,0xC233C0u,0xC233C6u,0xC233CAu,0xC233D0u,0xC233D6u,0xC233DCu,0xC233E0u,0xC233E6u,0xC233EAu,0xC233F2u,0xC233F4u,0xC233FAu,0xC233FEu,0xC23406u,0xC23408u,0xC2340Au,0xC2340Eu,0xC23414u,0xC23416u,0xC2341Cu,0xC23422u,0xC23426u,0xC2342Cu,0xC23432u,0xC23434u,0xC23436u,0xC23438u,0xC2343Eu,0xC23444u,0xC23446u,0xC23448u,0xC2344Au,0xC23452u,0xC23454u,0xC2345Au,0xC23462u,0xC23466u,0xC2346Au,0xC2346Eu,0xC23476u,0xC23478u,0xC2347Au,0xC2347Eu,0xC23482u,0xC23486u,0xC23488u,0xC2348Au,0xC2348Eu,0xC23494u,0xC23496u,0xC23498u,0xC2349Cu,0xC234A4u,0xC234A6u,0xC234ACu,0xC234AEu,0xC234B4u,0xC234B8u,0xC234C0u,0xC234C6u,0xC234C8u,0xC234CCu,0xC234CEu,0xC234D2u,0xC234D4u,0xC234D6u,0xC234DCu,0xC234E0u,0xC234E6u,0xC234EAu,0xC234F0u,0xC234F8u,0xC234FAu,0xC234FCu,0xC23500u,0xC23506u,0xC23508u,0xC2350Eu,0xC23514u,0xC23518u,0xC2351Cu,0xC23522u,0xC23526u,0xC23528u,0xC2352Cu,0xC2352Eu,0xC23532u,0xC23538u,0xC23544u,0xC2354Au,0xC2354Cu,0xC23552u,0xC23554u,0xC23558u,0xC2355Au,0xC23560u,0xC23564u,0xC2356Au,0xC2356Eu,0xC23576u,0xC23578u,0xC2357Eu,0xC23580u,0xC23584u,0xC23586u,0xC2358Au,0xC23590u,0xC23596u,0xC2359Cu,0xC235A0u,0xC235A6u,0xC235A8u,0xC235AAu,0xC235ACu,0xC235AEu,0xC235B4u,0xC235BAu,0xC235BCu,0xC235BEu,0xC235C4u,0xC235C6u,0xC235C8u,0xC235CEu,0xC235D6u,0xC235D8u,0xC235DEu,0xC235E0u,0xC235E2u,0xC235E6u,0xC235E8u,0xC235EAu,0xC235F0u,0xC235F8u,0xC235FCu,0xC235FEu,0xC23606u,0xC23608u,0xC2360Cu,0xC2360Eu,0xC23614u,0xC23616u,0xC2361Au,0xC23620u,0xC236AAu,0xC236ACu,0xC236B2u,0xC236B8u,0xC236BEu,0xC236C0u,0xC236C6u,0xC236CCu,0xC236CEu,0xC236D0u,0xC236D2u,0xC236D4u,0xC236D8u,0xC236DEu,0xC236E4u,0xC236E8u,0xC236ECu,0xC236F0u,0xC236F4u,0xC236F8u,0xC236FEu,0xC23704u,0xC2370Au,0xC2370Eu,0xC23712u,0xC23716u,0xC23718u,0xC2371Eu,0xC23724u,0xC23726u,0xC23728u,0xC2372Au,0xC2372Cu,0xC23730u,0xC23736u,0xC2373Cu,0xC23740u,0xC23744u,0xC2374Au,0xC2377Eu,0xC23784u,0xC23786u,0xC2378Eu,0xC23790u,0xC23798u,0xC2379Au,0xC2379Cu,0xC2379Eu,0xC237A0u,0xC237A2u,0xC237A4u,0xC237AAu,0xC237ACu,0xC237AEu,0xC237B4u,0xC237B6u,0xC237B8u,0xC237BCu,0xC237C0u,0xC237C4u,0xC237C8u,0xC237CAu,0xC237CEu,0xC237D0u,0xC237D4u,0xC237D8u,0xC237DAu,0xC237E0u,0xC237E6u,0xC237E8u,0xC237F0u,0xC237F8u,0xC237FAu,0xC237FCu,0xC237FEu,0xC23800u,0xC23804u,0xC23808u,0xC2380Eu,0xC23812u,0xC23816u,0xC2381Au,0xC2381Cu,0xC23822u,0xC23826u,0xC2382Cu,0xC2382Eu,0xC23832u,0xC2383Au,0xC2383Cu,0xC23842u,0xC23846u,0xC2384Cu,0xC2384Eu,0xC23852u,0xC2385Au,0xC23860u,0xC23862u,0xC23868u,0xC2386Eu,0xC23870u,0xC23872u,0xC23874u,0xC23876u,0xC23878u,0xC2387Au,0xC2387Cu,0xC23880u,0xC23884u,0xC23888u,0xC2388Cu,0xC23890u,0xC23894u,0xC2389Au,0xC2389Eu,0xC238A2u,0xC238A6u,0xC238A8u,0xC238ACu,0xC238AEu,0xC238B2u,0xC238B6u,0xC238BAu,0xC238BCu,0xC238C0u,0xC238C4u,0xC238C8u,0xC238CAu,0xC238CCu,0xC238CEu,0xC238D2u,0xC238D6u,0xC238D8u,0xC238DAu,0xC238DCu,0xC238DEu,0xC238E2u,0xC238EAu,0xC238F2u,0xC238F8u,0xC238FAu,0xC23900u,0xC23902u,0xC23908u,0xC2390Au,0xC23910u,0xC23912u,0xC23918u,0xC2391Au,0xC2391Cu,0xC2391Eu,0xC23920u,0xC23922u,0xC23924u,0xC23926u,0xC2392Cu,0xC2392Eu,0xC23930u,0xC23932u,0xC23936u,0xC2393Au,0xC2393Eu,0xC23940u,0xC23942u,0xC23944u,0xC23946u,0xC23948u,0xC2394Cu,0xC23950u,0xC23954u,0xC23956u,0xC23958u,0xC2395Au,0xC2395Eu,0xC23962u,0xC23966u,0xC23968u,0xC2396Au,0xC2396Cu,0xC2396Eu,0xC23970u,0xC23974u,0xC23978u,0xC2397Cu,0xC23980u,0xC23986u,0xC23988u,0xC2398Cu,0xC23990u,0xC23996u,0xC2399Cu,0xC239A2u,0xC239A8u,0xC239ACu,0xC239B0u,0xC239B4u,0xC239B6u,0xC239BAu,0xC239C0u,0xC239C6u,0xC239CEu,0xC239D4u,0xC239D8u,0xC239DCu,0xC239E0u,0xC239E2u,0xC239E6u,0xC239EAu,0xC239EEu,0xC239F0u,0xC239F2u,0xC239F4u,0xC239FAu,0xC239FCu,0xC239FEu,0xC23A00u,0xC23A02u,0xC23A06u,0xC23A0Cu,0xC23A12u,0xC23A14u,0xC23A16u,0xC23A18u,0xC23A1Eu,0xC23A24u,0xC257DCu,0xC257E4u,0xC257EAu,0xC257ECu,0xC257F0u,0xC257F6u,0xC257F8u,0xC257FAu,0xC257FCu,0xC257FEu,0xC25800u,0xC25802u,0xC25804u,0xC25806u,0xC25808u,0xC2580Au,0xC2580Cu,0xC2580Eu,0xC25810u,0xC25816u,0xC25818u,0xC2581Au,0xC2581Cu,0xC2581Eu,0xC25820u,0xC25822u,0xC25824u,0xC25826u,0xC25828u,0xC2582Au,0xC2582Cu,0xC25830u,0xC25832u,0xC25834u,0xC25836u,0xC25838u,0xC2583Au,0xC2583Cu,0xC2583Eu,0xC25840u,0xC25842u,0xC25844u,0xC25846u,0xC2584Au,0xC2584Cu,0xC2584Eu,0xC25850u,0xC25852u,0xC2585Au,0xC2585Cu,0xC2585Eu,0xC25860u,0xC25862u};
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
                flight_record_actions_contract_enter(REG_PC,rd_u32(REG_A[7]));
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
    static const uint8_t modes[]={0,2,125,1};
    static const uint8_t classes[]={0,16,48,49};
    static const uint8_t available[]={0,1,2,3,0x10,0x20,0x30,0xff};
    unsigned i,profile=scenario; uint32_t bits=random_value(); gaddr record=0xc60800u,source=(profile&1)?0xc60c00u:0xc46184u;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    for(i=0;i<3;++i) {
        unsigned j; gaddr a=i==0?record:i==1?0xc60c00u:0xc46184u;
        for(j=0;j<128;++j) wr_u32(a+4*j,random_value());
        wr_u16(a,(uint16_t)bits); wr_u8(a+2,(uint8_t)((profile>>2)&9)); wr_u8(a+5,(uint8_t)((profile>>4)&1));
        wr_u8(a+98,classes[(profile>>5)%4]); wr_u8(a+99,classes[(profile>>7)%4]); wr_u8(a+95,available[(profile>>9)%8]);
        wr_u8(a+124,(uint8_t)(profile&255)); wr_u16(a+86,(uint16_t)((profile&8)?0x8000:(profile&16)?0xff00:0x400));
        wr_u32(a+24,(profile&32)?0x100001:0x100000); wr_u8(a+101,(uint8_t)bits);
        for(j=0;j<9;++j) wr_u16(a+146+2*j,(uint16_t)(j==0||j==4||j==8?0x4000:0));
    }
    REG_A[0]=0xc60400u; REG_A[1]=record; REG_A[2]=source; REG_A[3]=0xc61800u; REG_A[4]=0xc61200u; REG_A[5]=0xc61600u; REG_A[6]=0xc62080u;
    wr_u32(0xc1ab74u,0xc60400u); wr_u16(0xc6043eu,(uint16_t)bits); wr_u16(0xc60442u,(uint16_t)(bits>>16));
    wr_u16(0xc459c2u,(uint16_t)((profile&256)?1:0)); wr_u16(0xc45946u,(uint16_t)bits);
    wr_u8(0xc45888u,(uint8_t)((profile>>6)&1)); wr_u8(0xc458a6u,modes[(profile>>2)%4]); wr_u8(0xc458a7u,(uint8_t)((profile>>4)%5));
    wr_u8(0xc45785u,(uint8_t)((profile>>8)&1)); wr_u8(0xc45789u,(uint8_t)((profile>>1)&1)); wr_u8(0xc458b5u,(uint8_t)((profile>>3)&1));
    wr_u16(0xc458dau,(uint16_t)((profile>>2)%8)); wr_u16(0xc458deu,(uint16_t)((profile&4)?source-0xc46184u:0));
    wr_u16(0xc459b4u,(uint16_t)((profile>>6)&1)); wr_u16(0xc458dcu,(uint16_t)((profile>>7)&1));
    wr_u8(0xc45799u,(uint8_t)((profile>>5)%8)); wr_u8(0xc4579au,(uint8_t)((profile&64)?255:(profile&128)?254:0));
    wr_u8(0xc4579cu,(uint8_t)((profile&512)?255:0)); wr_u8(0xc457aeu,(uint8_t)((profile&1024)?1:0)); wr_u8(0xc45793u,(uint8_t)((profile&2048)?0:1));
    wr_u16(0xc461f2u,(uint16_t)((profile&4096)?0:1)); wr_u16(0xc4fda2u,(uint16_t)((profile&16)?0x1fc:(profile&32)?21:(profile&64)?1:0));
    for(i=0;i<8;++i) {
        uint32_t pointer=(profile&128)?0xffffffffu:(profile&256)?0:0xc61400u;
        wr_u32(0xc2366au+8*i,0x12348000u|i); wr_u32(0xc2366eu+8*i,pointer);
        wr_u16(0xc23622u+8*i,(uint16_t)(i+10)); wr_u16(0xc23624u+8*i,(uint16_t)(i+20));
    }
    wr_u32(0xc61400u,0xc63000u);
    for(i=0;i<512;++i) wr_u8(0xc63002u+i,(uint8_t)((profile&8)?255:profile&127));
    if(selected_entry==0xc23578u && (profile&4096)) wr_u8(0xc45799u,0x7f);
    if(profile&8192u) {
        if(selected_entry==0xc23228u && (profile&1)) {
            wr_u8(0xc458a6u,125); wr_u8(record+2,8); wr_u8(0xc45799u,7); wr_u8(0xc4579au,0);
        }
        if(selected_entry==0xc233aau || (selected_entry==0xc23228u && !(profile&1))) {
            wr_u8(0xc458a6u,0); wr_u8(record+2,1); wr_u8(record+5,0); wr_u8(0xc457aeu,0);
            wr_u8(0xc45799u,7); wr_u8(0xc4579au,0); wr_u16(0xc4fda2u,1); wr_u32(0xc236a6u,0xffffffffu);
        }
        if(selected_entry==0xc23578u) { wr_u8(0xc45799u,3); wr_u32(record+24,0x100000); }
    }
    if(selected_entry==0xc23354u) {
        wr_u8(0xc45799u,(uint8_t)(profile&1)); wr_u32(0xc2366au+4*(profile&1),0xc61400u);
        wr_u16(0xc4fda2u,(uint16_t)((profile&2)?0x1fc:0));
    }
    if(selected_entry==0xc2385au) REG_D[0]=(REG_D[0]&0xffff0000u)|((profile&1)?40:220);
    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; expected_sp=REG_A[7]+4; wr_u32(REG_A[7],0xc70000u);
    for(i=0;i<4;++i) wr_u32(REG_A[7]+4+4*i,random_value());
    if(selected_entry==0xc257ecu) {
        uint32_t scale=((profile&1)?0xffff0000u:0u)|((profile&32)?0xff00u:0x100u);
        wr_u32(REG_A[7]+4,fault_profile?(scale&0xffff0000u):scale); wr_u32(REG_A[7]+8,(profile&4)?0xffffff00u:256);
        wr_u32(REG_A[7]+12,(profile&8)?0xfffffe00u:512); wr_u32(REG_A[7]+16,(profile&16)?0xfffffc00u:1024);
    }
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
    fault_profile=argc>3 && !strcmp(argv[3],"fault");
    if(argc>2) selected_entry=(uint32_t)strtoul(argv[2],NULL,16);
    if(!state || !rom || !m || !base || !before || !reference || !cpu || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) {
        fprintf(stderr,"flight-record-actions oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1;
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u source\n",scenario);
        flight_record_actions_contract_reset(scenario,1);
        if(!setjmp(fault_stop) && source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"flight-record-actions oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        flight_record_actions_contract_reset(scenario,0);
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u C\n",scenario);
        if(!setjmp(fault_stop)) switch(selected_entry) {
        case 0xc23354u: fa18_test_flight_record_append(); break;
        case 0xc2385au: fa18_test_flight_record_motion(); break;
        case 0xc230e8u: glue_C230E8(); break;
        case 0xc23116u: glue_C23116(); break;
        case 0xc23186u: glue_C23186(); break;
        case 0xc23228u: glue_C23228(); break;
        case 0xc233aau: glue_C233AA(); break;
        case 0xc23578u: glue_C23578(); break;
        case 0xc236aau: glue_C236AA(); break;
        case 0xc23716u: glue_C23716(); break;
        case 0xc2377eu: glue_C2377E(); break;
        case 0xc257ecu: glue_C257EC(); break;
        default: return 1;
        }
        flight_record_actions_contract_finish();
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"flight-record-actions oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=(fault_profile?0xc06c02u:0xc70000u) || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"flight-record-actions oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"flight-record-actions oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("flight-record-actions oracle %06X: %u %s matched all registers, PC, full SR and all RAM; %u source boundaries observed\n",selected_entry,cases,fault_profile?"first fault-child boundary observations":selected_entry==0xc23354u||selected_entry==0xc2385au?"internal source segments":"complete calls",count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
