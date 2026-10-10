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
#include "../../../fa18-interceptor-decomp/port/game/native/clock.c"
#include "../../../fa18-interceptor-decomp/port/game/native/model_state.c"
#include "../../../fa18-interceptor-decomp/port/game/native/records.c"
#include "stages.h"
#include "matrix_route.h"
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
            native_clock_request();
            REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        opcode=rd_u16(REG_PC); REG_PPC=REG_PC; REG_IR=opcode; REG_PC+=2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"original record update did not return at %06X\n",REG_PC); return 0;
}
static int publication_cases(void) {
    FA18Machine *saved=malloc(sizeof *saved),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!saved || !before || !expected) return 0;
    memcpy(saved,fa18_machine,sizeof *saved);
    static const int16_t indices[]={0,1,4,15,-1,-63,63,64};
    for(unsigned test=0;test<256;++test) {
        memcpy(fa18_machine,saved,sizeof *saved);
        int16_t index=indices[test%8],offset=(int16_t)((uint16_t)index<<9);
        uint32_t event=(uint32_t[]){0,1,0x7f,0x80,0xff,0x4016,0x44,0xdeadbeef}[(test/8)%8];
        wr_u8(CONTROL_RECORDS+(gaddr)(int32_t)offset+0x62,(test&8)?0x30:0x20);
        wr_u8(CONTEXT_SELECT,(test&16)?1:0);wr_u8(CONTEXT_PUBLISH_RETURN_MODE,2);
        wr_u8(KEY_TAKEN,(test&32)?1:0);wr_u8(KEY_COUNT,(uint8_t[]){0,9,10,0xff}[(test/64)%4]);
        wr_u8(KEY_WRITE,(uint8_t[]){0,9,10,0xff}[(test/64)%4]);wr_u8(KEY_TRANSLATED_WRITE,0);
        memcpy(before,fa18_machine,sizeof *before);
        memset(REG_DA,0,sizeof REG_DA);REG_D[0]=event;REG_D[1]=(uint16_t)index;
        REG_A[7]=0xc7ff00;wr_u32(REG_A[7],0xc70000);REG_PC=0xc1bee8;
        m68k_set_reg(M68K_REG_SR,0x2700);fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
        if(!original()) return 0;
        memcpy(expected,fa18_machine->chip,0x80000);memcpy(expected+0x80000,fa18_machine->slow,0x80000);
        memcpy(fa18_machine,before,sizeof *before);
        RecordView state={event};const ViewCommandHooks view={record_view_child,NULL,&state};
        const ContextPublicationHooks hooks={.view=&view,.consume=record_publication_child,.context=&state};
        publish_context_record_command(event,index,&hooks);
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?fa18_machine->chip[i]:fa18_machine->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"record publication case %u RAM %06X differs\n",test,i);return 0;
            }
        }
    }
    memcpy(fa18_machine,saved,sizeof *saved);free(expected);free(before);free(saved);
    puts("256 complete native record-publication parents match original non-stack RAM/display");return 1;
}
static int matrix_route_cases(void) {
    FA18Machine *saved=malloc(sizeof *saved),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!saved || !before || !expected) return 0;
    memcpy(saved,fa18_machine,sizeof *saved);
    const uint16_t angles[]={0,1,0x7fff,0x8000,0xffe7,0xffff,0x7080,0x3840};
    for(unsigned test=0;test<128;++test) {
        memcpy(fa18_machine,saved,sizeof *saved);
        const unsigned offset=(test&64)?0x1000:0;
        const gaddr record=CONTROL_RECORDS+offset;
        wr_u16(VIEW_RECORD,(uint16_t)offset);wr_u8(CONTEXT_SELECT,0);wr_u8(VIEW_MODE,0);
        wr_u8(MATRIX_ROUTE_SELECTOR,(uint8_t[]){0,1,0xff,0x80}[(test>>4)&3]);
        wr_u8(record+0x62,(test&32)?0x20:0x10);
        for(unsigned k=0;k<3;++k) wr_u16(record+0x66+2*k,angles[(test+k*3)&7]);
        memcpy(before,fa18_machine,sizeof *before);
        memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00;wr_u32(REG_A[7],0xc70000);
        m68k_set_reg(M68K_REG_SR,0x2700);REG_PC=0xc2db18;fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
        if(!original()) return 0;
        memcpy(expected,fa18_machine->chip,0x80000);memcpy(expected+0x80000,fa18_machine->slow,0x80000);
        memcpy(fa18_machine,before,sizeof *before);update_control_record_matrix_route(NULL);
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?fa18_machine->chip[i]:fa18_machine->slow[i-0x80000];
            if(actual!=expected[i]) {fprintf(stderr,"matrix route case %u RAM %06X differs\n",test,i<0x80000?i:0xc00000+i-0x80000);return 0;}
        }
    }
    memcpy(fa18_machine,saved,sizeof *saved);free(expected);free(before);free(saved);
    puts("128 complete C2DB18 signed-angle matrix parents match original non-stack RAM");return 1;
}
static int matrix_transform_cases(void) {
    FA18Machine *saved=malloc(sizeof *saved),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!saved || !before || !expected) return 0;
    memcpy(saved,fa18_machine,sizeof *saved);
    const uint16_t pitch[]={0,8,0x1b80,0x1bd0,0x1c00,0x1c10,0x1c20,0x1c28,
                            0x1c70,0x1cc0,0x5380,0x53d0,0x5410,0x5440,0x5488,0x5530};
    const uint16_t angles[]={0,0x38,0x3840,0x7080};
    const uint16_t coefficients[]={0x3fd7,0x3fd8,0x3fff,0x4000,0x4001,0x5fc1,0x5fc2,0x5fc3,
                                   0x6000,0x7f4c,0x7fff,0x8000,0x8001,0xa03e,0xbfff,0xc000};
    for(unsigned test=0;test<576;++test) {
        memcpy(fa18_machine,saved,sizeof *saved);
        const gaddr matrix=CONTROL_RECORDS+0x80;
        const uint16_t a=test<512 && (test&256)?7:0,b=test<512 && (test&256)?0xfffc:0,c=test<512 && (test&256)?0x4b:0;
        if(test<512) rotation_matrix(pitch[test&15],angles[(test>>4)&3],angles[(test>>6)&3],matrix);
        else {
            /* Full original owner with signed matrix coefficients near the
             * coarse clamp and word-sign boundary, as reached after flight.
             * Exercise both signs and companion-axis configurations. */
            for(unsigned k=0;k<9;++k) wr_u16(matrix+2*k,0);
            wr_u16(matrix,0x4000);wr_u16(matrix+8,(test&16)?0xc000:0x4000);
            wr_u16(matrix+16,(test&32)?0xc000:0x4000);
            wr_u16(matrix+14,coefficients[test&15]);
        }
        memcpy(before,fa18_machine,sizeof *before);
        memset(REG_DA,0,sizeof REG_DA);REG_D[0]=a;REG_D[2]=b;REG_D[4]=c;REG_A[4]=matrix;
        REG_A[7]=0xc7ff00;wr_u32(REG_A[7],0xc70000);
        m68k_set_reg(M68K_REG_SR,0x2700);REG_PC=0xc2dee0;fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
        if(!original()) return 0;
        uint32_t returned[3]={REG_D[4],REG_D[5],REG_D[6]};const uint16_t divisor=(uint16_t)REG_D[3];
        const gaddr product_cursor=REG_A[3];
        memcpy(expected,fa18_machine->chip,0x80000);memcpy(expected+0x80000,fa18_machine->slow,0x80000);
        memcpy(fa18_machine,before,sizeof *before);
        int16_t result[3];MatrixTransformAngleState transform;
        build_transform_product(matrix,a,b,c);extract_transform_angles(result,&transform);
        for(unsigned k=0;k<3;++k) if((uint32_t)(int32_t)result[k]!=returned[k]) {
            fprintf(stderr,"matrix transform case %u angle %u source %08X native %04X\n",test,k,returned[k],(uint16_t)result[k]);return 0;
        }
        if((uint16_t)transform.divisor!=divisor) {
            fprintf(stderr,"matrix transform case %u divisor source %04X native %04X\n",test,divisor,(uint16_t)transform.divisor);return 0;
        }
        if(transform.next_product!=product_cursor) {
            fprintf(stderr,"matrix transform case %u cursor source %06X native %06X\n",
                test,product_cursor,transform.next_product);return 0;
        }
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?fa18_machine->chip[i]:fa18_machine->slow[i-0x80000];
            if(actual!=expected[i]) {fprintf(stderr,"matrix transform case %u RAM %06X differs\n",test,i);return 0;}
        }
    }
    memcpy(fa18_machine,saved,sizeof *saved);free(expected);free(before);free(saved);
    puts("576 complete C2DEE0 matrix transforms match returned angles/divisor/cursor and non-stack RAM");return 1;
}
static int matrix_settle_cases(void) {
    FA18Machine *saved=malloc(sizeof *saved),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!saved || !before || !expected) return 0;
    memcpy(saved,fa18_machine,sizeof *saved);
    const uint16_t old[]={0,1,2,0x38,0x1c20,0x1c21,0x37ff,0x3840,
                          0x3841,0x3890,0x545f,0x5460,0x707f,0x7080,0x8000,0xffff};
    const uint16_t proposed[]={0,0x38,0x1c20,0x3710,0x3840,0x3850,0x5460,0x7080};
    for(unsigned test=0;test<256;++test) {
        memcpy(fa18_machine,saved,sizeof *saved);
        const gaddr record=CONTROL_RECORDS;
        wr_u8(record+3,(test&128)?0x40:0x50);wr_u8(record+5,0);wr_u16(record+0x66,0);
        wr_u16(record+0x6a,old[test&15]);
        memcpy(before,fa18_machine,sizeof *before);
        memset(REG_DA,0,sizeof REG_DA);REG_D[4]=7;REG_D[5]=0x1234;REG_D[6]=proposed[(test>>4)&7];
        REG_A[1]=record;REG_A[7]=0xc7ff00;wr_u32(REG_A[7],0xc70000);
        m68k_set_reg(M68K_REG_SR,0x2700);REG_PC=0xc2d704;fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
        if(!original()) return 0;
        memcpy(expected,fa18_machine->chip,0x80000);memcpy(expected+0x80000,fa18_machine->slow,0x80000);
        memcpy(fa18_machine,before,sizeof *before);
        uint16_t result[3]={7,0x1234,proposed[(test>>4)&7]};finish_record_matrix_angles(record,result);
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?fa18_machine->chip[i]:fa18_machine->slow[i-0x80000];
            if(actual!=expected[i]) {fprintf(stderr,"matrix settling case %u RAM %06X differs\n",test,i<0x80000?i:0xc00000+i-0x80000);return 0;}
        }
    }
    memcpy(fa18_machine,saved,sizeof *saved);free(expected);free(before);free(saved);
    puts("256 complete C2D704 angle-settling tails match original non-stack RAM");return 1;
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
    native_clock_set(rd_u32(MENU_TIME_REQUEST+32)*50u+rd_u32(MENU_TIME_REQUEST+36)/20000u);
    if(argc==3) native_clock_set((unsigned)strtoul(argv[2],NULL,10));
    if(!publication_cases()) return 1;
    if(!matrix_route_cases()) return 1;
    if(!matrix_transform_cases()) return 1;
    if(!matrix_settle_cases()) return 1;
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
