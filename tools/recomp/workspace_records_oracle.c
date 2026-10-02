/* Complete workspace selector helpers versus original generated instructions.
 * These entries are cold in sealed recordings, so test all selector bytes,
 * CCR combinations, signed-word bounds and 32-bit addition overflow here. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "m68kcpu.h"
#include "bus.h"
#include "machine.h"
#include "recomp_runtime.h"
#include "recomp_ports.h"
#include "recomp_functions.h"
#include "ports_glue.h"
#include "memory.h"

extern int fa18_write_log_active;
extern int64_t fa18_cycle_origin, fa18_next_event;

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

static uint32_t random_state = 0xc1ebb084u;
static uint32_t next_value(void) {
    random_state ^= random_state << 13;
    random_state ^= random_state >> 17;
    random_state ^= random_state << 5;
    return random_state;
}

static void fixture(uint32_t pc, unsigned scenario) {
    static const uint16_t words[] = {0,1,0x7fff,0x8000,0xffff,0x00ff,0x0100,0xff00};
    static const uint32_t longs[] = {0,1,0xffffffffu,0x7fffffffu,0x80000000u,0xffffc000u};
    unsigned selector = scenario & 255u, variant = scenario >> 8, i;
    gaddr record = 0xc48184u + 32u * selector;
    uint16_t packed = (uint16_t)((selector << 8) | (next_value() & 255u));
    m68k_set_reg(M68K_REG_SR, 0x2700u | (variant & 31u));
    for (i = 0; i < 15; ++i) REG_DA[i] = next_value();
    REG_D[1] = (REG_D[1] & 0xffff0000u) | packed;
    REG_D[2] = longs[variant % 6u]; REG_D[4] = longs[(variant / 6u) % 6u];
    REG_A[1] = 0xc61000u; REG_A[6] = 0xc62080u;
    wr_u16(REG_A[1], packed);
    wr_u16(REG_A[6] - 0x1c, words[variant % 8u]);
    wr_u16(REG_A[6] - 0x1e, words[(variant / 8u) % 8u]);
    wr_u16(record + 6, (uint16_t)next_value());
    wr_u16(record + 8, (uint16_t)next_value());
    wr_u16(record + 0xc, words[variant % 8u]);
    wr_u16(record + 0xe, words[(variant / 8u) % 8u]);
    wr_u32(record + 0x10, longs[variant % 6u]);
    REG_A[7] = 0xc7ff00u; wr_u32(REG_A[7], 0xc70000u);
    REG_PC = pc; fa18_recomp_abort = 0; SET_CYCLES(100000000);
    fa18_next_event = fa18_cycle_origin - GET_CYCLES() + 1000000;
}

static int source_call(void) {
    unsigned dispatch;
    int result = FA18_RET;
    /* Both helpers branch into tails shared with their control-record variants. */
    for (dispatch = 0; dispatch < 32; ++dispatch) {
        int lo = 0, hi = fa18_recomp_entry_count;
        if (REG_PC == 0xc70000u && REG_A[7] == 0xc7ff04u) return result;
        while (lo < hi) {
            int mid = lo + (hi - lo) / 2;
            if (fa18_recomp_entries[mid].pc < REG_PC) lo = mid + 1; else hi = mid;
        }
        if (lo == fa18_recomp_entry_count || fa18_recomp_entries[lo].pc != REG_PC)
            return FA18_EXIT_INTERP;
        result = fa18_recomp_functions[fa18_recomp_entries[lo].function].fn(
            (int)fa18_recomp_entries[lo].label);
        if (result == FA18_EXIT_INTERP) return result;
    }
    return FA18_EXIT_INTERP;
}

int main(int argc, char **argv) {
    static const struct { uint32_t pc; int (*glue)(void); } entries[] = {
        {0xc1ebb0u, glue_C1EBB0}, {0xc1ec84u, glue_C1EC84}
    };
    size_t state_size = 0, rom_size = 0;
    uint8_t *state = read_file("captures/native/demo01/state.bin", &state_size);
    uint8_t *rom = read_file("local/system/kick13.rom", &rom_size);
    FA18Machine *m = calloc(1, sizeof *m), *before = malloc(sizeof *before);
    uint8_t *reference = malloc(FA18_CHIP_SIZE + FA18_SLOW_SIZE);
    unsigned char *cpu = malloc(m68k_context_size());
    char error[256];
    unsigned count = argc > 1 ? (unsigned)strtoul(argv[1], NULL, 10) : 16384, e, scenario;
    if (!state || !rom || !m || !before || !reference || !cpu || !count) {
        fprintf(stderr, "workspace oracle: need sealed state, ROM and nonzero case count\n");
        return 1;
    }
    if (!fa18_machine_load_state(m, state, state_size, rom, rom_size, error, sizeof error)) {
        fprintf(stderr, "workspace oracle: %s\n", error); return 1;
    }
    free(state); free(rom);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF, NULL); fa18_bus_timing = 0;
    for (e = 0; e < sizeof entries / sizeof entries[0]; ++e) {
        for (scenario = 0; scenario < count; ++scenario) {
            uint32_t registers[16], pc, sr;
            unsigned i;
            int result;
            fixture(entries[e].pc, scenario);
            memcpy(before, m, sizeof *m); m68k_get_context(cpu);
            fa18_write_log_active = 1; /* Equal held machine events. */
            result = source_call();
            if (result != FA18_RET || REG_PC != 0xc70000u || REG_A[7] != 0xc7ff04u) {
                fprintf(stderr, "workspace oracle: %06X case %u source return %d PC %06X\n",
                        entries[e].pc, scenario, result, REG_PC); return 1;
            }
            for (i = 0; i < 16; ++i) registers[i] = REG_DA[i];
            pc = REG_PC; sr = m68k_get_reg(NULL, M68K_REG_SR);
            memcpy(reference, m->chip, FA18_CHIP_SIZE);
            memcpy(reference + FA18_CHIP_SIZE, m->slow, FA18_SLOW_SIZE);
            memcpy(m, before, sizeof *m); m68k_set_context(cpu);
            result = entries[e].glue(); fa18_write_log_active = 0;
            if (result != FA18_RET || REG_PC != pc || m68k_get_reg(NULL, M68K_REG_SR) != sr) {
                fprintf(stderr, "workspace oracle: %06X case %u return/SR source %06X/%04X C %06X/%04X\n",
                        entries[e].pc, scenario, pc, sr, REG_PC,
                        m68k_get_reg(NULL, M68K_REG_SR)); return 1;
            }
            for (i = 0; i < 16; ++i) if (REG_DA[i] != registers[i]) {
                fprintf(stderr, "workspace oracle: %06X case %u %c%u source %08X C %08X\n",
                        entries[e].pc, scenario, i < 8 ? 'D' : 'A', i & 7u,
                        registers[i], REG_DA[i]); return 1;
            }
            for (i = 0; i < FA18_CHIP_SIZE + FA18_SLOW_SIZE; ++i) {
                uint8_t got = i < FA18_CHIP_SIZE ? m->chip[i] : m->slow[i - FA18_CHIP_SIZE];
                gaddr address = i < FA18_CHIP_SIZE ? i : i - FA18_CHIP_SIZE + FA18_SLOW_BASE;
                /* Only the source's saved A2 and return word are dead on return. */
                if (address >= 0xc7fefcu && address < 0xc7ff04u) continue;
                if (got != reference[i]) {
                    fprintf(stderr, "workspace oracle: %06X case %u byte %06X source %02X C %02X\n",
                            entries[e].pc, scenario, address, reference[i], got); return 1;
                }
            }
        }
        printf("workspace oracle: %06X %u complete calls matched original registers, full SR, PC and RAM\n",
               entries[e].pc, count);
    }
    free(cpu); free(reference); free(before); free(m); return 0;
}
