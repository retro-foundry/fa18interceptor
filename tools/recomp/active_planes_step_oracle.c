/* One-instruction oracle for plane and audio timing bridges. The
 * authoritative instruction bytes come from the sealed state; Musashi
 * evaluates them independently. Recorded full-call/live checks complement
 * this structural proof, whose chipset writes are held and DMA waits off. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "m68kcpu.h"
#include "m68kops.h"
#include "bus.h"
#include "machine.h"
#include "recomp_runtime.h"
#include "recomp_ports.h"
#include "memory.h"
#include "ports_glue.h"
/* Generated from original translation comments by the oracle launcher. */
#include "../../build/recomp/step_oracle_cases.h"

extern int64_t fa18_cycle_origin, fa18_next_event;
extern int fa18_write_log_active;

static uint8_t *read_file(const char *path, size_t *size) {
    FILE *f = fopen(path, "rb");
    long length;
    uint8_t *data;
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) || (length = ftell(f)) < 0 || fseek(f, 0, SEEK_SET)) {
        fclose(f); return NULL;
    }
    data = malloc((size_t)length);
    if (!data || fread(data, 1, (size_t)length, f) != (size_t)length) {
        free(data); fclose(f); return NULL;
    }
    fclose(f); *size = (size_t)length; return data;
}

static uint32_t seed = 0x18c2fd8cu;
static uint32_t next_value(void) {
    seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5; return seed;
}

static void fixture(uint32_t pc, unsigned scenario) {
    static const uint16_t boundaries[] = {0, 1, 0x7FFF, 0x8000, 0xFFFF, 0xFC00, 0x03FF, 0x0400, 15, 16, 31, 32, 33, 63, 64, 65};
    unsigned i;
    fa18_write_log_active = 0;
    for (i = 0; i < 16; ++i) REG_DA[i] = next_value();
    for (i = 0; i < 8; ++i)
        REG_D[i] = (REG_D[i] & 0xFFFF0000u) | boundaries[(scenario + i) % 16];
    m68k_set_reg(M68K_REG_SR, 0x2700u | (scenario & 31u));
    REG_A[0] = 0xDFF000u;
    REG_A[2] = 0xC61000u;
    REG_A[1] = 0xC61200u;
    REG_A[3] = 0xC61300u;
    REG_A[4] = 0xC61100u;
    REG_A[7] = 0xC7FF00u;
    wr_u32(REG_A[7], 0xC70000u);
    for (i = 0; i < 4; ++i) wr_u32(REG_A[2] + i * 4, next_value());
    for (i = 0; i < 3; ++i) wr_u32(0xC4591Cu + i * 4, next_value());
    wr_u32(0xC456E2u, next_value());
    wr_u16(0xC45984u, boundaries[scenario % 16]);
    wr_u8(0xC4589Bu, (uint8_t)scenario);
    wr_u8(0xC45785u, (uint8_t)(scenario >> 1));
    wr_u16(0xC4FF26u, boundaries[(scenario + 3) % 16]);
    if (pc >= 0xC32662u && pc < 0xC3316Au) {
        REG_A[0] = 0xC61000u; REG_A[3] = 0x10000u;
        for (i = 0; i < 64; ++i) {
            wr_u8(REG_A[0] + i, (uint8_t)next_value());
            wr_u32(REG_A[3] + i * 4, next_value());
        }
    }
    REG_PC = pc;
    fa18_cycle_origin = 100000000;
    fa18_next_event = INT64_MAX;
    SET_CYCLES(100000000);
    fa18_bus_reset();
    fa18_write_log_active = 1;
}

int main(int argc, char **argv) {
    size_t state_size = 0, rom_size = 0;
    uint8_t *state = read_file("captures/native/demo01/state.bin", &state_size);
    uint8_t *rom = read_file("local/system/kick13.rom", &rom_size);
    FA18Machine *m = calloc(1, sizeof *m), *before = malloc(sizeof *before);
    uint8_t *reference = malloc(FA18_CHIP_SIZE + FA18_SLOW_SIZE);
    unsigned char *cpu = malloc(m68k_context_size());
    unsigned cases = argc > 1 ? (unsigned)strtoul(argv[1], NULL, 10) : 64;
    unsigned instructions = 0, matched = 0, scenario;
    uint32_t pc;
    const char *group = argc > 2 ? argv[2] : "planes";
    unsigned item;
    char error[256], disassembly[128];
    if (!state || !rom || !m || !before || !reference || !cpu || !cases) {
        fprintf(stderr, "plane step oracle: missing inputs or allocation\n"); return 1;
    }
    if (!fa18_machine_load_state(m, state, state_size, rom, rom_size, error, sizeof error)) {
        fprintf(stderr, "plane step oracle: %s\n", error); return 1;
    }
    free(state); free(rom);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF, NULL);
    fa18_bus_timing = 0;
    for (item = 0; item < sizeof step_oracle_cases / sizeof step_oracle_cases[0]; ++item) {
        uint16_t opcode;
        pc = step_oracle_cases[item].pc;
        opcode = fa18_bus_read16(pc);
        ++instructions;
        for (scenario = 0; scenario < cases; ++scenario) {
            uint32_t regs[16], want_pc, want_sr;
            int want_cycles;
            unsigned i;
            fixture(pc, scenario);
            memcpy(before, m, sizeof *m); m68k_get_context(cpu);
            fa18_bus_begin(pc); fa18_bus_fetch(pc);
            REG_PPC = pc; REG_IR = opcode; REG_PC = pc + 2;
            m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
            memcpy(regs, REG_DA, sizeof regs);
            want_pc = REG_PC; want_sr = m68k_get_reg(NULL, M68K_REG_SR); want_cycles = GET_CYCLES();
            memcpy(reference, m->chip, FA18_CHIP_SIZE);
            memcpy(reference + FA18_CHIP_SIZE, m->slow, FA18_SLOW_SIZE);
            memcpy(m, before, sizeof *m); m68k_set_context(cpu); SET_CYCLES(100000000);
            fa18_bus_reset();
            int handled = step_oracle_cases[item].step();
            if (!handled || REG_PC != want_pc ||
                m68k_get_reg(NULL, M68K_REG_SR) != want_sr || GET_CYCLES() != want_cycles) {
                fprintf(stderr, "plane step oracle: %06X case %u PC/SR/cycles source %06X/%04X/%d C %06X/%04X/%d\n",
                        pc, scenario, want_pc, want_sr, want_cycles,
                        REG_PC, m68k_get_reg(NULL, M68K_REG_SR), GET_CYCLES()); return 1;
            }
            for (i = 0; i < 16; ++i) if (REG_DA[i] != regs[i]) {
                fprintf(stderr, "plane step oracle: %06X case %u %c%u source %08X C %08X\n",
                        pc, scenario, i < 8 ? 'D' : 'A', i & 7u, regs[i], REG_DA[i]); return 1;
            }
            if (memcmp(reference, m->chip, FA18_CHIP_SIZE) ||
                memcmp(reference + FA18_CHIP_SIZE, m->slow, FA18_SLOW_SIZE)) {
                fprintf(stderr, "plane step oracle: %06X case %u RAM differs\n", pc, scenario); return 1;
            }
            ++matched;
        }
    }
    printf("%s step oracle: %u instructions, %u cases matched registers, SR, PC, cycles and RAM\n",
           group, instructions, matched);
    free(cpu); free(reference); free(before); free(m); return 0;
}
