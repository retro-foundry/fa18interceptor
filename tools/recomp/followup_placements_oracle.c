/* Complete parent oracle. Original instructions independently evaluate
 * the complete entry and source-format lists, including real descriptor children. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "m68kcpu.h"
#include "machine.h"
#include "bus.h"
#include "recomp_runtime.h"
#include "recomp_ports.h"
#include "memory.h"
#include "ports_glue.h"
#include "globals.h"

extern int fa18_write_log_active;
extern int64_t fa18_next_event;
static uint32_t seed = 0xc1cb14u;
static uint32_t random_value(void) {
    seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5; return seed;
}
static uint8_t *read_file(const char *path, size_t *size) {
    FILE *file = fopen(path, "rb"); long length; uint8_t *bytes;
    if (!file) return NULL;
    if (fseek(file, 0, SEEK_END) || (length = ftell(file)) < 0 || fseek(file, 0, SEEK_SET)) return NULL;
    bytes = malloc((size_t)length);
    if (!bytes || fread(bytes, 1, (size_t)length, file) != (size_t)length || fclose(file)) return NULL;
    *size = (size_t)length; return bytes;
}
static int source_call(uint32_t ret, uint32_t sp) {
    unsigned dispatch;
    for (dispatch = 0; dispatch < 10000; ++dispatch) {
        int lo = 0, hi = fa18_recomp_entry_count, result;
        if (REG_PC == ret && REG_A[7] == sp) return FA18_RET;
        while (lo < hi) {
            int mid = lo + (hi - lo) / 2;
            if (fa18_recomp_entries[mid].pc < REG_PC) lo = mid + 1; else hi = mid;
        }
        if (lo == fa18_recomp_entry_count || fa18_recomp_entries[lo].pc != REG_PC) return FA18_EXIT_INTERP;
        result = fa18_recomp_functions[fa18_recomp_entries[lo].function].fn((int)fa18_recomp_entries[lo].label);
        if (result == FA18_EXIT_INTERP) return result;
    }
    return FA18_EXIT_INTERP;
}
static void fixture(unsigned scenario) {
    static const uint8_t countdowns[]={0,1,2,127,128,129,255};
    static const int16_t caches[]={-1,0,1,255,256,1023,1024,32767,-32768};
    static const int32_t depths[]={0,-1,0x7fffffff,(int32_t)0x80000000u,0x3fffffff,-0x400001};
    unsigned i,j,primary=scenario%4u,alternate=(scenario/4u)%6u;
    uint16_t offset=(scenario&0x20u)?24:0;
    for(i=0;i<16;++i) {
        gaddr record=CONTROL_RECORDS+i*512u;
        for(j=0;j<32;++j) wr_u8(record+j,(uint8_t)random_value());
        wr_u8(record+1,(scenario&(1u<<(i%8u)))?0x40:0);
        for(j=0;j<3;++j) wr_u32(record+0x14+4*j,random_value());
        wr_s32(record+0x10,depths[(scenario+i)%6u]);
        wr_u32(0xc22188u+i*20u,scenario&0x400u?0xc096cau:0xc2f490u);
        wr_u32(0xc2218cu+i*20u,0xc62000u);
        wr_u32(0xc22190u+i*20u,0xc62100u); wr_u32(0xc22194u+i*20u,random_value());
    }
    for(i=0;i<primary;++i) wr_u16(0xc4e98au+2*i,1u+i);
    wr_u16(0xc4e98au+2*primary,(scenario&0x1000u)?0xffffu:0);
    wr_u16(0xc459b0u,offset); wr_u16(0xc458deu,(uint16_t)((scenario%16u)*512u));
    wr_u8(0xc45785u,scenario&0x40u?1:0);
    wr_u8(0xc45837u,(uint8_t)(scenario>>8)); wr_u16(0xc458dau,(uint16_t)(scenario>>3));
    wr_u8(0xc458bcu,(uint8_t)random_value());
    wr_s32(POSITION_BIAS,depths[(scenario/12u)%6u]);
    wr_s32(0xc45a78u,depths[(scenario/72u)%6u]);
    wr_u16(0xc45a72u,(uint16_t)random_value()); wr_u16(0xc45a76u,(uint16_t)random_value());
    wr_u16(0xc4594cu,(uint16_t)random_value()); wr_u16(0xc4594eu,(uint16_t)random_value());
    wr_u32(PROJECTION_Y,random_value()); wr_u16(PROJECTION_WORDS,(uint16_t)random_value());
    wr_u16(PROJECTION_WORDS+4,(uint16_t)random_value());
    wr_u32(LIST_WRITE,0xc64000u);
    for(i=0;i<9;++i) wr_u16(LIST_MATRIX+2*i,(uint16_t)random_value());
    for(i=0;i<alternate;++i) {
        gaddr record=0xc4f6cau+offset+24*i,descriptor=0xc61000u+24*i;
        uint16_t header=(uint16_t)(((scenario>>4)&255u)<<8|((scenario+i)&15u));
        if((scenario+i)%3u==1u) header|=0x10u;
        if((scenario+i)%3u==2u) header|=0x40u;
        wr_u16(record,header); wr_u32(record+2,descriptor);
        for(j=0;j<3;++j) wr_u16(record+6+2*j,(uint16_t)random_value());
        wr_u32(record+12,random_value()); wr_s16(record+16,caches[(scenario/128u+i)%9u]);
        wr_u8(record+18,countdowns[(scenario/256u+i)%7u]); wr_u8(record+19,(uint8_t)random_value());
        wr_u16(record+20,scenario&4u?0xffffu:0x400u);
        wr_u32(descriptor,scenario%19u==0?0xffffffffu:scenario&0x800u?0xc1ed48u:scenario&0x400u?0xc096cau:0xc2f490u);
        wr_u32(descriptor+4,0xc62000u); wr_u32(descriptor+8,0xc62100u); wr_u32(descriptor+12,random_value());
        if(header&0x40u) {
            gaddr owner=WORKSPACE_RECORDS+(gaddr)(int32_t)((int16_t)(header&0xff00u)>>3);
            for(j=0;j<3;++j) wr_u32(owner+0x14+4*j,random_value());
        }
    }
    wr_u16(0xc4f6cau+offset+24*alternate,0xffffu); wr_u16(0xc62100u,0xffffu);
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[7]=0xc7ff00u; wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31u)); REG_PC=0xc1ccbcu;
    fa18_recomp_abort=0; fa18_next_event=INT64_MAX; SET_CYCLES(100000000);
}
static void position_fixture(unsigned scenario) {
    unsigned i, input=scenario/2u;
    uint16_t header=(uint16_t)(((input>>5)&255u)<<8 | (random_value()&0xf0u) | (input&15u));
    int16_t high=(int16_t)(header&0xff00u);
    gaddr record=scenario&1u ? CONTROL_RECORDS+(gaddr)(int32_t)(int16_t)(high*2u)
                            : WORKSPACE_RECORDS+(gaddr)(int32_t)(high>>3);
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_D[7]=(REG_D[7]&0xffff0000u)|header;
    for(i=0;i<3;++i) wr_u32(record+0x14+4*i,random_value());
    wr_u32(POSITION_BIAS,random_value());
    REG_A[7]=0xc7ff00u; wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700u|(input&31u));
    REG_PC=scenario&1u?0xc1d0b6u:0xc1d0a4u;
    fa18_recomp_abort=0; fa18_next_event=INT64_MAX; SET_CYCLES(100000000);
}
int main(int argc, char **argv) {
    size_t state_size = 0, rom_size = 0;
    uint8_t *state = read_file("captures/native/demo01/state.bin", &state_size);
    uint8_t *rom = read_file("local/system/kick13.rom", &rom_size);
    FA18Machine *m = calloc(1, sizeof *m), *base = malloc(sizeof *base), *before = malloc(sizeof *before);
    uint8_t *reference = malloc(FA18_CHIP_SIZE + FA18_SLOW_SIZE);
    void *cpu = malloc(m68k_context_size()); char error[256];
    unsigned cases = argc > 1 ? (unsigned)strtoul(argv[1], NULL, 10) : 8192, scenario;
    int positions=argc>2 && !strcmp(argv[2],"positions");
    unsigned stack_bytes=positions?0:160;
    if (!state || !rom || !m || !base || !before || !reference || !cpu || !cases) return 1;
    if (!fa18_machine_load_state(m, state, state_size, rom, rom_size, error, sizeof error)) {
        fprintf(stderr, "followup placement oracle: %s\n", error); return 1;
    }
    free(state); free(rom); memcpy(base, m, sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF, NULL); fa18_bus_timing = 0;
    for (scenario = 0; scenario < cases; ++scenario) {
        uint32_t regs[16], entry, sr; unsigned i;
        memcpy(m, base, sizeof *m);
        if(positions) position_fixture(scenario); else fixture(scenario);
        entry = REG_PC;
        memcpy(before, m, sizeof *m); m68k_get_context(cpu); fa18_write_log_active = 1;
        if (source_call(0xc70000u, 0xc7ff04u) != FA18_RET) {
            fprintf(stderr, "followup placement oracle: case %u source did not return at %06X\n", scenario, REG_PC); return 1;
        }
        memcpy(regs, REG_DA, sizeof regs); sr = m68k_get_reg(NULL, M68K_REG_SR);
        memcpy(reference, m->chip, FA18_CHIP_SIZE);
        memcpy(reference + FA18_CHIP_SIZE, m->slow, FA18_SLOW_SIZE);
        memcpy(m, before, sizeof *m); m68k_set_context(cpu);
        if(entry==0xc1d0a4u) glue_C1D0A4();
        else if(entry==0xc1d0b6u) glue_C1D0B6();
        else glue_C1CCBC();
        fa18_write_log_active = 0;
        for (i = 0; i < 16; ++i) if (regs[i] != REG_DA[i]) {
            fprintf(stderr, "followup placement oracle: case %u %c%u source %08X C %08X\n",
                scenario, i < 8 ? 'D' : 'A', i & 7u, regs[i], REG_DA[i]); return 1;
        }
        if (REG_PC != 0xc70000u || ((sr ^ m68k_get_reg(NULL, M68K_REG_SR)) & (positions?0xffffu:0xff00u))) {
            fprintf(stderr,"followup placement oracle: case %u PC/SR source %04X C %04X\n",scenario,sr,m68k_get_reg(NULL,M68K_REG_SR));
            return 1;
        }
        for (i = 0; i < FA18_CHIP_SIZE + FA18_SLOW_SIZE; ++i) {
            gaddr address = i < FA18_CHIP_SIZE ? i : i - FA18_CHIP_SIZE + FA18_SLOW_BASE;
            uint8_t got = i < FA18_CHIP_SIZE ? m->chip[i] : m->slow[i - FA18_CHIP_SIZE];
            /* Parent has no locals. Empty renderer callbacks use their
             * original LINK -152 frame and one child return: 160 bytes. */
            if (address >= 0xc7ff00u - stack_bytes && address < 0xc7ff00u) continue;
            if (got != reference[i]) {
                fprintf(stderr, "followup placement oracle: case %u byte %06X source %02X C %02X\n",
                    scenario, address, reference[i], got); return 1;
            }
        }
    }
    printf("followup placement oracle: %u complete %s calls matched all registers, PC, %s and RAM outside %u-byte child stack\n",
           cases,positions?"position":"parent",positions?"full SR":"SR control",stack_bytes);
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
