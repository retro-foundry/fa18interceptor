/* Generator helper: Musashi's disassembler and opcode->handler mapping over a
 * flat 24-bit image supplied by tools/recomp/recomp.py. Including m68kops.c
 * here makes its static handler table visible for index lookup. */
#include <stdint.h>
#include <string.h>
#include "m68kops.c"
#include "m68k.h"

static uint8_t image[1 << 24];

static unsigned rd8(unsigned a) { return image[a & 0xFFFFFF]; }
static unsigned rd16(unsigned a) { return rd8(a) << 8 | rd8(a + 1); }
unsigned int m68k_read_memory_8(unsigned int a) { return rd8(a); }
unsigned int m68k_read_memory_16(unsigned int a) { return rd16(a); }
unsigned int m68k_read_memory_32(unsigned int a) { return rd16(a) << 16 | rd16(a + 2); }
void m68k_write_memory_8(unsigned int a, unsigned int v) { (void)a; (void)v; }
void m68k_write_memory_16(unsigned int a, unsigned int v) { (void)a; (void)v; }
void m68k_write_memory_32(unsigned int a, unsigned int v) { (void)a; (void)v; }
unsigned int m68k_read_disassembler_16(unsigned int a) { return rd16(a); }
unsigned int m68k_read_disassembler_32(unsigned int a) { return rd16(a) << 16 | rd16(a + 2); }
void fa18_machine_instruction_hook(unsigned int pc) { (void)pc; }

__declspec(dllexport) void fa18_dasm_load(uint32_t base, const uint8_t *data, uint32_t size) {
    memcpy(image + (base & 0xFFFFFF), data, size);
}

__declspec(dllexport) int fa18_dasm_init(void) {
    m68k_init();
    m68k_set_cpu_type(M68K_CPU_TYPE_68000);
    return 1;
}

__declspec(dllexport) unsigned fa18_dasm(uint32_t pc, char *out) {
    return m68k_disassemble(out, pc, M68K_CPU_TYPE_68000);
}

__declspec(dllexport) int fa18_handler_index(unsigned op) {
    void (*h)(void) = m68ki_instruction_jump_table[op & 0xFFFF];
    int i;
    for (i = 0; m68k_opcode_handler_table[i].mask; i++)
        if (m68k_opcode_handler_table[i].opcode_handler == h) return i;
    return -1;
}

__declspec(dllexport) int fa18_handler_cycles(unsigned op) { return m68ki_cycles[0][op & 0xFFFF]; }
