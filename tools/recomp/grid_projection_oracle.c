/* Adversarial structural fixtures for $C279D0, evaluated by the original
 * translated instructions. Custom writes are held in both executions; live
 * DMA and timing are proved separately by recording and instruction gates.
 * These are validation inputs, not game defaults. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "m68kcpu.h"
#include "bus.h"
#include "machine.h"
#include "recomp_runtime.h"
#include "recomp_ports.h"
#include "recomp_functions.h"
#include "memory.h"

extern int glue_C279D0(void);
extern int fa18_write_log_active;
extern int64_t fa18_cycle_origin, fa18_next_event;

static uint8_t *read_file(const char *path, size_t *size) {
    FILE *f = fopen(path, "rb");
    long length;
    uint8_t *data;
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) || (length = ftell(f)) < 0 || fseek(f, 0, SEEK_SET)) {
        fclose(f);
        return NULL;
    }
    data = malloc((size_t)length);
    if (!data || fread(data, 1, (size_t)length, f) != (size_t)length) {
        free(data);
        fclose(f);
        return NULL;
    }
    fclose(f);
    *size = (size_t)length;
    return data;
}

static uint32_t random_state = 0x18c279d0u;
static uint32_t next_value(void) {
    random_state ^= random_state << 13;
    random_state ^= random_state >> 17;
    random_state ^= random_state << 5;
    return random_state;
}

static void fixture(unsigned scenario) {
    static const int16_t depths[] = {-4609,-4608,-3457,-3456,-2049,-2048,-513,-512,
                                    -225,-224,-129,-128,-64,-1,0,1,127};
    static const int16_t kinds[] = {-12,-16,-20,0,1,2,3};
    int16_t depth = depths[scenario % (sizeof depths / sizeof depths[0])];
    unsigned shift = depth >= -128 ? 3 : 0;
    gaddr table = depth >= -128 ? 0xc28124u : depth >= -224 ? 0xc28368u : 0xc2854cu;
    gaddr pair_table = shift ? 0xc286dcu : 0xc286d0u;
    unsigned i, j, count = scenario % 5u;
    int16_t scaled = (int16_t)((uint16_t)depth << shift);
    wr_u8(0xc4586bu, (uint8_t)(scenario / 17u % 4u));
    wr_s32(0xc45a78u, depth);
    wr_s16(0xc45a72u, (int16_t)((scenario % 9u) * 256 - 1024));
    wr_s16(0xc45a76u, (int16_t)((scenario / 9u % 9u) * 256 - 1024));
    wr_s16(table, scenario % 37u == 0 ? (int16_t)0x8000 : (int16_t)count); wr_s16(table + 2, (int16_t)(scenario % 3u));
    for (i = 0; i < 1024; ++i) wr_u8(0xc27d24u + i, (uint8_t)(scenario % 4u));
    for (i = 0; i < 4; ++i) {
        wr_s16(table + 4 + i * 6, (int16_t)((scenario / 4u % 5u) * 256 - 512));
        wr_s16(table + 6 + i * 6, (int16_t)((scenario / 20u % 5u) * 256 - 512));
        wr_s16(table + 8 + i * 6, kinds[(scenario / 3u + i) % 7u]);
    }
    for (i = 0; i < 9; ++i) wr_s16(0xc45bd8u + 2u * i, 0);
    wr_s16(0xc45bd8u, 256); wr_s16(0xc45be2u, 256);
    wr_s16(0xc45be6u, scaled < 0 ? -256 : 256);
    /* Tilted projections, word overflow and coordinates on the clamp boundary. */
    if (scenario % 11u == 0) {
        for (i = 0; i < 9; ++i) wr_s16(0xc45bd8u + 2u * i, (int16_t)next_value());
    }
    for (i = 0; i < 3; ++i) for (j = 0; j < 3; ++j) {
        wr_s16(pair_table + 24u * i + 4u * j, j == 2 && scenario % 2u ? 16000 : 0);
        wr_s16(pair_table + 24u * i + 4u * j + 2, (int16_t)(j * 8));
    }
    wr_s16(0xc45984u, (int16_t)(scenario % 13u == 0 ? 0 : 179));
    for (i = 0; i < 6; ++i) wr_u16(0xc4b392u + 2u * i, (uint16_t)(0x6d00u + i));
    for (i = 0; i < 15; ++i) REG_DA[i] = next_value();
    REG_A[7] = 0xc7ff00u; wr_u32(REG_A[7], 0xc70000u);
    REG_PC = 0xc279d0u; fa18_recomp_abort = 0; fa18_next_event = INT64_MAX;
    SET_CYCLES(100000000);
    fa18_next_event = fa18_cycle_origin - GET_CYCLES() + 1000000;
}

int main(int argc, char **argv) {
    size_t state_size = 0, rom_size = 0;
    uint8_t *state = read_file("captures/native/demo01/state.bin", &state_size);
    uint8_t *rom = read_file("local/system/kick13.rom", &rom_size);
    FA18Machine *m = calloc(1, sizeof *m), *before = malloc(sizeof *before);
    uint8_t *reference = malloc(FA18_CHIP_SIZE + FA18_SLOW_SIZE);
    unsigned char *cpu = malloc(m68k_context_size());
    char error[256];
    unsigned count = argc > 1 ? (unsigned)strtoul(argv[1], NULL, 10) : 512, scenario;
    int entry_label = -1, entry;
    unsigned partial = 0, clamped = 0;
    if (!state || !rom || !m || !before || !reference || !cpu || !count) {
        fprintf(stderr, "grid oracle: need demo01 state, Kickstart ROM, memory and nonzero case count\n");
        return 1;
    }
    if (!fa18_machine_load_state(m, state, state_size, rom, rom_size, error, sizeof error)) {
        fprintf(stderr, "grid oracle: %s\n", error);
        return 1;
    }
    free(state);
    free(rom);
    fa18_recomp_init(1);
    fa18_ports_init(FA18_PORTS_OFF, NULL);
    for (entry = 0; entry < fa18_recomp_entry_count; ++entry)
        if (fa18_recomp_entries[entry].pc == 0xc279d0u)
            entry_label = (int)fa18_recomp_entries[entry].label;
    if (entry_label < 0) {
        fprintf(stderr, "grid oracle: original entry C279D0 is missing\n");
        return 1;
    }
    fa18_bus_timing = 0;
    for (scenario = 0; scenario < count; ++scenario) {
        uint32_t registers[16], pc;
        unsigned i;
        int result;
        fixture(scenario);
        memcpy(before, m, sizeof *m);
        m68k_get_context(cpu);
        fa18_write_log_active = 1; /* Hold custom writes in both executions. */
        result = fa18_fn_C279D0(entry_label);
        /* Pixel writers end through a computed JMP. Dispatch original labels
         * until the outer return, without changing validation timing inputs. */
        for (i = 0; result != FA18_EXIT_INTERP &&
                    !(REG_PC == 0xc70000u && REG_A[7] == 0xc7ff04u) && i < 100000; ++i) {
            int lo = 0, hi = fa18_recomp_entry_count;
            while (lo < hi) {
                int mid = lo + (hi - lo) / 2;
                if (fa18_recomp_entries[mid].pc < REG_PC) lo = mid + 1; else hi = mid;
            }
            if (lo == fa18_recomp_entry_count || fa18_recomp_entries[lo].pc != REG_PC) break;
            result = fa18_recomp_functions[fa18_recomp_entries[lo].function].fn(
                (int)fa18_recomp_entries[lo].label);
        }
        if (result != FA18_RET || REG_PC != 0xc70000u || REG_A[7] != 0xc7ff04u) {
            fprintf(stderr, "grid oracle: case %u source did not return (result %d, PC %06X)\n",
                    scenario, result, REG_PC);
            return 1;
        }
        if (rd_u16(0xc4b392u) != 0x6d00u && rd_u16(0xc4b39au) == 0x6d04u) ++partial;
        for (i = 0; i < 3; ++i) {
            uint16_t x = rd_u16(0xc4b392u + 4u * i), y = rd_u16(0xc4b394u + 4u * i);
            if (x == 0 || x == 319 || y == 0 || y == 179) { ++clamped; break; }
        }
        for (i = 0; i < 16; ++i) registers[i] = REG_DA[i];
        pc = REG_PC;
        memcpy(reference, m->chip, FA18_CHIP_SIZE);
        memcpy(reference + FA18_CHIP_SIZE, m->slow, FA18_SLOW_SIZE);
        memcpy(m, before, sizeof *m);
        m68k_set_context(cpu);
        result = glue_C279D0();
        fa18_write_log_active = 0;
        if (result != FA18_RET || REG_PC != pc) {
            fprintf(stderr, "grid oracle: case %u glue did not return to %06X\n", scenario, pc);
            return 1;
        }
        for (i = 0; i < 16; ++i) {
            if (REG_DA[i] != registers[i]) {
                fprintf(stderr, "grid oracle: case %u %c%u source %08X glue %08X\n",
                        scenario, i < 8 ? 'D' : 'A', i & 7u, registers[i], REG_DA[i]);
                return 1;
            }
        }
        for (i = 0; i < FA18_CHIP_SIZE + FA18_SLOW_SIZE; ++i) {
            uint8_t got = i < FA18_CHIP_SIZE ? m->chip[i] : m->slow[i - FA18_CHIP_SIZE];
            gaddr address = i < FA18_CHIP_SIZE ? i : i - FA18_CHIP_SIZE + FA18_SLOW_BASE;
            if (address < REG_A[7] && address >= REG_A[7] - 0x1000u) continue;
            if (got != reference[i]) {
                fprintf(stderr, "grid oracle: case %u byte %06X source %02X glue %02X\n",
                        scenario, address, reference[i], got);
                return 1;
            }
        }
    }
    if (count >= 4096 && (!partial || !clamped)) {
        fprintf(stderr, "grid oracle: expected partial-write and edge-clamp fixtures\n");
        return 1;
    }
    printf("grid oracle: %u cases matched original registers and RAM (%u partial writes, %u edge clamps)\n",
           count, partial, clamped);
    free(cpu);
    free(reference);
    free(before);
    free(m);
    return 0;
}
