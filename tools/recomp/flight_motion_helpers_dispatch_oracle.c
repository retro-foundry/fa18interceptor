/* Complete flight-motion-helpers proof, including cold internal paths and real children. */
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
extern void fa18_flight_motion_helpers_fixture_begin(const char *phase);
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
static uint32_t selected_entry=0xc26322u;
static const uint32_t source_boundaries[]={0xC26322u,0xC26324u,0xC26326u,0xC26328u,0xC2632Au,0xC2632Cu,0xC26330u,0xC26332u,0xC26334u,0xC26336u,0xC26338u,0xC2633Au,0xC2633Cu,0xC2633Eu,0xC26340u,0xC26342u,0xC26344u,0xC26348u,0xC2634Cu,0xC26350u,0xC26352u,0xC26358u,0xC2635Au,0xC26360u,0xC26362u,0xC26366u,0xC2636Au,0xC2636Cu,0xC26370u,0xC26374u,0xC2637Au,0xC26380u,0xC26382u,0xC26384u,0xC26386u,0xC2638Au,0xC2638Cu,0xC2638Eu,0xC26392u,0xC2639Au,0xC26C72u,0xC26C76u,0xC26C7Au,0xC26C7Cu,0xC26C82u,0xC26C84u,0xC26C8Au,0xC26C8Cu,0xC26C8Eu,0xC26C92u,0xC26C94u,0xC26C96u,0xC26C98u,0xC26C9Au,0xC26C9Cu,0xC26C9Eu,0xC26CA0u,0xC26CA2u,0xC26CA4u,0xC26CA6u,0xC26CAAu,0xC26CAEu,0xC26CB2u,0xC26CB4u,0xC26CB6u,0xC26CB8u,0xC26CBAu,0xC26CBCu,0xC26CBEu,0xC26CC0u,0xC26CC4u,0xC26CC8u,0xC26CCEu,0xC26CD0u,0xC26CD2u,0xC26CD4u,0xC26CD6u,0xC26CD8u,0xC26CDCu,0xC26CE2u,0xC26CE8u,0xC26CEAu,0xC26CECu,0xC26CEEu,0xC26CF4u,0xC26CF6u,0xC26CFCu,0xC26CFEu,0xC26D04u,0xC26D06u,0xC26D0Au,0xC26D0Eu,0xC26D12u,0xC26D16u,0xC26D1Au,0xC26D1Eu,0xC26D20u,0xC26D22u,0xC26D24u,0xC26D26u,0xC26D28u,0xC26D2Eu,0xC26D30u,0xC26D34u,0xC26D3Au,0xC26D3Cu,0xC26D42u,0xC26D44u,0xC26D4Au,0xC26D4Cu,0xC26D50u,0xC26D54u,0xC26D58u,0xC26D5Au,0xC26D5Cu,0xC26D5Eu,0xC26D62u,0xC26D64u,0xC26D68u,0xC26D6Cu,0xC26D70u,0xC26D72u,0xC26D74u,0xC26D76u,0xC26D7Cu,0xC26D7Eu,0xC26D80u,0xC26D82u,0xC26D84u,0xC26D88u,0xC26D8Au,0xC26D8Eu,0xC26D92u,0xC26D98u,0xC26D9Au,0xC26D9Cu,0xC26D9Eu,0xC26DA0u,0xC26DA2u,0xC26DA4u,0xC26DAAu,0xC26DB0u,0xC26DB2u,0xC26DB4u,0xC26DB6u,0xC26DBCu,0xC26DC0u,0xC26DC4u,0xC26DC8u,0xC26DCCu,0xC26DCEu,0xC26DD2u,0xC26DD4u,0xC26DD6u,0xC26DD8u,0xC26DDCu,0xC26DE0u,0xC26DE4u,0xC26DE6u,0xC26DE8u,0xC26DEAu,0xC26DEEu,0xC26DF2u,0xC26DF6u,0xC26DFAu,0xC26DFEu,0xC26E00u,0xC26E02u,0xC26E04u,0xC26E08u,0xC26E0Au,0xC26E0Eu,0xC26E14u,0xC26E18u,0xC26E1Cu,0xC26E20u,0xC26E22u,0xC26E26u,0xC26E28u,0xC26E2Eu,0xC26E30u,0xC26E34u,0xC26E38u,0xC26E3Eu,0xC26E40u,0xC26E44u,0xC26E48u,0xC26E4Au,0xC26E4Cu,0xC26E4Eu,0xC26E50u,0xC26E52u,0xC26E54u,0xC26E56u,0xC26E58u,0xC26E5Au,0xC26E5Cu,0xC26E5Eu,0xC26E60u,0xC26E62u,0xC26E64u,0xC26E66u,0xC26E68u,0xC26E6Au,0xC26E6Eu,0xC26E72u,0xC26E74u,0xC26E76u,0xC26E78u,0xC26E7Au,0xC26E7Eu,0xC26E82u,0xC26E86u,0xC26E88u,0xC26E8Au,0xC26E8Cu,0xC26E8Eu,0xC26E90u,0xC26E92u,0xC26E94u,0xC26E96u,0xC26E98u,0xC26E9Au,0xC26E9Cu,0xC26EA0u,0xC26EA2u,0xC26EA4u,0xC26EA6u,0xC26EAAu};
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
    static const uint32_t extremes[]={0,1,0xffffffffu,0x80000000u,0x7fffffffu,0x08000000u,0x7ffu,0xfffff800u};
    unsigned i,p=scenario,index=(p>>5)&1; gaddr record=0xc60800u,scene=0xc61000u,root=0xc46184u+512*index;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    for(i=0;i<128;++i) { wr_u32(record+4*i,random_value()); wr_u32(root+4*i,random_value()); }
    for(i=0;i<96;++i) { wr_u32(0xc62000u+4*i,random_value()); wr_u32(0xc7fd80u+4*i,random_value()); }
    REG_A[0]=0xc60400u; REG_A[1]=record; REG_A[2]=0xc60c00u; REG_A[3]=scene; REG_A[4]=0xc61200u; REG_A[5]=0xc61600u; REG_A[6]=0xc62080u;
    REG_D[5]=(p&64)?extremes[p&7]:random_value(); REG_D[7]=(p&128)?extremes[(p>>3)&7]:random_value();
    REG_D[6]=(p&16)?0xffu:(p&32)?0xffffff00u:(random_value()&0x00ffff00u)|0x100u;
    wr_u32(record+24,extremes[(p>>2)&7]);
    for(i=0;i<16;++i) wr_u16(0xc48184u+32*i,(uint16_t)((i==p%17)?0:64));
    if(p&256) REG_D[1]=REG_D[1]&0xffff0000u;
    wr_u16(0xc459b8u,(uint16_t)index);
    if(selected_entry==0xc26c72u) {
        gaddr projected=0xc45c72u+64*((p>>3)&15);
        wr_u32(projected,random_value()); wr_u32(projected+4,extremes[(p>>2)&7]); wr_u32(projected+8,random_value());
        wr_u32(projected+12,REG_D[5]); wr_u32(projected+16,REG_D[6]); wr_u32(projected+20,REG_D[7]);
    }
    if(selected_entry==0xc26cc0u) {
        unsigned upper=(p>>1)&1,second=(p>>2)&1; gaddr tables[]={0xc39168u,0xc39e48u,0xc391e4u,0xc39e68u};
        wr_u32(scene,(p&32)?25600:256); wr_u32(scene+4,upper?25600:0); wr_u32(scene+8,0);
        wr_u8(root+98,(p&1)?32:0); wr_u8(root+125,0); wr_u16(root+12,0); wr_u16(root+14,0); wr_u32(root+16,0);
        wr_u16(root+164,0); wr_u16(root+166,(uint16_t)(upper?second?0:200:100)); wr_u16(root+168,0);
        wr_u16(root+170,16); wr_u16(root+172,(uint16_t)(upper?(p&16)?50:200:0)); wr_u16(root+174,0);
        wr_u16(root+176,0); wr_u16(root+178,0); wr_u16(root+180,16);
        wr_u16(0xc63002u,0); wr_u16(0xc63102u,6);
        /* A triangle with a negative Y normal, followed by the original
         * negative sentinel. Both hit and miss traverse the real child. */
        wr_u16(0xc63022u,0); wr_u16(0xc63024u,6); wr_u16(0xc63026u,12);
        for(i=0;i<4;++i) {
            gaddr t=tables[i]; wr_u32(t,0xc63000u); wr_u32(t+4,(p&8)?0xc63020u:0xffffffffu); wr_u32(t+8,0xffffffffu);
            wr_u32(t+22,0xc63100u); wr_u32(t+26,(p&8)?0xc63020u:0xffffffffu); wr_u32(t+30,0xffffffffu);
        }
    }
    if(selected_entry==0xc26d8au) {
        wr_u32(scene,random_value()); wr_u32(scene+4,random_value()); wr_u32(scene+8,random_value());
        wr_u8(root+123,(uint8_t)((p&15)|((p>>2)&0xf0))); wr_u8(root+125,(uint8_t)((p>>4)&15));
        for(i=0;i<16;++i) wr_u32(0xc38b34u+4*i,0xc64000u);
        wr_u16(0xc64000u,(uint16_t)((p&64)?0x8000:(p&16)?0x4000:0)); wr_u16(0xc64002u,6); wr_u16(0xc64004u,12);
        wr_u16(0xc64006u,0x8000); wr_u16(0xc64008u,0); wr_u16(0xc6400au,0);
        for(i=0;i<9;++i) wr_u16(root+164+2*i,(uint16_t)random_value());
    }
    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; expected_sp=REG_A[7]+4; wr_u32(REG_A[7],0xc70000u);
    wr_u32(REG_A[7]+4,(p>>3)&15);
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
        fprintf(stderr,"flight-motion-helpers dispatch oracle: %s\n",error); return 1;
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
        fa18_flight_motion_helpers_fixture_begin("original");
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"flight-motion-helpers dispatch oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        source_hardware=fa18_write_log_hardware!=0; hardware_cases+=source_hardware; matched_cases+=!source_hardware;
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu); SET_CYCLES(saved_cycles);
        fa18_write_log_active=0;
        fa18_ports_init(tested_mode,selection);
        fa18_flight_motion_helpers_fixture_begin("dispatch");
        {
            uint32_t previous=REG_PPC;
            int result;
            wr_u16(0xc70010u,0x4e71u); REG_PPC=0xc70010u;
            if(fa18_ports_enter_source_only(0,&result)) { fputs("non-call source entry was accepted\n",stderr); return 1; }
            REG_PPC=previous;
            result=fa18_recomp_call_dynamic();
            if(tested_mode==FA18_PORTS_ON && (result!=FA18_EXIT_DISPATCH || fa18_ports_active_steps()!=1)) {
                fputs("flight-motion-helpers ON entry did not start its native continuation\n",stderr); return 1;
            }
            if(result==FA18_EXIT_DISPATCH) result=fa18_recomp_resume(0xc70000u,expected_sp);
            if(result!=FA18_RET || fa18_ports_active_steps()) {
                fprintf(stderr,"flight-motion-helpers dispatch case %u mode %u did not complete at %06X\n",scenario,tested_mode,REG_PC); return 1;
            }
            if(tested_mode!=FA18_PORTS_ON && !fa18_structural_port_classified(selected_entry,source_hardware)) {
                fprintf(stderr,"flight-motion-helpers comparison case %u mode %u did not match\n",scenario,tested_mode);
                fa18_ports_report("build/recomp/flight_motion_helpers_dispatch_failed_report.json"); return 1;
            }
        }
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"flight-motion-helpers dispatch oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"flight-motion-helpers dispatch oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"flight-motion-helpers dispatch oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    {
        int result;
        unsigned guard;
        for(guard=0;guard<3;++guard) {
            int function=-1,label=-1,i;
            memcpy(m,base,sizeof *m); fixture(0);
            wr_u16(0xc70010u,0x4e71u); REG_PPC=0xc70010u;
            fa18_ports_init(guard==0?FA18_PORTS_OFF:tested_mode,guard==1?"FFFFFE":selection);
            for(i=0;i<fa18_recomp_function_count;++i)
                if(fa18_recomp_functions[i].entry==selected_entry) function=i;
            for(i=0;i<fa18_recomp_entry_count;++i)
                if(fa18_recomp_entries[i].pc==selected_entry) label=(int)fa18_recomp_entries[i].label;
            if(function>=0) {
                if(label<0) { fputs("missing translated entry label\n",stderr); return 1; }
                result=guard==2?fa18_ports_enter(function,label,0):fa18_recomp_call_dynamic();
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
      printf("flight-motion-helpers dispatch oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("classification: %u hardware-bearing source calls, %u hardware-free source calls; reference modes require exact hardware or matched classification respectively\n",hardware_cases,matched_cases);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
