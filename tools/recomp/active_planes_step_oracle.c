/* One-instruction oracle for registered game timing bridges. The
 * authoritative instruction bytes come from the sealed state; Musashi
 * evaluates them independently. Recorded full-call/live checks complement
 * this structural proof, whose chipset writes and events are held. Optional
 * bus fixtures compare instruction access timing under five-plane DMA. */
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

static unsigned fixture_bus_offset;
static void reset_fixture_bus(void) {
    fa18_bus_reset();
    if (fa18_bus_timing) {
        /* Five low-resolution planes and refresh compete for RAM slots.
         * Reset the same map before each side of the comparison. */
        fa18_machine->dmacon = 0x0300;
        fa18_machine->custom[0x100 >> 1] = 0x5000;
        fa18_machine->custom[0x08e >> 1] = 0x2081;
        fa18_machine->custom[0x090 >> 1] = 0x2cc1;
        fa18_machine->custom[0x092 >> 1] = 0x0038;
        fa18_machine->custom[0x094 >> 1] = 0x00d0;
        fa18_bus_line(fa18_machine, 80, fa18_machine_now() - fixture_bus_offset);
    }
}

static void fixture(uint32_t pc, unsigned scenario) {
    static const uint16_t boundaries[] = {0, 1, 0x7FFF, 0x8000, 0xFFFF, 0xFC00, 0x03FF, 0x0400, 15, 16, 31, 32, 33, 63, 64, 65};
    unsigned i;
    fixture_bus_offset = 40 + (scenario % 32u) * 4;
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
    if ((pc >= 0xC0DAEEu && pc < 0xC0DB42u) ||
        (pc >= 0xC2EC70u && pc < 0xC2ED6Cu) ||
        (pc >= 0xC2F1B8u && pc < 0xC2F482u)) {
        REG_A[0] = 0xC61000u; REG_A[5] = 0xC61400u;
        REG_A[6] = 0xC62080u;
        for (i = 0; i < 96; ++i) {
            wr_u32(REG_A[0] + i * 4, next_value());
            wr_u32(REG_A[1] + i * 4, next_value());
            wr_u32(REG_A[2] + i * 4, next_value());
            wr_u32(REG_A[3] + i * 4, next_value());
            wr_u32(REG_A[4] + i * 4, next_value());
        }
        for (i = 0; i < 32; ++i) {
            wr_u32(REG_A[6] - 0x40 + i * 4, next_value());
            wr_u32(REG_A[7] + i * 4, next_value());
        }
        if (pc == 0xC2ECCAu || pc == 0xC2ECDCu) {
            static const uint32_t dividends[] = {
                0, 1, 0xffffffffu, 0x80000000u, 0x7fffffffu,
                0xffff0000u, 0x00008000u, 0x00010000u
            };
            static const uint16_t divisors[] = {0, 1, 0xffff, 2, 0xfffe, 0x7fff, 0x8000, 17};
            REG_D[pc == 0xC2ECCAu ? 0 : 1] = dividends[scenario % 8u];
            REG_D[2] = (REG_D[2] & 0xffff0000u) | divisors[(scenario / 4u) % 8u];
        }
    }
    if ((pc >= 0xC122A2u && pc < 0xC123FAu) ||
        (pc >= 0xC1C2C8u && pc < 0xC1C40Cu) ||
        (pc >= 0xC1C54Eu && pc < 0xC1C63Eu) ||
        (pc >= 0xC230B0u && pc < 0xC23228u) ||
        (pc >= 0xC23744u && pc < 0xC23A26u) ||
        (pc >= 0xC244E2u && pc < 0xC2467Eu) ||
        (pc >= 0xC254E8u && pc < 0xC2564Eu) ||
        (pc >= 0xC25864u && pc < 0xC25876u) ||
        (pc >= 0xC25704u && pc < 0xC257DCu)) {
        REG_A[0] = 0xC61000u; REG_A[5] = 0xC61400u;
        REG_A[6] = 0xC62080u;
        for (i = 0; i < 96; ++i) {
            wr_u32(REG_A[0] + i * 4, next_value());
            wr_u32(REG_A[1] + i * 4, next_value());
            wr_u32(REG_A[2] + i * 4, next_value());
            wr_u32(REG_A[4] + i * 4, next_value());
            wr_u32(REG_A[5] + i * 4, next_value());
        }
        for (i = 0; i < 32; ++i) {
            wr_u32(REG_A[6] - 0x40 + i * 4, next_value());
            wr_u32(REG_A[7] + i * 4, next_value());
        }
        wr_u16(0xC459C0u, boundaries[scenario % 16u]);
        wr_u16(REG_A[1] + 0x4a, boundaries[(scenario + 1) % 16u]);
        wr_u16(REG_A[1] + 0x6c, boundaries[(scenario + 2) % 16u]);
        if (pc == 0xC1C35Cu || pc == 0xC1C3BCu || pc == 0xC257A8u) {
            static const uint32_t dividends[] = {
                0, 1, 65535, 65536, 0x7fffffffu, 0x80000000u,
                (17u << 16) - 1, 17u << 16, 0xffffffffu
            };
            static const uint16_t divisors[] = {0, 1, 17, 0x7fff, 0x8000, 0xffff};
            unsigned destination = pc == 0xC1C35Cu ? 6 : pc == 0xC1C3BCu ? 1 : 0;
            unsigned source = pc == 0xC257A8u ? 1 : 5;
            REG_D[destination] = dividends[scenario % 9u];
            REG_D[source] = (REG_D[source] & 0xffff0000u) | divisors[(scenario / 4u) % 6u];
        }
    }
    if ((pc >= 0xC258C8u && pc < 0xC25980u) ||
        (pc >= 0xC2D970u && pc < 0xC2DCC2u) ||
        (pc >= 0xC2DEE0u && pc < 0xC2E47Au) ||
        (pc >= 0xC2E5ACu && pc < 0xC2E5F6u)) {
        REG_A[0] = 0xC61000u; REG_A[5] = 0xC61400u;
        REG_A[6] = 0xC62080u;
        for (i = 0; i < 96; ++i) {
            wr_u32(REG_A[0] + i * 4, next_value());
            wr_u32(REG_A[1] + i * 4, next_value());
            wr_u32(REG_A[2] + i * 4, next_value());
            wr_u32(REG_A[3] + i * 4, next_value());
            wr_u32(REG_A[4] + i * 4, next_value());
        }
        for (i = 0; i < 32; ++i)
            wr_u32(REG_A[7] + i * 4, next_value());
        if (rd_u16(pc) == 0x81c3u) { /* DIVS.W D3,D0 in record composition. */
            static const uint32_t dividends[] = {0, 1, 0xffffffffu, 0x80000000u,
                                                0x7fffffffu, 0xffff0000u, 0x8000u, 0x10000u};
            static const uint16_t divisors[] = {0, 1, 0xffff, 2, 0xfffe, 0x7fff, 0x8000, 17};
            REG_D[0] = dividends[scenario % 8u];
            REG_D[3] = (REG_D[3] & 0xffff0000u) | divisors[(scenario / 4u) % 8u];
        }
    }
    if ((pc >= 0xC082B0u && pc < 0xC0833Eu) ||
        (pc >= 0xC12098u && pc < 0xC12242u) ||
        (pc >= 0xC1B906u && pc < 0xC1C2B8u) ||
        (pc >= 0xC1C7F6u && pc < 0xC1C85Eu)) {
        REG_A[0] = 0xC61000u; REG_A[5] = 0xC61400u;
        REG_A[6] = 0xC62080u;
        for (i = 0; i < 96; ++i) {
            wr_u32(REG_A[0] + i * 4, next_value());
            wr_u32(REG_A[1] + i * 4, next_value());
            wr_u32(REG_A[3] + i * 4, next_value());
        }
        for (i = 0; i < 32; ++i) {
            wr_u32(REG_A[6] - 0x40 + i * 4, next_value());
            wr_u32(REG_A[7] + i * 4, next_value());
        }
        if (pc >= 0xC1C7F6u && pc < 0xC1C85Eu) {
            static const uint16_t rates[] = {0, 0x60, 0x61, 0xc0, 0xc1,
                                            0x1000, 0x1001, 0x7fff, 0x8000, 0xffff};
            for (i = 0; i < 3; ++i)
                wr_u16(REG_A[3] + 0x56 + i * 2, rates[(scenario + i) % 10u]);
            wr_u16(REG_A[3] + 0x6c, rates[(scenario + 3) % 10u]);
        }
    }
    if (pc >= 0xC06132u && pc < 0xC06178u) {
        REG_A[6] = 0xC62080u;
        wr_u16(REG_A[6] + 0x20, boundaries[scenario % 16u]);
        for (i = 0; i < 8; ++i)
            wr_u32(REG_A[7] + i * 4, next_value());
    }
    if (pc >= 0xC2F5C0u && pc < 0xC2FA78u) {
        REG_A[0] = 0xC61000u; REG_A[5] = 0xC61400u;
        for (i = 0; i < 32; ++i) {
            wr_u32(REG_A[0] + i * 4, next_value());
            wr_u32(REG_A[1] + i * 4, next_value());
            wr_u32(REG_A[2] + i * 4, next_value());
            wr_u32(REG_A[3] + i * 4, next_value());
            wr_u32(REG_A[4] + i * 4, next_value());
            wr_u32(REG_A[7] + i * 4, next_value());
        }
    }
    if ((pc >= 0xC09A78u && pc < 0xC09B48u) ||
        (pc >= 0xC1CA82u && pc < 0xC1CB14u) ||
        (pc >= 0xC1D90Au && pc < 0xC1D9D8u) ||
        (pc >= 0xC1E328u && pc < 0xC1E504u)) {
        REG_A[0] = 0xC61000u; REG_A[5] = 0xC61400u;
        REG_A[6] = 0xC62080u;
        for (i = 0; i < 96; ++i) {
            wr_u32(REG_A[0] + i * 4, next_value());
            wr_u32(REG_A[1] + i * 4, next_value());
            wr_u32(REG_A[2] + i * 4, next_value());
            wr_u32(REG_A[4] + i * 4, next_value());
            wr_u32(REG_A[5] + i * 4, next_value());
        }
        for (i = 0; i < 32; ++i) {
            wr_u32(REG_A[6] - 0x40 + i * 4, next_value());
            wr_u32(REG_A[7] + i * 4, next_value());
        }
        if (pc == 0xC1D994u || pc == 0xC1D9B2u) {
            static const uint32_t dividends[] = {
                0, 1, 65535, 65536, 0x7fffffffu, 0x80000000u,
                (17u << 16) - 1, 17u << 16, 0xffffffffu
            };
            static const uint16_t divisors[] = {0, 1, 17, 0x7fff, 0x8000, 0xffff};
            REG_D[pc == 0xC1D994u ? 3 : 4] = dividends[scenario % 9u];
            REG_D[2] = (REG_D[2] & 0xffff0000u) | divisors[(scenario / 4u) % 6u];
        }
    }
    if ((pc >= 0xC0F56Au && pc < 0xC0F5F8u) ||
        (pc >= 0xC11312u && pc < 0xC1134Eu) ||
        (pc >= 0xC11B0Eu && pc < 0xC11B42u) ||
        (pc >= 0xC123FAu && pc < 0xC12950u) ||
        (pc >= 0xC24E2Cu && pc < 0xC25A3Eu) ||
        (pc >= 0xC28720u && pc < 0xC28F2Cu) ||
        (pc >= 0xC2D954u && pc < 0xC2D99Cu) ||
        (pc >= 0xC2E47Au && pc < 0xC2E750u)) {
        REG_A[0] = 0xC61000u; REG_A[5] = 0xC61400u;
        REG_A[6] = 0xC62080u;
        for (i = 0; i < 96; ++i) {
            wr_u32(REG_A[0] + i * 4, next_value());
            wr_u32(REG_A[1] + i * 4, next_value());
            wr_u32(REG_A[2] + i * 4, next_value());
            wr_u32(REG_A[4] + i * 4, next_value());
            wr_u32(REG_A[5] + i * 4, next_value());
        }
        for (i = 0; i < 32; ++i) {
            wr_u32(REG_A[6] - 0x40 + i * 4, next_value());
            wr_u32(REG_A[7] + i * 4, next_value());
        }
        if (pc == 0xC24E36u) {
            static const uint32_t dividends[] = {
                0, 1, 3599, 3600, 3601, 0x00010000u,
                (3600u << 16) - 1, 3600u << 16,
                0x80000000u, 0xffffffffu
            };
            REG_D[0] = dividends[scenario % 10u];
        }
        if (pc == 0xC25664u || pc == 0xC256A0u || pc == 0xC256D4u) {
            static const uint32_t dividends[] = {
                0, 1, 199, 200, 201, (200u << 16) - 1,
                200u << 16, 0x80000000u, 0xffffffffu
            };
            REG_D[2] = dividends[scenario % 9u];
        }
        if (pc == 0xC2566Cu || pc == 0xC256A8u || pc == 0xC256DCu) {
            REG_D[1] = scenario & 16u ? next_value() : next_value() & 0xffffu;
        }
        if (pc == 0xC25998u) {
            static const uint32_t dividends[] = {
                0, 1, 0xffffffffu, 0x80000000u, 0x7fffffffu,
                0xffff0000u, 0x00008000u, 0x00010000u
            };
            static const uint16_t divisors[] = {0, 1, 0xffff, 2, 0xfffe, 0x7fff, 0x8000, 17};
            REG_D[0] = dividends[scenario % 8u];
            REG_D[1] = (REG_D[1] & 0xffff0000u) | divisors[(scenario / 4u) % 8u];
        }
    }
    if (pc >= 0xC1D3F4u && pc < 0xC1D722u) {
        static const uint8_t byte_edges[] = {0, 1, 0x7f, 0x80, 0xff, 15, 16, 31};
        REG_A[0] = 0xC61000u; REG_A[5] = 0xC61400u;
        REG_A[6] = 0xC62080u;
        for (i = 0; i < 8; ++i) {
            /* Also exercise signed bytes and preservation of bits 8-31. */
            if (scenario & 16u)
                REG_D[i] = (REG_D[i] & 0xffffff00u) | byte_edges[(scenario + i) % 8];
        }
        for (i = 0; i < 96; ++i) {
            wr_u32(REG_A[0] + i * 4, next_value());
            wr_u32(REG_A[1] + i * 4, next_value());
            wr_u32(REG_A[2] + i * 4, next_value());
            wr_u32(REG_A[3] + i * 4, next_value());
            wr_u32(REG_A[5] + i * 4, next_value());
        }
        for (i = 0; i < 32; ++i) {
            wr_u32(REG_A[6] - 0x40 + i * 4, next_value());
            wr_u32(REG_A[7] + i * 4, next_value());
        }
    }
    if ((pc >= 0xC2EE44u && pc < 0xC2F1B8u) ||
        (pc >= 0xC2FA70u && pc < 0xC30038u) ||
        (pc >= 0xC301F0u && pc < 0xC30466u)) {
        REG_A[6] = 0xC62080u;
        if (pc < 0xC2F1B8u || (pc >= 0xC301F0u && pc < 0xC30316u))
            REG_A[0] = 0xC61000u;
        for (i = 0; i < 96; ++i) {
            wr_u16(0xC61000u + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[1] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[2] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[3] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[4] + i * 2, (uint16_t)next_value());
        }
        for (i = 0; i < 32; ++i) {
            wr_u32(REG_A[6] - 0x40 + i * 4, next_value());
            wr_u32(REG_A[7] + i * 4, next_value());
        }
    }
    if ((pc >= 0xC0D74Au && pc < 0xC0DAEEu) ||
        (pc >= 0xC2E758u && pc < 0xC2EC68u)) {
        REG_A[0] = 0xC61000u; REG_A[6] = 0xC62080u;
        for (i = 0; i < 96; ++i) {
            wr_u16(REG_A[0] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[1] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[2] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[3] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[4] + i * 2, (uint16_t)next_value());
        }
        for (i = 0; i < 32; ++i) wr_u32(REG_A[6] - 0x40 + i * 4, next_value());
    }
    if ((pc >= 0xC17C62u && pc < 0xC17EF2u) ||
        (pc >= 0xC1803Cu && pc < 0xC18108u) ||
        (pc >= 0xC50AB4u && pc < 0xC50B36u)) {
        REG_A[0] = 0xC61000u; REG_A[6] = 0xC62080u;
        for (i = 0; i < 32; ++i) {
            wr_u32(REG_A[0] + i * 4, next_value());
            wr_u32(REG_A[1] + i * 4, next_value());
            wr_u32(REG_A[6] - 0x40 + i * 4, next_value());
        }
        wr_u32(0xC07288u, next_value());
    }
    if (pc >= 0xC091A8u && pc < 0xC0924Au) {
        for (i = 0; i < 96; ++i) {
            wr_u16(REG_A[1] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[2] + i * 2, (uint16_t)next_value());
        }
    }
    if (pc >= 0xC212B0u && pc < 0xC2131Cu) {
        REG_A[0] = 0xC61000u; REG_A[6] = 0xC62080u;
        for (i = 0; i < 64; ++i) {
            wr_u16(REG_A[4] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[6] - 0x80 + i * 2, (uint16_t)next_value());
        }
    }
    if ((pc >= 0xC23CA6u && pc < 0xC24368u) ||
        (pc >= 0xC24688u && pc < 0xC24DA8u)) {
        REG_A[0] = 0xC61000u; REG_A[6] = 0xC62080u;
        for (i = 0; i < 64; ++i) {
            wr_u16(REG_A[0] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[1] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[2] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[3] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[4] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[6] - 0x80 + i * 2, (uint16_t)next_value());
        }
    }
    if (pc >= 0xC279D0u && pc < 0xC27D24u) {
        REG_A[0] = 0xC61000u; REG_A[5] = 0xC61400u;
        REG_A[6] = 0xC62080u;
        for (i = 0; i < 32; ++i) {
            wr_u16(REG_A[0] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[1] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[3] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[4] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[6] - 0x24 + i * 2, (uint16_t)next_value());
        }
    }
    if (pc >= 0xC2AA9Cu && pc < 0xC2AFFAu) {
        REG_A[0] = 0xC61000u; REG_A[5] = 0xC61400u;
        REG_A[6] = 0xC62080u;
        for (i = 0; i < 48; ++i) {
            wr_u16(REG_A[0] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[1] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[3] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[4] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[6] - 0x46 + i * 2, (uint16_t)next_value());
            wr_u32(REG_A[7] + i * 4, next_value());
        }
    }
    if (pc >= 0xC2B042u && pc < 0xC2B3B4u) {
        REG_A[0] = 0xC61000u; REG_A[5] = 0xC61400u;
        for (i = 0; i < 32; ++i) {
            wr_u16(REG_A[0] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[1] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[3] + i * 2, (uint16_t)next_value());
            wr_u16(REG_A[4] + i * 2, (uint16_t)next_value());
            wr_u32(REG_A[7] + i * 4, next_value());
        }
    }
    if (pc >= 0xC2005Cu && pc < 0xC200F6u) {
        REG_A[0] = 0xC61000u; REG_A[2] = 0xC61200u;
        REG_A[3] = 0xC61300u; REG_A[4] = 0xC61400u;
        REG_A[6] = 0xC62080u;
    }
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
    if (pc >= 0xC2F2CAu && pc < 0xC2F47Eu) {
        /* The source's four-plane circle loop uses A2 as the custom base.
         * Set it after fixture RAM seeding so setup itself does not touch MMIO. */
        REG_A[2] = 0xDFF000u;
    }
    REG_PC = pc;
    fa18_cycle_origin = 100000000;
    fa18_next_event = INT64_MAX;
    SET_CYCLES(100000000);
    reset_fixture_bus();
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
    fa18_bus_timing = argc > 3 && strcmp(argv[3], "bus") == 0;
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
            reset_fixture_bus();
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
    printf("%s step oracle (%s): %u instructions, %u cases matched registers, SR, PC, cycles and RAM\n",
           group, fa18_bus_timing ? "DMA bus" : "CPU", instructions, matched);
    free(cpu); free(reference); free(before); free(m); return 0;
}
