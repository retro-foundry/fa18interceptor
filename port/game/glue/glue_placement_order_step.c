/* Complete placement ordering and packed-cell helper timing. Readable
 * ordering is in placement_order.c; these bounded source PCs preserve CPU,
 * bus accesses and event boundaries while the caller is still translated. */
#include "glue_renderer_step_math.h"
#include <stdlib.h>

#include "glue_cache_step_operands.h"

static int placement_step(uint32_t first, uint32_t end) {
    uint32_t pc = REG_PC, value, address, other;
    uint16_t opcode, mask;
    unsigned mode, reg, destination, width, count;
    if (pc < first || pc >= end) return 0;
    opcode = step_begin(pc);
    mode = (opcode >> 3) & 7u; reg = opcode & 7u;
    destination = (opcode >> 9) & 7u;
    switch (pc) {
    case 0xC1E53C: case 0xC1EAE8: case 0xC1EBAC:
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC1E53E: case 0xC1EAEA: case 0xC1EBAE: case 0xC1EC38:
    case 0xC1EC56: case 0xC1EC82: case 0xC1ECFA: case 0xC1ED28:
        REG_PC = m68ki_pull_32(); break;
    case 0xC1E540:
        m68ki_push_32(A(6)); A(6) = A(7);
        A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC1E544: case 0xC1E5A8: case 0xC1E6FE: case 0xC1E74A:
    case 0xC1E766: case 0xC1E7CA: case 0xC1E888: case 0xC1E8E8:
    case 0xC1E980: case 0xC1E988: case 0xC1EA74: case 0xC1EA7C:
    case 0xC1EAF6: case 0xC1EAFC: case 0xC1EB14: case 0xC1EB16:
    case 0xC1EB28: case 0xC1EC60:
        A(destination) = cache_step_address(mode, reg, 4); break;
    case 0xC1E54A: case 0xC1EBF8: case 0xC1EBFA: case 0xC1ECD4:
    case 0xC1ECE6: case 0xC1ECFC: case 0xC1ED12:
        D(destination) = (uint32_t)(int32_t)(int8_t)opcode;
        flags_logic_l(D(destination)); break;
    case 0xC1E562: case 0xC1E570: case 0xC1E57A: case 0xC1E580:
    case 0xC1E5A2: case 0xC1E5AE: case 0xC1E5B4: case 0xC1E5CE:
    case 0xC1E5F4: case 0xC1E642: case 0xC1E648: case 0xC1E666:
    case 0xC1E682: case 0xC1E6D6: case 0xC1E6EE: case 0xC1E6F6:
    case 0xC1E70A: case 0xC1E70E: case 0xC1E718: case 0xC1E71E:
    case 0xC1E720: case 0xC1E724: case 0xC1E742: case 0xC1E772:
    case 0xC1E776: case 0xC1E780: case 0xC1E786: case 0xC1E788:
    case 0xC1E78C: case 0xC1E7AC: case 0xC1E7C2: case 0xC1E7EE:
    case 0xC1E876: case 0xC1E880: case 0xC1E892: case 0xC1E8A8:
    case 0xC1E8AC: case 0xC1E8BC: case 0xC1E8C4: case 0xC1E8CC:
    case 0xC1E8D6: case 0xC1E8E4: case 0xC1E908: case 0xC1E946:
    case 0xC1E978: case 0xC1E98C: case 0xC1E9AE: case 0xC1E9B0:
    case 0xC1E9CA: case 0xC1E9CC: case 0xC1E9CE: case 0xC1E9D4:
    case 0xC1EA6C: case 0xC1EA90: case 0xC1EA9E: case 0xC1EAA4:
    case 0xC1EAEC: case 0xC1EAF4: case 0xC1EB02: case 0xC1EB0C:
    case 0xC1EB2A: case 0xC1EB30: case 0xC1EB4C: case 0xC1EB5C:
    case 0xC1EB6A: case 0xC1EB7E: case 0xC1EB84: case 0xC1EBE4:
    case 0xC1EBEA: case 0xC1EBFC: case 0xC1EC00: case 0xC1EC08:
    case 0xC1EC12: case 0xC1EC2C: case 0xC1EC3A: case 0xC1EC42:
    case 0xC1EC46: case 0xC1EC52: case 0xC1EC68: case 0xC1EC70:
    case 0xC1EC74: case 0xC1EC7C: case 0xC1ECD6: case 0xC1ECE8:
    case 0xC1ECFE: case 0xC1ED14: case 0xC1E568: case 0xC1E5C6:
    case 0xC1E5CA: case 0xC1E5E6: case 0xC1E65C: case 0xC1E67C:
    case 0xC1E67E: case 0xC1E680: case 0xC1E6AC: case 0xC1E6F2:
    case 0xC1E712: case 0xC1E730: case 0xC1E734: case 0xC1E754:
    case 0xC1E77A: case 0xC1E7A2: case 0xC1E7A6: case 0xC1E7D4:
    case 0xC1E81C: case 0xC1E82C: case 0xC1E840: case 0xC1E87A:
    case 0xC1E87E: case 0xC1E898: case 0xC1EA06: case 0xC1EC58:
    case 0xC1E58E: case 0xC1E790: case 0xC1E8A0: case 0xC1E7FC:
    case 0xC1E7FE: case 0xC1E564: case 0xC1E56E: case 0xC1E5E0:
    case 0xC1E5E2: case 0xC1E5E4: case 0xC1E728: case 0xC1E72C:
    case 0xC1E79A: case 0xC1E79E: case 0xC1E7E6: case 0xC1E7EA:
    case 0xC1E802: case 0xC1E890: case 0xC1E956: case 0xC1E95C:
    case 0xC1E960: case 0xC1EA5A: case 0xC1EC80:
        width = (opcode >> 12) == 1 ? 1 : (opcode >> 12) == 3 ? 2 : 4;
        value = cache_step_read(mode, reg, width);
        mode = (opcode >> 6) & 7u;
        cache_step_write(mode, destination, width, value);
        if (mode != 1) cache_step_logic(value, width);
        break;
    case 0xC1E54C: case 0xC1E656: case 0xC1E93E: case 0xC1E94E:
    case 0xC1EA54: case 0xC1EB2C: case 0xC1EB80: case 0xC1E864:
    case 0xC1EA4E:
        width = (opcode & 0x40u) ? 2 : 4;
        cache_step_logic(cache_step_read(mode, reg, width), width); break;
    case 0xC1E556: case 0xC1E57E: case 0xC1E5A0: case 0xC1E5DA:
    case 0xC1E604: case 0xC1E614: case 0xC1E650: case 0xC1E672:
    case 0xC1E692: case 0xC1E6A2: case 0xC1E6FA: case 0xC1E73A:
    case 0xC1E762: case 0xC1E7BA: case 0xC1E7E2: case 0xC1E860:
    case 0xC1E86E: case 0xC1E8D4: case 0xC1E94A: case 0xC1E958:
    case 0xC1EA4A: case 0xC1EAD6: case 0xC1EAE4:
        step_branch(pc, opcode, 1); break;
    case 0xC1E54E: case 0xC1E55C: case 0xC1E572: case 0xC1E59A:
    case 0xC1E5EE: case 0xC1E658: case 0xC1E664: case 0xC1E738:
    case 0xC1E7AA: case 0xC1E7F0: case 0xC1E8B4: case 0xC1E8C0:
    case 0xC1E93C: case 0xC1EB08: case 0xC1EB2E: case 0xC1EB82:
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1E56C: case 0xC1E63A: case 0xC1EAF2:
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC1E578: case 0xC1E588: case 0xC1E5D4: case 0xC1E5FA:
    case 0xC1E60A: case 0xC1E660: case 0xC1E66C: case 0xC1E688:
    case 0xC1E698: case 0xC1E798: case 0xC1E952:
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1E596: case 0xC1E6DC: case 0xC1E6E2: case 0xC1E942:
    case 0xC1EB3C: case 0xC1EB90: case 0xC1EC40:
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1E620: case 0xC1E62A: case 0xC1E634: case 0xC1E6B4:
    case 0xC1E6C0: case 0xC1E6CC: case 0xC1E85A: case 0xC1E866:
    case 0xC1E8C6: case 0xC1E948: case 0xC1EA40: case 0xC1EA46:
    case 0xC1EA50: case 0xC1EA58: case 0xC1EAC6: case 0xC1EACC:
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1E626: case 0xC1E630: case 0xC1E6BA: case 0xC1E6C6:
    case 0xC1E6D2: case 0xC1E75E: case 0xC1E7DE:
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC1E5AA: case 0xC1E5C2: case 0xC1E5D6: case 0xC1E5DC:
    case 0xC1E5FC: case 0xC1E600: case 0xC1E60C: case 0xC1E610:
    case 0xC1E616: case 0xC1E61A: case 0xC1E66E: case 0xC1E676:
    case 0xC1E68A: case 0xC1E68E: case 0xC1E69A: case 0xC1E69E:
    case 0xC1E6A4: case 0xC1E6A8: case 0xC1E6E6: case 0xC1E6EA:
    case 0xC1E71A: case 0xC1E782: case 0xC1E7F6:
        value = (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16();
        m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC1E7B4:
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC1E85C: case 0xC1E86A: case 0xC1EB24: case 0xC1EB58:
    case 0xC1EBA8:
        step_dbf(pc, &D(reg)); break;
    case 0xC1E558: case 0xC1E584: case 0xC1EB38: case 0xC1EB8C:
        value = m68ki_read_imm_16();
        step_compare_word(value, cache_step_read(mode, reg, 2)); break;
    case 0xC1E894: case 0xC1E8DC: case 0xC1E8E0: case 0xC1E96C:
    case 0xC1E970: case 0xC1E974: case 0xC1EA64: case 0xC1EA68:
        value = m68ki_read_imm_16(); step_add_word(&D(reg), value); break;
    case 0xC1E574: case 0xC1E58A: case 0xC1E704: case 0xC1E744:
    case 0xC1E76C: case 0xC1E7C4: case 0xC1E872: case 0xC1E882:
    case 0xC1E8A4: case 0xC1E8C8: case 0xC1E8D8: case 0xC1E968:
    case 0xC1E97A: case 0xC1EA6E: case 0xC1EB34: case 0xC1EB3E:
    case 0xC1EB88: case 0xC1EB92: case 0xC1EBF0: case 0xC1EC04:
    case 0xC1EC0E: case 0xC1EC18: case 0xC1EC4A: case 0xC1EC5A:
    case 0xC1EC6C: case 0xC1EC78: case 0xC1ECEC: case 0xC1ED04:
    case 0xC1ED1A: case 0xC1E592: case 0xC1E794: case 0xC1EAD0:
    case 0xC1EAD8:
        width = (opcode & 0x40u) ? 2 : 1;
        other = m68ki_read_imm_16();
        address = mode == 0 ? 0 : cache_step_address(mode, reg, width);
        value = mode == 0 ? D(reg) : cache_step_read_memory(address, width);
        value = (opcode & 0x0200u) ? value & other : value | other;
        if (mode == 0) cache_step_write(0, reg, width, value);
        else cache_step_write_memory(address, value, width, 0);
        cache_step_logic(value, width); break;
    case 0xC1E550: case 0xC1E640: case 0xC1E800: case 0xC1E832:
    case 0xC1E8D0: case 0xC1E95E: case 0xC1EADE: case 0xC1E598:
    case 0xC1E5EC: case 0xC1EA42: case 0xC1EAC8:
        value = destination ? destination : 8;
        if (mode == 1) {
            if (opcode & 0x0100u) A(reg) -= value; else A(reg) += value;
        } else {
            address = mode == 0 ? 0 : cache_step_address(mode, reg, 2);
            other = mode == 0 ? D(reg) : cache_step_read_memory(address, 2);
            if (opcode & 0x0100u) step_subtract_word(&other, value);
            else step_add_word(&other, value);
            if (mode == 0) SET_W(D(reg), other);
            else step_write_word(address, other);
        }
        break;
    case 0xC1E552: case 0xC1E63C: case 0xC1E652: case 0xC1E88E:
    case 0xC1E986: case 0xC1EA7A: case 0xC1EB12: case 0xC1EB20:
    case 0xC1EB50: case 0xC1EB54: case 0xC1EB7A: case 0xC1EBA0:
    case 0xC1EBA4: case 0xC1EC66: case 0xC1E55E: case 0xC1E59C:
    case 0xC1E5F0:
        value = (uint32_t)(int32_t)(int16_t)cache_step_read(mode, reg, 2);
        if ((opcode >> 12) == 9) A(destination) -= value;
        else A(destination) += value;
        break;
    case 0xC1E5D0: case 0xC1E5F6: case 0xC1E606: case 0xC1E668:
    case 0xC1E684: case 0xC1E694: case 0xC1E6D8: case 0xC1E6DE:
    case 0xC1EC3C: case 0xC1E73E: case 0xC1E7BE:
        count = m68ki_read_imm_16() & 31u;
        FLAG_Z = D(reg) & (1u << count);
        if (opcode & 0x0080u) D(reg) &= ~(1u << count);
        break;
    case 0xC1E5BA: case 0xC1E5BC: case 0xC1E81A: case 0xC1E820:
    case 0xC1E930: case 0xC1E932: case 0xC1E934: case 0xC1E9F6:
    case 0xC1E9FA: case 0xC1EA98: case 0xC1EABC: case 0xC1EABE:
    case 0xC1E622: case 0xC1E62C: case 0xC1E636: case 0xC1E6B6:
    case 0xC1E6C2: case 0xC1E6CE: case 0xC1E75A: case 0xC1E7DA:
    case 0xC1E84A: case 0xC1E84C: case 0xC1E84E: case 0xC1E89E:
    case 0xC1E8FE: case 0xC1E9F8: case 0xC1EA30: case 0xC1EA32:
    case 0xC1EA34:
        renderer_negate(&D(reg), (opcode & 0x40u) ? 2 : 4); break;
    case 0xC1E5BE: case 0xC1E5C0: case 0xC1E838: case 0xC1E83A:
    case 0xC1EA1E: case 0xC1EA20: case 0xC1EBE8: case 0xC1EBEE:
    case 0xC1EC30:
        D(reg) = (uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC1E770: case 0xC1E7C8: case 0xC1E80E: case 0xC1E810:
    case 0xC1E834: case 0xC1E836: case 0xC1E886: case 0xC1E912:
    case 0xC1E91A: case 0xC1E91E: case 0xC1E92A: case 0xC1E97E:
    case 0xC1E9DE: case 0xC1E9E6: case 0xC1EA12: case 0xC1EA1A:
    case 0xC1EA72: case 0xC1EAAC: case 0xC1EAB0: case 0xC1EB0E:
    case 0xC1EB10: case 0xC1EB6C: case 0xC1EB6E: case 0xC1EC5E:
    case 0xC1E750: case 0xC1E7D0: case 0xC1E828: case 0xC1E82A:
    case 0xC1E856: case 0xC1E858: case 0xC1E916: case 0xC1E924:
    case 0xC1E93A: case 0xC1E9E2: case 0xC1EA02: case 0xC1EA04:
    case 0xC1EA16: case 0xC1EA26: case 0xC1EA3C: case 0xC1EA3E:
    case 0xC1EAC4: case 0xC1ECE4: case 0xC1ECF8: case 0xC1ED10:
    case 0xC1ED26:
        width = (opcode & 0x40u) ? 2 : 4;
        value = cache_step_read(mode, reg, width);
        if (width == 2) step_add_word(&D(destination), value);
        else step_add_long(&D(destination), value); break;
    case 0xC1E812: case 0xC1E816: case 0xC1E8F2: case 0xC1E8F4:
    case 0xC1E994: case 0xC1E996: case 0xC1E99A: case 0xC1E9A4:
    case 0xC1E9A6: case 0xC1E9AA: case 0xC1E9EA: case 0xC1E9F2:
    case 0xC1EA86: case 0xC1EA88: case 0xC1EA8C: case 0xC1EAB4:
    case 0xC1EAB8: case 0xC1EB62: case 0xC1EC1C: case 0xC1EC1E:
    case 0xC1ECDC: case 0xC1ECF0: case 0xC1ED08: case 0xC1ED1E:
    case 0xC1E61E: case 0xC1E628: case 0xC1E632: case 0xC1E6B2:
    case 0xC1E6BE: case 0xC1E6CA: case 0xC1E83C: case 0xC1E846:
    case 0xC1E9B6: case 0xC1E9BE: case 0xC1E9C6: case 0xC1E9EE:
    case 0xC1EA22: case 0xC1EA2C: case 0xC1EC28: case 0xC1EC2A:
        width = (opcode & 0x40u) ? 2 : 4;
        value = cache_step_read(mode, reg, width);
        if (width == 2) step_subtract_word(&D(destination), value);
        else step_subtract_long(&D(destination), value); break;
    case 0xC1E8B2: case 0xC1E624: case 0xC1E62E: case 0xC1E638:
    case 0xC1E65E: case 0xC1E6B8: case 0xC1E6C4: case 0xC1E6D0:
    case 0xC1E75C: case 0xC1E7DC:
        width = (opcode & 0x40u) ? 2 : 4;
        value = cache_step_read(mode, reg, width);
        if (width == 2) step_compare_word(value, D(destination));
        else step_compare_long(value, D(destination)); break;
    case 0xC1E674: case 0xC1E67A: case 0xC1E7F4: case 0xC1E7FA:
    case 0xC1E902:
        if ((opcode & 0xF8u) == 0x48u) {
            value = A(destination); A(destination) = A(reg); A(reg) = value;
        } else { value = D(destination); D(destination) = D(reg); D(reg) = value; }
        break;
    case 0xC1E708: case 0xC1E8B0: case 0xC1E90C: case 0xC1E910:
    case 0xC1E9D8: case 0xC1E9DC: case 0xC1EA0C: case 0xC1EA10:
    case 0xC1EAA8: case 0xC1EAAA: case 0xC1E8FA: case 0xC1E900:
    case 0xC1E90E: case 0xC1E9B8: case 0xC1E9C0: case 0xC1E9C8:
    case 0xC1E9DA: case 0xC1EA0E: case 0xC1EA96: case 0xC1EA9C:
    case 0xC1EC24: case 0xC1EC26: case 0xC1ECE2: case 0xC1ECF6:
    case 0xC1ED0E: case 0xC1ED24: case 0xC1E748: case 0xC1EC02:
    case 0xC1EC50: case 0xC1ECDA: case 0xC1EB0A: case 0xC1EB68:
    case 0xC1EBF4: case 0xC1EBF6: case 0xC1EC32:
        count = (opcode & 0x20u) ? D(destination) & 63u : destination ? destination : 8;
        if (opcode & 0x0100u) {
            if (opcode & 0x40u) renderer_asl_word(&D(reg), count);
            else step_asl_long(&D(reg), count);
        } else if (opcode & 0x0008u) SET_W(D(reg), step_lsr_word_value(D(reg), count));
        else if (opcode & 0x40u) renderer_asr_word(&D(reg), count);
        else step_asr_long(&D(reg), count);
        break;
    case 0xC1E804: case 0xC1E808: case 0xC1E82E: case 0xC1E8EC:
    case 0xC1E904: case 0xC1E962: case 0xC1E98E: case 0xC1E99E:
    case 0xC1E9D0: case 0xC1EA08: case 0xC1EA5E: case 0xC1EA80:
    case 0xC1EAA0: case 0xC1EBE0: case 0xC1EC34: case 0xC1EB18:
    case 0xC1EB1C: case 0xC1EB44: case 0xC1EB48: case 0xC1EB70:
    case 0xC1EB76: case 0xC1EB98: case 0xC1EB9C:
        mask = m68ki_read_imm_16(); width = (opcode & 0x40u) ? 4 : 2;
        if (mode == 3 || mode == 4) address = A(reg);
        else address = cache_step_address(mode, reg, width);
        if (opcode & 0x0400u) renderer_load(address, mask, width, mode == 3 ? (int)reg : -1);
        else renderer_store(address, mask, width, mode == 4 ? (int)reg : -1);
        break;
    case 0xC1E822: case 0xC1E824: case 0xC1E826: case 0xC1E850:
    case 0xC1E852: case 0xC1E854: case 0xC1E8F8: case 0xC1E8FC:
    case 0xC1E936: case 0xC1E938: case 0xC1E9B2: case 0xC1E9B4:
    case 0xC1E9BA: case 0xC1E9BC: case 0xC1E9C2: case 0xC1E9C4:
    case 0xC1E9FC: case 0xC1E9FE: case 0xC1EA00: case 0xC1EA36:
    case 0xC1EA38: case 0xC1EA3A: case 0xC1EA94: case 0xC1EA9A:
    case 0xC1EAC0: case 0xC1EAC2:
        renderer_multiply(&D(destination), cache_step_read(mode, reg, 2)); break;
    case 0xC1E8B8:
        step_write_word(cache_step_address(mode, reg, 2), 0); flags_logic_w(0); break;
    case 0xC1EC20: case 0xC1EC22: case 0xC1ECE0: case 0xC1ECF4:
    case 0xC1ED0C: case 0xC1ED22:
        step_swap(&D(reg)); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C1E540_step(void) { return placement_step(0xC1E53C, 0xC1EBB0); }
int glue_C1EBE0_step(void) { return placement_step(0xC1EBE0, 0xC1EC3A); }
int glue_C1EC3A_step(void) { return placement_step(0xC1EC3A, 0xC1EC84); }
int glue_C1ECD4_step(void) { return placement_step(0xC1ECD4, 0xC1ECFC); }
int glue_C1ECFC_step(void) { return placement_step(0xC1ECFC, 0xC1ED2A); }
