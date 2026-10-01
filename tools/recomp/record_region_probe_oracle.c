/* Adversarial structural fixtures for $C2B05A, evaluated by the original
 * translated instructions. These are validation inputs, not game defaults. */
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

extern int glue_C2B05A(void);
extern int64_t fa18_next_event;

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

static uint32_t random_state = 0x18c2b05au;
static uint32_t next_value(void) {
    random_state ^= random_state << 13;
    random_state ^= random_state >> 17;
    random_state ^= random_state << 5;
    return random_state;
}

static void fixture(unsigned scenario) {
    static const int16_t outlines[3][4][2] = {
        {{0, 0}, {64, 0}, {64, 64}, {0, 64}},
        {{0, 0}, {64, 8}, {32, 64}, {0, 32}},
        {{-32, -16}, {64, 0}, {96, 64}, {0, 32}}
    };
    const int16_t (*vertices)[2] = outlines[scenario % 3u];
    gaddr group = 0xc43e6cu;
    unsigned i, j;
    uint32_t x = ((scenario % 97u) << 12) + (scenario % 4u) * 0x100u;
    uint32_t y = ((scenario * 17u) % 97u) << 12;
    uint16_t directory_index;
    if (scenario % 19u == 0) x = 0u - x;
    if (scenario % 23u == 0) y = 0u - y;
    directory_index = (uint16_t)(((x >> 24) * 2u) + ((y >> 24) << 6));
    wr_u16(0xc459b6u, 0);
    wr_u32(0xc46198u, x);
    wr_u32(0xc461a0u, y);
    wr_u8(0xc46188u, (uint8_t)next_value());
    wr_u16(0xc42e6cu + (gaddr)(int32_t)(int16_t)directory_index,
           scenario % 17u == 0 ? 0 : scenario % 29u == 0 ? 0xffff : 0x1000);
    wr_u16(group, scenario % 7u == 0 ? 0xffff : 0);
    wr_u16(group + 2, 0);
    wr_u16(group + 4, 4);
    for (i = 0; i < 4; ++i) {
        j = scenario & 1u ? 3u - i : i;
        wr_s16(group + 6 + 4u * i, vertices[j][0]);
        wr_s16(group + 8 + 4u * i, vertices[j][1]);
    }
    wr_u16(group + 22, 0xffff);
    /* A valid placement and then the source's five-word terminator. */
    wr_u16(0xc42a96u, 0);
    wr_u16(0xc42a98u, 0);
    wr_s16(0xc42a9au, scenario % 3u == 0 ? -1 : 0);
    wr_u16(0xc42a9cu, 0);
    wr_s16(0xc42a9eu, (int16_t)(scenario % 33u));
    wr_s16(0xc42aa0u, (int16_t)(scenario % 41u));
    for (i = 0; i < 5; ++i) wr_s16(0xc42aa8u + 2u * i, (int16_t)next_value());
    wr_u16(0xc42aa8u, 0xffff);
    if (scenario % 11u == 0) wr_u16(0xc42a98u, 0xffff);
    wr_s16(0xc1d7e2u, (int16_t)((scenario % 9u) - 4));
    wr_s16(0xc1d7e4u, (int16_t)((scenario % 13u) - 6));
    wr_u32(0xc22198u, scenario % 5u == 0 ? 0 : 0xc61000u);
    for (i = 0; i < 4; ++i) {
        j = scenario & 2u ? 3u - i : i;
        wr_s16(0xc61000u + 4u * i, vertices[j][0]);
        wr_s16(0xc61002u + 4u * i, vertices[j][1]);
    }
    wr_u16(0xc61010u, 0xffff);
    for (i = 0; i < 15; ++i) REG_DA[i] = next_value();
    REG_A[7] = 0xc7ff00u;
    wr_u32(REG_A[7], 0xc70000u);
    REG_PC = 0xc2b05au;
    fa18_recomp_abort = 0;
    fa18_next_event = INT64_MAX;
    SET_CYCLES(100000000);
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
    if (!state || !rom || !m || !before || !reference || !cpu || !count) {
        fprintf(stderr, "region oracle: need demo01 state, Kickstart ROM, memory and nonzero case count\n");
        return 1;
    }
    if (!fa18_machine_load_state(m, state, state_size, rom, rom_size, error, sizeof error)) {
        fprintf(stderr, "region oracle: %s\n", error);
        return 1;
    }
    free(state);
    free(rom);
    fa18_recomp_init(1);
    fa18_ports_init(FA18_PORTS_OFF, NULL);
    for (entry = 0; entry < fa18_recomp_entry_count; ++entry)
        if (fa18_recomp_entries[entry].pc == 0xc2b05au)
            entry_label = (int)fa18_recomp_entries[entry].label;
    if (entry_label < 0) {
        fprintf(stderr, "region oracle: original entry C2B05A is missing\n");
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
        result = fa18_fn_C2B05A(entry_label);
        if (result != FA18_RET) {
            fprintf(stderr, "region oracle: case %u source did not return (result %d, PC %06X)\n",
                    scenario, result, REG_PC);
            return 1;
        }
        for (i = 0; i < 16; ++i) registers[i] = REG_DA[i];
        pc = REG_PC;
        memcpy(reference, m->chip, FA18_CHIP_SIZE);
        memcpy(reference + FA18_CHIP_SIZE, m->slow, FA18_SLOW_SIZE);
        memcpy(m, before, sizeof *m);
        m68k_set_context(cpu);
        result = glue_C2B05A();
        if (result != FA18_RET || REG_PC != pc) {
            fprintf(stderr, "region oracle: case %u glue did not return to %06X\n", scenario, pc);
            return 1;
        }
        for (i = 0; i < 16; ++i) {
            if (REG_DA[i] != registers[i]) {
                fprintf(stderr, "region oracle: case %u %c%u source %08X glue %08X\n",
                        scenario, i < 8 ? 'D' : 'A', i & 7u, registers[i], REG_DA[i]);
                return 1;
            }
        }
        for (i = 0; i < FA18_CHIP_SIZE + FA18_SLOW_SIZE; ++i) {
            uint8_t got = i < FA18_CHIP_SIZE ? m->chip[i] : m->slow[i - FA18_CHIP_SIZE];
            gaddr address = i < FA18_CHIP_SIZE ? i : i - FA18_CHIP_SIZE + FA18_SLOW_BASE;
            if (address < REG_A[7] && address >= REG_A[7] - 0x1000u) continue;
            if (got != reference[i]) {
                fprintf(stderr, "region oracle: case %u byte %06X source %02X glue %02X\n",
                        scenario, address, reference[i], got);
                return 1;
            }
        }
    }
    printf("region oracle: %u cases matched original registers and RAM\n", count);
    free(cpu);
    free(reference);
    free(before);
    free(m);
    return 0;
}
