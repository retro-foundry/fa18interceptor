/* Original sound-resource parents and descriptor services. CPU/ROM are confined
 * to this comparison fixture; sample bytes come from the original ADF. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "machine.h"
#include "bus.h"
#include "m68kcpu.h"
#include "m68kops.h"
#include "recomp_runtime.h"
#include "recomp_ports.h"
#include "../../../fa18-interceptor-decomp/src/audio/audio_assets.c"
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
static unsigned failed_slot;
static int failed_setup=-1;
static int test_load(void *context,const char *path,unsigned slot) {
    return slot==failed_slot?0:load(context,path,slot);
}
static int test_duplicate(void *context,unsigned source,unsigned destination) {
    return destination==failed_slot?0:duplicate(context,source,destination);
}
static int32_t test_setup(void *context,enum InputDisplayChild child) {
    return (int)child==failed_setup?0:setup_sound_child(context,child);
}
static int original(gaddr entry,AudioAssets *assets,int descriptor) {
    memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00;wr_u32(REG_A[7],0xc70000);
    wr_u32(REG_A[7]+4,568);wr_u32(REG_A[7]+8,37);
    if(entry==0xc50614) {wr_u32(REG_A[7]+4,13);wr_u32(REG_A[7]+8,37);}
    REG_PC=entry;m68k_set_reg(M68K_REG_SR,0x2700);fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned steps=0;steps<2000000;++steps) {
        if(REG_PC==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        uint32_t result=0;int child=1;
        if(!descriptor && REG_PC==0xc5058e) {
            gaddr string=rd_u32(REG_A[7]+4);char path[80];unsigned i;
            for(i=0;i+1<sizeof path && rd_u8(string+i);++i) path[i]=(char)rd_u8(string+i);
            path[i]=0;if(strncmp(path,"df0:",4)) return 0;
            unsigned slot=rd_u32(REG_A[7]+8);
            int fail=(failed_setup==IDS_LOAD_TEXT_2 && !strcmp(path+4,"text/textegn")) ||
                (failed_setup==IDS_LOAD_TEXT_5 && slot==5) || (failed_setup==IDS_LOAD_TEXT_11 && slot==11) ||
                (failed_setup==IDS_LOAD_TEXT_0 && slot==0) || (failed_setup==IDS_LOAD_TEXT_8 && slot==8);
            result=fail?0:test_load(assets,path+4,slot);
        } else if(!descriptor && REG_PC==0xc50614) {
            result=test_duplicate(assets,rd_u32(REG_A[7]+4),rd_u32(REG_A[7]+8));
        } else if(!descriptor && REG_PC==0xc5046c) {
            unsigned slot=rd_u32(REG_A[7]+8);
            int fail=(failed_setup==IDS_ALLOCATE_TEXT_4 && slot==4) || (failed_setup==IDS_ALLOCATE_TEXT_6 && slot==6);
            result=fail?0:create_voice(assets,rd_u32(REG_A[7]+4),slot);
        } else if(!descriptor && REG_PC==0xc507d6) release(assets,rd_u32(REG_A[7]+4));
        else if(descriptor && REG_PC==0xc53b30) result=allocate(assets,rd_u32(REG_A[7]+4));
        else child=0;
        if(child) {REG_D[0]=result;REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;}
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"sound-resource source stopped at %06X\n",REG_PC);return 0;
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];AmigaOfs disk={0};
    uint8_t *state=read_bytes("captures/native/demo01/state.bin",&ns),*rom=read_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?read_bytes(argv[1],&nd):NULL;
    FA18Machine *machine=calloc(1,sizeof *machine),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state || !rom || !data || nd!=0x100000 || !machine || !before || !expected || !amiga_ofs_open(&disk,"local/media/fa18.adf")) return 1;
    if(!fa18_machine_load_state(machine,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    unsigned count=0;
    for(unsigned test=0;test<23;++test) {
        memcpy(machine->chip,data,0x80000);memcpy(machine->slow,data+0x80000,0x80000);
        AudioAssets assets={&disk,{0x8000,0xc55000,0x68000},error,sizeof error,0},saved_assets=assets;
        unsigned descriptor=test>=11 && test<15;
        gaddr entry=test<3?0xc17510:test<11?0xc1756a:test<13?0xc5046c:test<15?0xc50614:0xc1787a;
        failed_slot=test<3?(test==1?35:test==2?36:~0u):test<11?(test==3?~0u:(unsigned[]){13,15,17,19,25,27,33}[test-4]):~0u;
        failed_setup=test<16?-1:(int[]){IDS_LOAD_TEXT_2,IDS_ALLOCATE_TEXT_4,IDS_ALLOCATE_TEXT_6,
            IDS_LOAD_TEXT_5,IDS_LOAD_TEXT_11,IDS_LOAD_TEXT_0,IDS_LOAD_TEXT_8}[test-16];
        if(!descriptor) memset(native_storage_range(SOUND_VOICES,40*4),0,40*4);
        else {
            wr_u32(SOUND_VOICES+37*4,0);wr_u32(0xc06cc6,1);
            if(test>=13) {
                gaddr record=sample_voice(13);
                for(unsigned field=8;field<64;field+=4) wr_u32(record+field,0x55100000u+field*0x103u);
                if(test==14) wr_u32(record+4,rd_u32(record+4)|0x80000000u);
            }
        }
        wr_u8(SOUND_FLAGS,0x15);wr_u8(SOUND_FLAGS-1,0x80);
        if(test==12) wr_u32(0xc06cc6,0);
        memcpy(before,machine,sizeof *before);
        if(!original(entry,&assets,descriptor)) return 1;
        memcpy(expected,machine->chip,0x80000);memcpy(expected+0x80000,machine->slow,0x80000);
        memcpy(machine,before,sizeof *before);assets=saved_assets;
        const SoundResourceHooks hooks={test_load,test_duplicate,release,&assets};
        if(test<3) load_intro_sound_resources(&hooks);
        else if(test<11) load_menu_sound_resources(&hooks);
        else if(test<13) {if(rd_u32(0xc06cc6)) create_voice(&assets,568,37);}
        else if(test<15) duplicate(&assets,13,37);
        else {
            const InputDisplayHooks setup={test_setup,NULL,&assets};
            load_setup_text_resources(0x4a20,&setup);
        }
        /* C1787A's source stack local corresponds to a host-owned two-byte
         * local at $4A1E; it is deliberately excluded, like other stack data. */
        for(unsigned i=0;i<0xff000;++i) {
            if(test>=15 && (i==0x4a1e || i==0x4a1f)) continue;
            uint8_t actual=i<0x80000?machine->chip[i]:machine->slow[i-0x80000];
            if(actual!=expected[i]) {fprintf(stderr,"sound case %u parent %06X offset %06X source %02X native %02X\n",test,entry,i,expected[i],actual);return 1;}
        }
        ++count;
    }
    printf("%u intro/menu sound-resource and descriptor cases match original non-stack RAM\n",count);
    amiga_ofs_close(&disk);free(expected);free(before);free(machine);free(data);free(state);free(rom);return 0;
}
