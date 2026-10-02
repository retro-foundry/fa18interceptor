/* Whole $C1E540 domain/adapter oracle. Fixtures keep source-owned lists
 * terminated and exercise the descriptor, plane, polygon and partition paths.
 * --glue compares original C1C9AE live CPU outputs. Live event timing has
 * separate independent-instruction and full-recording gates. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "m68kcpu.h"
#include "bus.h"
#include "machine.h"
#include "recomp_runtime.h"
#include "recomp_ports.h"
#include "recomp_functions.h"
#include "globals.h"
#include "placement_order.h"

extern int fa18_write_log_active;
extern int64_t fa18_cycle_origin, fa18_next_event;
extern int glue_C1E540(void);

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

static uint32_t random_state = 0xc1e540u;
static uint32_t next_value(void) {
    random_state ^= random_state << 13; random_state ^= random_state >> 17;
    random_state ^= random_state << 5; return random_state;
}

static void fixture(unsigned scenario) {
    unsigned variant = scenario & 31u, seed = scenario >> 5, i, k;
    unsigned count = variant == 0 ? 0 : variant == 1 ? 1 : 2 + seed % 7;
    unsigned anchor = count ? (variant == 2 ? count - 1 : seed % count) : 0;
    unsigned shape = (scenario / 4u) % 3u;
    int height = (seed & 1u) ? -13 : 13;
    m68k_set_reg(M68K_REG_SR, 0x2700u | (scenario & 31u));
    for (i = 0; i < 15; ++i) REG_DA[i] = next_value();
    wr_u16(GRID_ORIGIN_X, 0); wr_u16(GRID_ORIGIN_Z, 0);
    wr_u16(PROJECTION_WORDS, (uint16_t)(seed % 13 - 6));
    wr_u16(PROJECTION_WORDS + 4, (uint16_t)(seed % 17 - 8));
    wr_s32(PROJECTION_Y, height);
    for (i = 0; i < 10; ++i) {
        gaddr entry = 0xc4f6cau + i * 24u;
        gaddr descriptor = 0xc62000u + i * 32u;
        gaddr flags = 0xc64000u + i * 32u;
        gaddr faces = 0xc66000u + i * 128u;
        gaddr planes = 0xc67000u + i * 128u;
        gaddr edges = 0xc68000u + i * 32u;
        gaddr triangle = 0xc69000u + i * 32u;
        gaddr record = CONTROL_RECORDS + (i + 1u) * 512u;
        gaddr workspace = WORKSPACE_RECORDS + (i + 1u) * 32u;
        uint16_t kind = i == anchor ? 0 : (uint16_t)((variant % 3u == 1) ? 0x10u : (variant % 3u == 2) ? 0x40u : 0);
        uint16_t head = (uint16_t)(((i + 1u) << 8) | kind);
        if (i == anchor && variant >= 8) head |= seed % 4u;
        for (k = 0; k < 6; ++k) { wr_u32(entry + k * 4, 0); wr_u32(WORKSPACES + i * 24u + k * 4, next_value()); }
        wr_u16(entry, i < count ? head : 0xffffu);
        wr_u32(entry + 2, descriptor);
        wr_s16(entry + 6, (int16_t)(i * 2u)); wr_s16(entry + 8, (int16_t)(i + 1u));
        wr_s16(entry + 10, (int16_t)(i * 3u)); wr_u16(entry + 14, 0);
        wr_u32(entry + 16, next_value()); wr_u32(entry + 20, next_value());
        wr_u32(descriptor + 4, flags); wr_u32(descriptor + 16, faces);
        for (k = 0; k < 8; ++k) wr_u32(flags + k * 4, 0);
        wr_u16(flags, (variant & 4u) ? 0x4000u : 0x8000u);
        wr_u16(flags + 2, 0); wr_u16(flags + 4, 0);
        wr_u8(flags + 7, i == anchor && variant != 2 ? 0x10u : 0);
        if (shape == 0) {
            wr_u16(faces, 1); wr_u32(faces + 2, (uint32_t)(height + (seed & 3u)));
            wr_u32(faces + 6, planes);
            for (k = 0; k < 12; ++k) wr_s16(planes + k * 2, (int16_t)((int)(next_value() % 41u) - 20));
        } else {
            wr_u16(faces, 0x8001u); wr_u32(faces + 2, shape == 1 ? 0x80000000u : 0);
            wr_u32(faces + 6, edges); wr_u32(faces + 10, triangle); wr_u32(faces + 14, triangle + 8);
            wr_u16(edges, 0); wr_u16(edges + 2, 6); wr_u16(edges + 4, 0x800cu); wr_u16(edges + 6, 0xffffu);
            wr_u16(triangle, 0); wr_u16(triangle + 2, 0); wr_u16(triangle + 4, 6); wr_u16(triangle + 6, 12);
            wr_u16(triangle + 8, 0); wr_u16(triangle + 10, 12); wr_u16(triangle + 12, 6); wr_u16(triangle + 14, 0);
        }
        wr_u16(record + 6, 0); wr_u16(record + 8, 0);
        wr_s16(record + 12, (int16_t)(i * 2u)); wr_s16(record + 14, (int16_t)(i * 3u));
        wr_s32(record + 16, height); wr_u8(record + 4, variant & 8u ? 0xc0u : 0);
        wr_u8(record + 0x7d, seed % 5u);
        for (k = 0; k < 9; ++k) wr_s16(record + 0xa4 + k * 2u, (int16_t)((int)(next_value() % 41u) - 20));
        wr_u16(workspace + 6, 0); wr_u16(workspace + 8, 0);
        wr_s16(workspace + 12, (int16_t)(i * 2u)); wr_s16(workspace + 14, (int16_t)(i * 3u));
        wr_s32(workspace + 16, height);
        if (variant == 3 && i == 0 && i != anchor) wr_s16(entry + 6, 0x4000);
        if (variant == 4 && i == count - 1 && i != anchor) wr_s16(entry + 10, 0x4000);
        if (variant == 5) { wr_u32(descriptor + 4, 0); }
        if (variant == 6) { wr_u16(flags, 0xffffu); }
        if (variant == 7 && i != anchor) wr_u16(entry, head | 15u);
    }
    REG_A[7] = 0xc7ff00u; wr_u32(REG_A[7], 0xc70000u);
    REG_PC = 0xc1e540u; fa18_recomp_abort = 0; SET_CYCLES(100000000);
    fa18_next_event = fa18_cycle_origin - GET_CYCLES() + 1000000;
}

static int source_call(uint32_t return_pc, uint32_t return_sp) {
    unsigned dispatch;
    int result = FA18_RET;
    for (dispatch = 0; dispatch < 10000; ++dispatch) {
        int lo = 0, hi = fa18_recomp_entry_count;
        if ((REG_PC & 0xffffffu) == return_pc && REG_A[7] == return_sp) return result;
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

int main(int argc, char **argv) {
    size_t state_size = 0, rom_size = 0;
    uint8_t *state = read_file("captures/native/demo01/state.bin", &state_size);
    uint8_t *rom = read_file("local/system/kick13.rom", &rom_size);
    FA18Machine *m = calloc(1, sizeof *m), *before = malloc(sizeof *before);
    uint8_t *reference = malloc(FA18_CHIP_SIZE + FA18_SLOW_SIZE);
    unsigned char *cpu = malloc(m68k_context_size());
    char error[256];
    int glue = argc > 1 && !strcmp(argv[argc - 1], "--glue");
    int captured;
    if (glue) --argc;
    captured = argc == 3 && !strcmp(argv[1], "--fixture");
    unsigned count = captured ? 1 : argc > 1 ? (unsigned)strtoul(argv[1], NULL, 10) : 2048, scenario;
    if (!state || !rom || !m || !before || !reference || !cpu || !count) {
        fprintf(stderr, "placement oracle: need sealed state, ROM and nonzero cases\n"); return 1;
    }
    if (!fa18_machine_load_state(m, state, state_size, rom, rom_size, error, sizeof error)) {
        fprintf(stderr, "placement oracle: %s\n", error); return 1;
    }
    free(state); free(rom);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF, NULL); fa18_bus_timing = 0;
    for (scenario = 0; scenario < count; ++scenario) {
        unsigned i;
        uint32_t return_pc, return_sp;
        uint32_t reference_registers[16], reference_sr;
        int result;
        if (captured) {
            uint32_t registers[18];
            FILE *file = fopen(argv[2], "rb");
            if (!file || fread(registers, sizeof registers, 1, file) != 1
                || fread(m->chip, FA18_CHIP_SIZE, 1, file) != 1
                || fread(m->slow, FA18_SLOW_SIZE, 1, file) != 1 || fgetc(file) != EOF || fclose(file)) {
                fprintf(stderr, "placement oracle: invalid captured entry fixture\n"); return 1;
            }
            m68k_set_reg(M68K_REG_SR, registers[16]);
            for (i = 0; i < 16; ++i) REG_DA[i] = registers[i];
            REG_PC = registers[17]; fa18_recomp_abort = 0; SET_CYCLES(100000000);
            fa18_next_event = fa18_cycle_origin - GET_CYCLES() + 1000000;
        } else fixture(scenario);
        return_pc = rd_u32(REG_A[7]) & 0xffffffu; return_sp = REG_A[7] + 4;
        memcpy(before, m, sizeof *m); m68k_get_context(cpu);
        fa18_write_log_active = 1;
        result = source_call(return_pc, return_sp);
        if (result != FA18_RET || (REG_PC & 0xffffffu) != return_pc || REG_A[7] != return_sp) {
            fprintf(stderr, "placement oracle: case %u source return %d PC %06X\n", scenario, result, REG_PC); return 1;
        }
        memcpy(reference, m->chip, FA18_CHIP_SIZE);
        memcpy(reference + FA18_CHIP_SIZE, m->slow, FA18_SLOW_SIZE);
        for (i = 0; i < 16; ++i) reference_registers[i] = REG_DA[i];
        reference_sr = m68k_get_reg(NULL, M68K_REG_SR);
        memcpy(m, before, sizeof *m); m68k_set_context(cpu);
        if (glue) glue_C1E540(); else order_placement_cache();
        fa18_write_log_active = 0;
        if (glue) {
            /* Original C1C9AE mask: D5 low, D6/D7 full, A1-A7 full;
             * CCR and other registers are dead. SR control remains live. */
            for (i = 5; i < 16; ++i) {
                uint32_t mask = i == 5 ? 0xFFFFu : 0xFFFFFFFFu;
                if (i == 8) continue;
                if ((REG_DA[i] & mask) != (reference_registers[i] & mask)) {
                    fprintf(stderr, "placement oracle: case %u %c%u source %08X C %08X\n",
                            scenario, i < 8 ? 'D' : 'A', i & 7u, reference_registers[i], REG_DA[i]); return 1;
                }
            }
            if ((REG_PC & 0xFFFFFFu) != return_pc
                || ((m68k_get_reg(NULL, M68K_REG_SR) ^ reference_sr) & 0xFF00u)) {
                fprintf(stderr, "placement oracle: case %u PC/SR control mismatch\n", scenario); return 1;
            }
        }
        for (i = 0; i < FA18_CHIP_SIZE + FA18_SLOW_SIZE; ++i) {
            uint8_t got = i < FA18_CHIP_SIZE ? m->chip[i] : m->slow[i - FA18_CHIP_SIZE];
            gaddr address = i < FA18_CHIP_SIZE ? i : i - FA18_CHIP_SIZE + FA18_SLOW_BASE;
            /* Original private frame, child save slots and return words are dead. */
            if (address >= return_sp - 0x94u && address < return_sp) continue;
            if (got != reference[i]) {
                fprintf(stderr, "placement oracle: case %u byte %06X source %02X C %02X\n", scenario, address, reference[i], got); return 1;
            }
        }
    }
    printf("placement oracle: %u complete C1E540 %s calls matched original Chip/Slow RAM outside dead private stack%s\n",
           count, glue ? "adapter" : "domain", glue ? " and original C1C9AE live CPU outputs" : "");
    free(cpu); free(reference); free(before); free(m); return 0;
}
