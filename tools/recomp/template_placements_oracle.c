/* Complete C1D10C domain oracle. Original instructions compute expected
 * memory independently. Case zero replays a recorded entry; later cases
 * install terminated, source-format tables to cover cold selector paths,
 * cache overflow, flagged filing and linked descriptor packet copies. */
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
#include "memory.h"
#include "template_placements.h"

extern int fa18_write_log_active;
extern int64_t fa18_cycle_origin, fa18_next_event;

static uint8_t *read_file(const char *path, size_t *size) {
    FILE *file = fopen(path, "rb");
    long length;
    uint8_t *bytes;
    if (!file) return NULL;
    if (fseek(file, 0, SEEK_END) || (length = ftell(file)) < 0 || fseek(file, 0, SEEK_SET)) {
        fclose(file); return NULL;
    }
    bytes = malloc((size_t)length);
    if (!bytes || fread(bytes, 1, (size_t)length, file) != (size_t)length || fclose(file)) {
        free(bytes); return NULL;
    }
    *size = (size_t)length; return bytes;
}

static uint32_t random_state = 0xc1d10cu;
static uint32_t next_value(void) {
    random_state ^= random_state << 13; random_state ^= random_state >> 17;
    random_state ^= random_state << 5; return random_state;
}

static void section(gaddr address, uint32_t next, unsigned vertices) {
    unsigned i;
    for (i = 0; i < 5; ++i) wr_u16(address + i * 2, (uint16_t)next_value());
    wr_u32(address + 10, next); wr_u16(address + 14, (uint16_t)vertices);
    for (i = 0; i < vertices * 6; ++i) wr_u16(address + 16 + i * 2, (uint16_t)next_value());
}

static void structural_fixture(unsigned scenario) {
    static const gaddr roots[] = {0xc4124eu, 0xc414b0u, 0xc42092u};
    static const gaddr directories[] = {TEMPLATE_SELECTOR_X, TEMPLATE_SELECTOR_Y, TEMPLATE_SELECTOR_Z};
    static const gaddr gates[] = {TEMPLATE_GATES_X, TEMPLATE_GATES_Y, TEMPLATE_GATES_Z};
    unsigned variant = (scenario - 1u) & 63u, seed = (scenario - 1u) >> 6;
    unsigned items = variant % 6u, bands = variant & 8u ? (seed & 16u ? 14 : 2) : 1;
    unsigned selectors = variant & 16u ? 16 : 1u + seed % 15u;
    gaddr cell = 0xc42a00u, stream = 0xc65000u, controls = 0xc43a00u, cursor;
    unsigned i, j, k;
    uint8_t checks = variant & 1u;
    /* All selector values index real source maps; the three directory roots
     * point to a common valid stream in this structural fixture only. */
    wr_u8(CELL_CHECKS, checks ? (seed & 8u ? 0x80 : 1) : 0);
    wr_u8(0xc45865u, (variant >> 1) & 1u);
    wr_u8(0xc45786u, (variant >> 2) & 1u);
    wr_u8(0xc45866u, seed & 2u ? (seed & 8u ? 0x80 : 1) : 0);
    wr_s32(0xc45a66u, seed & 1u ? -0xa000 : -0x9fff);
    wr_u8(0xc45850u, seed & 15u); wr_u8(0xc45851u, (seed >> 2) & 15u);
    wr_u8(0xc45854u, seed & 7u);
    for (i = 0; i < 4; ++i) wr_u16(0xc45948u + i * 2u,
        (seed & 32u) && !checks && i >= 2 ? (seed & 1u ? 0x7fff : 0x8000)
                                        : (uint16_t)(seed % 30u));
    wr_s32(PROJECTION_Y, seed & 32u ? (int32_t)(0x80000000u + seed * 2048u)
                                  : -(int32_t)(seed % 8u) * 2048);
    wr_s16(PROJECTION_WORDS, (int16_t)next_value());
    wr_s16(PROJECTION_WORDS + 4, (int16_t)next_value());
    for (i = 0; i < 3; ++i) {
        /* Derived roots address up to 4*32+15*2, normal up to 3*16+7*2. */
        for (j = 0; j < 128; ++j) wr_u16(roots[i] + j * 2u, (uint16_t)(controls - roots[i]));
        cursor = controls;
        for (j = 0; variant != 6 && j < bands; ++j) {
            wr_u8(cursor++, (uint8_t)(seed % 21u));
            if (variant == 0) {
                /* Zero selector count still consumes one signed selector
                 * in the second walk. The first walk sees this FF as its
                 * terminator because the source count is also its skip. */
                wr_u8(cursor++, 0); wr_u8(cursor++, 0xff);
            } else {
                wr_u8(cursor++, (uint8_t)selectors);
                for (k = 0; k < selectors; ++k) wr_u8(cursor++, (uint8_t)k);
            }
        }
        wr_u8(cursor, 0xff);
        for (j = 0; j < 128; ++j) wr_u16(directories[i] + j * 2u, (uint16_t)(cell - directories[i]));
        for (j = 0; j < 2048; j += 4) wr_u32(gates[i] + j, 0xffffffffu);
    }
    /* A sorted column array followed by its stream-pointer array; C1D4E4
     * performs its own source binary search, not a fabricated expected list. */
    wr_u16(cell, 258);
    for (i = 0; i < 129; ++i) {
        wr_u16(cell + 2 + i * 2u, (uint16_t)i);
        wr_u32(cell + 260 + i * 4u, stream);
    }
    for (i = 0; i < 4; ++i)
        for (j = 0; j < 21; ++j) wr_u8(0xc411f0u + i * 24u + j, (uint8_t)j);
    cursor = stream;
    for (i = 0; i < 16; ++i) {
        for (j = 0; j < items; ++j) {
            wr_u8(cursor++, (uint8_t)(((seed & 15u) << 4) | i));
            wr_u8(cursor++, (uint8_t)((variant & 32u ? 0x80u : 0) | (j % 8u)));
            if (!(variant & 32u)) {
                wr_u16(cursor, (uint16_t)next_value());
                wr_u16(cursor + 2, (uint16_t)next_value()); cursor += 4;
            }
        }
    }
    wr_u8(cursor, 0xff);
    for (i = 0; i < 16; ++i) {
        gaddr record = CONTROL_RECORDS + i * CONTROL_RECORD_BYTES;
        gaddr workspace = WORKSPACE_RECORDS + i * WORKSPACE_RECORD_BYTES;
        for (j = 0; j < 2; ++j) {
            gaddr bank = j ? workspace : record;
            wr_u8(bank + 1, checks && items && i < 8 ? 0x54 : 0);
            wr_u16(bank + 2, (uint16_t)(i % 8u));
            wr_u16(bank + 6, (uint16_t)(seed % 30u)); wr_u16(bank + 8, (uint16_t)(seed % 30u));
            wr_u8(bank + 10, (uint8_t)(i & 15u));
            wr_u16(bank + 12, (uint16_t)next_value()); wr_u16(bank + 14, (uint16_t)next_value());
            wr_s32(bank + 16, (int32_t)next_value());
        }
    }
    for (i = 0; i < 8; ++i) {
        gaddr descriptor = SCENE_POINTERS + i * 20u;
        gaddr flags = 0xc66000u + i * 128u;
        wr_u32(descriptor + 4, flags);
        wr_u16(flags, (seed & 2u) ? 0x8006u : (seed & 4u) ? 0x4000 : 0);
        wr_u16(flags + 2, 6); wr_u16(flags + 4, 6); wr_u16(flags + 6, 32);
        if (i == 1 && (seed & 8u)) wr_u32(descriptor + 4, 0);
        if (i == 2 && (seed & 8u)) wr_u16(flags, 0xffff);
        if (i == 3 && (seed & 8u)) wr_u16(flags + 6, 0xffff);
        for (j = 0; j < 5; ++j) wr_u16(flags + 32 + j * 2u, (uint16_t)next_value());
        section(flags + 42, (variant & 4u) ? flags + 96 : 0x80000000u, 2);
        section(flags + 96, (seed & 1u) ? 0 : 0xffffffffu, 1);
    }
    for (i = 0; i < 11; ++i) {
        wr_u32(0xc4d790u + i * 4, 0xffffffffu);
        for (j = 0; j < 256; j += 4) wr_u32(0xc4d7bcu + i * 256u + j, 0xa5a5a5a5u);
    }
    m68k_set_reg(M68K_REG_SR, 0x2700u | (scenario & 31u));
    for (i = 0; i < 15; ++i) REG_DA[i] = next_value();
    REG_A[7] = 0xc7ff00u; wr_u32(REG_A[7], 0xc70000u); REG_PC = 0xc1d10cu;
}

static int source_call(uint32_t return_pc, uint32_t return_sp) {
    unsigned dispatch;
    int result = FA18_RET;
    for (dispatch = 0; dispatch < 100000; ++dispatch) {
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
    size_t state_size = 0, rom_size = 0, fixture_size = 0;
    uint8_t *state = read_file("captures/native/demo01/state.bin", &state_size);
    uint8_t *rom = read_file("local/system/kick13.rom", &rom_size);
    uint8_t *entry = argc > 1 ? read_file(argv[1], &fixture_size) : NULL;
    unsigned count = argc > 2 ? (unsigned)strtoul(argv[2], NULL, 10) : 1, scenario;
    FA18Machine *m = calloc(1, sizeof *m), *before = malloc(sizeof *before);
    uint8_t *reference = malloc(FA18_CHIP_SIZE + FA18_SLOW_SIZE);
    unsigned char *cpu = malloc(m68k_context_size());
    uint32_t registers[18];
    char error[256];
    unsigned modes[3] = {0}, empty = 0, capped = 0, linked = 0, recoverable = 0;
    if (!state || !rom || !entry || !m || !before || !reference || !cpu || !count
        || fixture_size != sizeof registers + FA18_CHIP_SIZE + FA18_SLOW_SIZE) {
        fputs("template oracle: need sealed state/ROM, valid entry fixture and positive cases\n", stderr); return 1;
    }
    if (!fa18_machine_load_state(m, state, state_size, rom, rom_size, error, sizeof error)) {
        fprintf(stderr, "template oracle: %s\n", error); return 1;
    }
    free(state); free(rom); memcpy(registers, entry, sizeof registers);
    if (registers[17] != 0xc1d10cu) { fputs("template oracle: wrong captured entry\n", stderr); return 1; }
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF, NULL); fa18_bus_timing = 0;
    for (scenario = 0; scenario < count; ++scenario) {
        unsigned i;
        uint32_t return_pc, return_sp;
        int result;
        memcpy(m->chip, entry + sizeof registers, FA18_CHIP_SIZE);
        memcpy(m->slow, entry + sizeof registers + FA18_CHIP_SIZE, FA18_SLOW_SIZE);
        m68k_set_reg(M68K_REG_SR, registers[16]);
        for (i = 0; i < 16; ++i) REG_DA[i] = registers[i];
        REG_PC = registers[17];
        if (scenario) structural_fixture(scenario);
        if (scenario) wr_u16(ERROR_CODE, 0);
        fa18_recomp_abort = 0; SET_CYCLES(100000000);
        fa18_next_event = fa18_cycle_origin - GET_CYCLES() + 100000000;
        return_pc = rd_u32(REG_A[7]) & 0xffffffu; return_sp = REG_A[7] + 4;
        ++modes[rd_u8(CELL_CHECKS) ? 2 : rd_u8(0xc45865u) ? 1 : 0];
        memcpy(before, m, sizeof *m); m68k_get_context(cpu);
        fa18_write_log_active = 1;
        result = source_call(return_pc, return_sp);
        if (result != FA18_RET || (REG_PC & 0xffffffu) != return_pc || REG_A[7] != return_sp) {
            fprintf(stderr, "template oracle: case %u source return %d PC %06X error %04X\n",
                    scenario, result, REG_PC, rd_u16(ERROR_CODE)); return 1;
        }
        empty += !rd_u16(0xc4e988u); capped += rd_u16(0xc4e988u) >= 70;
        if (scenario) {
            for (i = 0; i < 11; ++i) {
                gaddr packet = 0xc4d7bcu + i * 256u;
                if (rd_s32(0xc4d790u + i * 4) > 0 && rd_u32(packet + 24) == packet + 40) {
                    ++linked; break;
                }
            }
        }
        recoverable += rd_u16(ERROR_CODE) == 0x0f;
        memcpy(reference, m->chip, FA18_CHIP_SIZE);
        memcpy(reference + FA18_CHIP_SIZE, m->slow, FA18_SLOW_SIZE);
        memcpy(m, before, sizeof *m); m68k_set_context(cpu);
        refresh_template_placements(); fa18_write_log_active = 0;
        for (i = 0; i < FA18_CHIP_SIZE + FA18_SLOW_SIZE; ++i) {
            uint8_t got = i < FA18_CHIP_SIZE ? m->chip[i] : m->slow[i - FA18_CHIP_SIZE];
            gaddr address = i < FA18_CHIP_SIZE ? i : i - FA18_CHIP_SIZE + FA18_SLOW_BASE;
            /* LINK: saved A6+60 locals; saved control cursor; child return;
             * 40-byte MOVEM; nested return; 4-byte D6 save = 124 bytes
             * below returned SP. Round only this private region to 128. */
            if (address >= return_sp - 128u && address < return_sp) continue;
            if (got != reference[i]) {
                fprintf(stderr, "template oracle: case %u byte %06X source %02X C %02X\n",
                        scenario, address, reference[i], got); return 1;
            }
        }
    }
    printf("template oracle: %u complete C1D10C domain calls matched original Chip/Slow RAM outside 128-byte private stack\n",
           count);
    printf("contexts A/B/append %u/%u/%u; empty %u, capped %u, linked-copies %u, recoverable-shift %u\n",
           modes[0], modes[1], modes[2], empty, capped, linked, recoverable);
    free(entry); free(cpu); free(reference); free(before); free(m); return 0;
}
