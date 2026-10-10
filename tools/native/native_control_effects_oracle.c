/* Full original C1518C launch/motion/render parent versus connected native C.
 * Source state comes from an actual disk/input runner export. */
#define main input_fixture_main
#include "native_input_oracle.c"
#undef main
#include "hardware.h"
#include "../../../fa18-interceptor-decomp/src/flight/control_effects.c"

static unsigned component_calls,face_calls,plane_calls,collision_hits,collision_misses;
static int scratch(unsigned i) {
    return (i>=CONTROL_FRAME-22&&i<CONTROL_FRAME)||
           (i>=ACTION_FRAME-18&&i<ACTION_FRAME+16)||
           (i>=AIM_FRAME-24&&i<AIM_FRAME+12)||
           (i>=DIRECTION_FRAME-12&&i<DIRECTION_FRAME+24)||
           (i>=NORMAL_FRAME+8&&i<NORMAL_FRAME+24)||
           (i>=COLLISION_FRAME-92&&i<COLLISION_FRAME+12);
}
static int original_control_effects(void) {
    memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00;wr_u32(REG_A[7],0xc70000);
    m68k_set_reg(M68K_REG_SR,0x2700);REG_PC=0xc1518c;
    fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned steps=0;steps<1000000;++steps) {
        if(REG_PC==0xc70000 && REG_A[7]==0xc7ff04) {wait_blitter();return 1;}
        component_calls+=REG_PC==0xc26cc0;face_calls+=REG_PC==0xc26d8a;plane_calls+=REG_PC==0xc27456;
        if(REG_PC==0xc15450) {collision_hits+=REG_D[0]!=0;collision_misses+=REG_D[0]==0;}
        int cycles_before=GET_CYCLES();
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
        fa18_machine->cycle+=cycles_before-GET_CYCLES();
    }
    fprintf(stderr,"source control effects failed at %06X\n",REG_PC);return 0;
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=read_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=read_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc>=2?read_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state||!rom||!data||nd!=0x100000||!m||!before||!expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    if(argc==3) {
        size_t na=0;uint8_t *actual=read_bytes(argv[2],&na);unsigned differences=0;
        if(!actual || na!=0x100000) return 1;
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        if(!original_control_effects()) return 1;
        for(unsigned i=0;i<0xff000;++i) {
            if(scratch(i)) continue;
            uint8_t source=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual[i]!=source) {
                if(differences<10) fprintf(stderr,"parent %06X source %02X native %02X\n",
                    i<0x80000?i:0xc00000+i-0x80000,source,actual[i]);
                ++differences;
            }
        }
        printf("Actual native control parent: differences=%u component=%u face=%u plane=%u hit=%u miss=%u\n",
               differences,component_calls,face_calls,plane_calls,collision_hits,collision_misses);
        return differences!=0 || (!component_calls && !face_calls);
    }
    unsigned launches=0,updates=0;
    for(unsigned variant=0;variant<256;++variant) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        /* Original ten 128-byte control slots. These inputs stay in the oracle. */
        for(unsigned i=0;i<0x500;++i) wr_u8(0xc45c72u+i,0);
        wr_u8(PAUSE_A,0);wr_u8(0xc457bdu,0);wr_u8(0xc457adu,0);
        wr_u8(MISSION_FLAGS_A,(uint8_t)(1+(variant&1)));
        wr_u8(0xc46201u,0x50);wr_u16(0xc461e4u,30);
        const gaddr record=0xc45c72u+((variant/2)%10)*128;
        if(variant>=20) {
            ++updates;wr_u8(MISSION_FLAGS_A,0);wr_u8(0xc46201u,0);
            wr_u16(record+38,variant&1?0x2401:0x1401);
            wr_u16(record+40,(uint16_t)((variant/2)%64));
            wr_u32(record,rd_u32(CONTROL_RECORDS+20)&0x3fffffu);
            wr_u32(record+4,rd_u32(CONTROL_RECORDS+24));
            wr_u32(record+8,rd_u32(CONTROL_RECORDS+28)&0x3fffffu);
            wr_u16(record+48,rd_u16(CONTROL_RECORDS+6));wr_u16(record+50,rd_u16(CONTROL_RECORDS+8));
            wr_u32(record+12,0x1000);wr_u32(record+16,0xffffe000u);wr_u32(record+20,0x3000);
            wr_u32(record+24,0x200);wr_u32(record+28,0xfffffc00u);wr_u32(record+32,0x600);
            wr_u8(0xc4588cu,(uint8_t)(1+(variant/2)%10));wr_u8(0xc4588du,rd_u8(0xc4588cu));
        } else ++launches;
        if(variant>=96) {
            /* Visible drawing contracts under the original identity view,
             * including ordinary point records and each chaff layer size. */
            wr_u16(0xc4594cu,0);wr_u16(0xc4594eu,0);
            wr_u16(0xc45a72u,0);wr_u16(0xc45a76u,0);
            wr_u32(0xc45a66u,0);wr_u32(0xc45a78u,0);
            for(unsigned i=0;i<9;++i) wr_u16(VIEW_ANGLE_MATRIX+2*i,i%4==0?0x100:0);
            wr_u16(record+48,0);wr_u16(record+50,0);
            wr_u32(record,0);wr_u32(record+4,0x1000);wr_u32(record+8,0x100000);
            wr_u16(record+40,(uint16_t)((variant-96)%64));
            if(variant>=160) wr_u16(record+38,0x401);
        }
        if(variant>=192) {
            /* A single original aircraft/carrier descriptor, with collision
             * branch selection confined to this test's metadata. Its model
             * point/face streams are the actual disk-loaded record's data. */
            const gaddr target=CONTROL_RECORDS+14*512;
            wr_u8(0xc4585eu,1);wr_u32(0xc459c6u,0x6500);
            wr_u16(0x6500,0x0e10);wr_u32(0x6502,0x6600);wr_u32(0x6604,0x6700);
            wr_u16(0x6700,0);wr_u8(0x6707,variant&1?16:0);
            wr_u16(target,rd_u16(target)|0x40);wr_u8(target+123,(uint8_t)(((variant-192)/4)%4));
            wr_u16(record+46,0);wr_u16(record+48,rd_u16(target+6));wr_u16(record+50,rd_u16(target+8));
            wr_u32(record,(uint32_t)(int32_t)rd_s16(target+12)<<8);
            wr_u32(record+4,rd_u32(target+16)<<8);
            wr_u32(record+8,(uint32_t)(int32_t)rd_s16(target+14)<<8);
            if(variant>=224) wr_u32(record+4,rd_u32(record+4)+0x10000);
            for(unsigned i=0;i<6;++i) wr_u32(record+12+4*i,0);
            wr_u16(record+38,0x401);wr_u16(record+40,10);
        }
        memcpy(before,m,sizeof *m);
        if(!original_control_effects()) {fprintf(stderr,"effect variant %u\n",variant);return 1;}
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);native_control_effects();wait_blitter();
        unsigned differences=0;
        for(unsigned i=0;i<0xff000;++i) {
            /* Only native automatic-frame scratch replaces source stack. */
            if(scratch(i)) continue;
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                if(differences<10) fprintf(stderr,"effect %u %06X: source %02X native %02X\n",variant,
                    i<0x80000?i:0xc00000+i-0x80000,expected[i],actual);
                ++differences;
            }
        }
        if(differences) {fprintf(stderr,"effect %u: %u compared RAM differences\n",variant,differences);return 1;}
    }
    printf("256 full control-effect parents match original non-stack RAM (%u launches, %u motion/drawing/expiry cases)\n",launches,updates);
    printf("Source collision coverage: component=%u face=%u plane=%u hit=%u miss=%u\n",
           component_calls,face_calls,plane_calls,collision_hits,collision_misses);
    if(!component_calls || !face_calls || !plane_calls || !collision_hits || !collision_misses) return 1;
    free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
