/* Complete parent oracle. Original instructions independently evaluate
 * both entries and source-format lists, including real descriptor children. */
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
    static const int32_t depths[] = {0, 1, -1, -32767, -32768, -32769,
                                    32767, 32768, (int32_t)0x80000000u, 0x7fffffff};
    static const int16_t caches[] = {-1, 0, 1, 255, 256, 1023, 1024, 32767, -32768};
    static const uint8_t countdowns[] = {0, 1, 2, 127, 128, 129, 255};
    static const gaddr routines[] = {0xc096bcu, 0xc096cau, 0xc1ed3cu, 0xc1ed48u, 0xc2f490u};
    unsigned i, bank, count = (scenario / 2) % 6u;
    uint16_t offset = (scenario & 8u) ? 24 : 0;
    wr_u16(0xc459acu, offset); wr_u16(0xc459aeu, offset);
    wr_s32(PROJECTION_Y, depths[(scenario / 12) % 10u]);
    wr_s32(POSITION_BIAS, scenario & 16u ? -0x800001 : scenario & 32u ? -0x380001 : -0x27ffff);
    wr_u16(PROJECTION_WORDS, (uint16_t)random_value());
    wr_u16(PROJECTION_WORDS + 4, (uint16_t)random_value());
    wr_u16(0xc458dau, (uint16_t)(scenario >> 2));
    wr_u8(0xc458bcu, (uint8_t)random_value());
    for (bank = 0; bank < 2; ++bank) {
        gaddr base = bank ? 0xc4f03au : 0xc4e9aau;
        for (i = 0; i < count; ++i) {
            gaddr record = base + offset + i * 24u;
            gaddr descriptor = 0xc61000u + (bank * 6u + i) * 24u;
            unsigned j, kind = (scenario / 64u + i) % 5u;
            for (j = 0; j < 24; ++j) wr_u8(record + j, (uint8_t)random_value());
            { uint16_t header = (uint16_t)(((scenario >> 4) & 255u) << 8 |
                                  (random_value() & 0xf0u) | ((scenario + i) & 15u));
              if (header == 0xffffu) header ^= 0x10u;
              wr_u16(record, header); }
            wr_u32(record + 2, descriptor);
            for (j = 0; j < 3; ++j) wr_s16(record + 6 + 2 * j, (int16_t)(random_value() % 2048u - 1024));
            wr_s16(record + 16, caches[(scenario / 128u + i) % 9u]);
            wr_u8(record + 18, countdowns[(scenario / 256u + i) % 7u]);
            wr_u16(record + 20, scenario & 4u ? 0xffffu : 0x400u);
            wr_u32(descriptor, scenario % 19u == 0 ? 0xffffffffu : routines[kind]);
            wr_u32(descriptor + 4, 0xc62000u); wr_u32(descriptor + 8, 0xc62100u);
            wr_u32(descriptor + 12, random_value());
        }
        wr_u16(base + offset + count * 24u, 0xffffu);
    }
    wr_u16(0xc62100u, 0xffffu);
    for (i = 0; i < 15; ++i) REG_DA[i] = random_value();
    REG_A[7] = 0xc7ff00u; wr_u32(REG_A[7], 0xc70000u);
    m68k_set_reg(M68K_REG_SR, 0x2700u | (scenario & 31u));
    REG_PC = scenario & 1u ? 0xc1cb26u : 0xc1cb14u;
    fa18_recomp_abort = 0; fa18_next_event = INT64_MAX; SET_CYCLES(100000000);
}
int main(int argc, char **argv) {
    size_t state_size = 0, rom_size = 0;
    uint8_t *state = read_file("captures/native/demo01/state.bin", &state_size);
    uint8_t *rom = read_file("local/system/kick13.rom", &rom_size);
    FA18Machine *m = calloc(1, sizeof *m), *base = malloc(sizeof *base), *before = malloc(sizeof *before);
    uint8_t *reference = malloc(FA18_CHIP_SIZE + FA18_SLOW_SIZE);
    void *cpu = malloc(m68k_context_size()); char error[256];
    unsigned cases = argc > 1 ? (unsigned)strtoul(argv[1], NULL, 10) : 8192, scenario;
    if (!state || !rom || !m || !base || !before || !reference || !cpu || !cases) return 1;
    if (!fa18_machine_load_state(m, state, state_size, rom, rom_size, error, sizeof error)) {
        fprintf(stderr, "scene placement oracle: %s\n", error); return 1;
    }
    free(state); free(rom); memcpy(base, m, sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF, NULL); fa18_bus_timing = 0;
    for (scenario = 0; scenario < cases; ++scenario) {
        uint32_t regs[16], entry, sr; unsigned i;
        memcpy(m, base, sizeof *m); fixture(scenario); entry = REG_PC;
        memcpy(before, m, sizeof *m); m68k_get_context(cpu); fa18_write_log_active = 1;
        if (source_call(0xc70000u, 0xc7ff04u) != FA18_RET) {
            fprintf(stderr, "scene placement oracle: case %u source did not return at %06X\n", scenario, REG_PC); return 1;
        }
        memcpy(regs, REG_DA, sizeof regs); sr = m68k_get_reg(NULL, M68K_REG_SR);
        memcpy(reference, m->chip, FA18_CHIP_SIZE);
        memcpy(reference + FA18_CHIP_SIZE, m->slow, FA18_SLOW_SIZE);
        memcpy(m, before, sizeof *m); m68k_set_context(cpu);
        if (entry == 0xc1cb14u) glue_C1CB14(); else glue_C1CB26();
        fa18_write_log_active = 0;
        for (i = 0; i < 16; ++i) if (regs[i] != REG_DA[i]) {
            fprintf(stderr, "scene placement oracle: case %u %c%u source %08X C %08X\n",
                scenario, i < 8 ? 'D' : 'A', i & 7u, regs[i], REG_DA[i]); return 1;
        }
        if (REG_PC != 0xc70000u || ((sr ^ m68k_get_reg(NULL, M68K_REG_SR)) & 0xff00u)) return 1;
        for (i = 0; i < FA18_CHIP_SIZE + FA18_SLOW_SIZE; ++i) {
            gaddr address = i < FA18_CHIP_SIZE ? i : i - FA18_CHIP_SIZE + FA18_SLOW_BASE;
            uint8_t got = i < FA18_CHIP_SIZE ? m->chip[i] : m->slow[i - FA18_CHIP_SIZE];
            /* Parent has no locals. Empty renderer callbacks use their
             * original LINK -152 frame and one child return: 160 bytes. */
            if (address >= 0xc7ff00u - 160 && address < 0xc7ff00u) continue;
            if (got != reference[i]) {
                fprintf(stderr, "scene placement oracle: case %u byte %06X source %02X C %02X\n",
                    scenario, address, reference[i], got); return 1;
            }
        }
    }
    printf("scene placement oracle: %u complete calls matched all registers, PC, SR control and RAM outside 160-byte child stack\n", cases);
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
