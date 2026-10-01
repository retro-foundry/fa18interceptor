#ifndef FA18_GLUE_STEP_H
#define FA18_GLUE_STEP_H
/* Source instruction plumbing shared by timing bridges. CPU state stays in
 * glue; the game operations remain in their readable domain modules. */
#include "glue.h"
#include "hardware.h"
#include "bus.h"
#include "m68kops.h"

static inline uint16_t step_begin(uint32_t pc) {
    uint16_t opcode = fa18_bus_read16(pc);
    fa18_bus_finish(pc); fa18_bus_begin(pc); fa18_bus_fetch(pc);
    REG_PPC = pc; REG_IR = opcode; REG_PC = pc + 2;
    return opcode;
}
static inline uint32_t step_displacement(uint32_t base) {
    return base + (int16_t)m68ki_read_imm_16();
}
static inline uint32_t step_indexed(uint32_t base) {
    uint16_t extension = m68ki_read_imm_16();
    uint32_t index = REG_DA[extension >> 12];
    if (!(extension & 0x0800u)) index = (uint32_t)(int32_t)(int16_t)index;
    return base + index + (int8_t)extension;
}
static inline void step_write_word(uint32_t address, uint16_t value) {
    if (address >= 0xDFF000u && address < 0xDFF200u) {
        fa18_bus_access(address); custom_write(address - 0xDFF000u, value);
    } else m68k_write_memory_16(address, value);
}
static inline void step_write_long(uint32_t address, uint32_t value) {
    if (address >= 0xDFF000u && address < 0xDFF1FEu) {
        fa18_bus_access(address); fa18_bus_access(address + 2);
        custom_write_ptr(address - 0xDFF000u, value);
    } else m68k_write_memory_32(address, value);
}
static inline void step_predecrement_long(uint32_t value) {
    A(7) -= 4;
    m68k_write_memory_16(A(7) + 2, value);
    m68k_write_memory_16(A(7), value >> 16);
}
static inline void step_compare_word(uint16_t source, uint16_t destination) {
    uint32_t result = (uint32_t)destination - source;
    FLAG_N = NFLAG_16(result); FLAG_Z = result & 0xFFFFu;
    FLAG_V = VFLAG_SUB_16(source, destination, result); FLAG_C = CFLAG_16(result);
}
static inline void step_compare_long(uint32_t source, uint32_t destination) {
    uint32_t result = destination - source;
    FLAG_N = NFLAG_32(result); FLAG_Z = result;
    FLAG_V = VFLAG_SUB_32(source, destination, result);
    FLAG_C = CFLAG_SUB_32(source, destination, result);
}
static inline void step_add_word(uint32_t *reg, uint16_t value) {
    uint32_t old = *reg & 0xFFFFu, sum = old + value;
    SET_W(*reg, sum);
    FLAG_N = NFLAG_16(sum); FLAG_Z = sum & 0xFFFFu;
    FLAG_V = VFLAG_ADD_16(value, old, sum); FLAG_X = FLAG_C = CFLAG_16(sum);
}
static inline void step_add_long(uint32_t *reg, uint32_t value) {
    uint32_t old = *reg, sum = old + value;
    *reg = sum;
    FLAG_N = NFLAG_32(sum); FLAG_Z = sum;
    FLAG_V = VFLAG_ADD_32(value, old, sum);
    FLAG_X = FLAG_C = CFLAG_ADD_32(value, old, sum);
}
static inline void step_subtract_long(uint32_t *reg, uint32_t value) {
    uint32_t old = *reg, result = old - value;
    *reg = result;
    step_compare_long(value, old); FLAG_X = FLAG_C;
}
static inline void step_subtract_word(uint32_t *reg, uint16_t value) {
    uint16_t old = (uint16_t)*reg;
    SET_W(*reg, (uint32_t)old - value);
    step_compare_word(value, old); FLAG_X = FLAG_C;
}
static inline void step_compare_byte(uint8_t source, uint8_t destination) {
    uint32_t result = (uint32_t)destination - source;
    FLAG_N = NFLAG_8(result); FLAG_Z = result & 0xFFu;
    FLAG_V = VFLAG_SUB_8(source, destination, result); FLAG_C = CFLAG_8(result);
}
static inline void step_subtract_byte(uint32_t *reg, uint8_t value) {
    uint8_t old = (uint8_t)*reg;
    SET_B(*reg, (uint32_t)old - value);
    step_compare_byte(value, old); FLAG_X = FLAG_C;
}
static inline void step_swap(uint32_t *reg) {
    *reg = (*reg << 16) | (*reg >> 16); flags_logic_l(*reg);
}
static inline void step_save_registers(void) {
    uint16_t mask = m68ki_read_imm_16();
    unsigned i, count = 0;
    for (i = 0; i < 16; ++i) if (mask & (1u << i)) {
        step_predecrement_long(REG_DA[15 - i]); ++count;
    }
    USE_CYCLES(count << CYC_MOVEM_L);
}
static inline void step_restore_registers(void) {
    uint16_t mask = m68ki_read_imm_16();
    uint32_t address = A(7);
    unsigned i, count = 0;
    for (i = 0; i < 16; ++i) if (mask & (1u << i)) {
        REG_DA[i] = m68k_read_memory_32(address); address += 4; ++count;
    }
    A(7) = address; USE_CYCLES(count << CYC_MOVEM_L);
}
static inline void step_dbf(uint32_t pc, uint32_t *reg) {
    SET_W(*reg, *reg - 1);
    if ((uint16_t)*reg != 0xFFFFu) {
        int16_t displacement = (int16_t)m68ki_read_imm_16();
        REG_PC = pc + 2 + displacement; USE_CYCLES(CYC_DBCC_F_NOEXP);
    } else {
        /* The source skips the extension fetch on the exhausted path. */
        REG_PC += 2; USE_CYCLES(CYC_DBCC_F_EXP);
    }
}
static inline void step_lsr_long(uint32_t *reg, unsigned count) {
    uint32_t old = *reg, result;
    count &= 63;
    result = count < 32 ? old >> count : 0;
    flags_logic_l(result);
    if (count) FLAG_X = FLAG_C = count <= 32 ? ((old >> (count - 1)) & 1u) << 8 : 0;
    *reg = result; USE_CYCLES(count << CYC_SHIFT);
}
static inline uint16_t step_lsr_word_value(uint16_t old, unsigned count) {
    uint16_t result;
    count &= 63;
    result = count < 16 ? (uint16_t)(old >> count) : 0;
    flags_logic_w(result); FLAG_C = 0;
    if (count) FLAG_X = FLAG_C = count <= 16 ? ((old >> (count - 1)) & 1u) << 8 : 0;
    USE_CYCLES(count << CYC_SHIFT);
    return result;
}
static inline void step_asl_long(uint32_t *reg, unsigned count) {
    uint32_t old = *reg, result, mask;
    count &= 63;
    result = count < 32 ? old << count : 0;
    flags_logic_l(result);
    FLAG_C = 0; FLAG_V = 0;
    if (count) {
        FLAG_X = FLAG_C = count <= 32 ? ((old >> (32 - count)) & 1u) << 8 : 0;
        if (count < 32) {
            mask = 0xFFFFFFFFu << (31 - count);
            old &= mask; FLAG_V = (old != 0 && old != mask) << 7;
        } else if (count == 32) {
            FLAG_V = (old != 0 && old != 0xFFFFFFFFu) << 7;
        } else FLAG_V = (old != 0) << 7;
    }
    *reg = result; USE_CYCLES(count << CYC_SHIFT);
}
static inline void step_asr_long(uint32_t *reg, unsigned count) {
    uint32_t old = *reg, result;
    count &= 63;
    if (!count) result = old;
    else if (count < 32) result = (uint32_t)((int32_t)old >> count);
    else result = (uint32_t)((int32_t)old >> 31);
    flags_logic_l(result); FLAG_C = 0; FLAG_V = 0;
    if (count) FLAG_X = FLAG_C = count <= 32 ? ((old >> (count - 1)) & 1u) << 8 : (old >> 31) << 8;
    *reg = result; USE_CYCLES(count << CYC_SHIFT);
}
static inline void step_branch(uint32_t pc, uint16_t opcode, int take) {
    int displacement = (int8_t)opcode;
    if (take) {
        if (!(opcode & 0xFFu)) displacement = (int16_t)m68ki_read_imm_16();
        REG_PC = pc + 2 + displacement;
    } else {
        /* The 68000 word Bcc not-taken path skips the extension read. */
        if (!(opcode & 0xFFu)) REG_PC += 2;
        USE_CYCLES((opcode & 0xFFu) ? CYC_BCC_NOTAKE_B : CYC_BCC_NOTAKE_W);
    }
}
#endif
