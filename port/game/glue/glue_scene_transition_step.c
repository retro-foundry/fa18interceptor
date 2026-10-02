/* Scene initializer and its root-setup children ($C0FAA4, $C0924A).
 * Readable operations remain in stages.c, scene_setup.c and control_records.c;
 * source calls, registers, bus accesses and event boundaries stay here. */
#include "glue_renderer_step_math.h"

static void transition_jsr(void) {
    uint32_t address = m68ki_read_imm_32();
    m68ki_push_32(REG_PC); REG_PC = address;
}

static void transition_bsr(uint32_t pc) {
    int16_t displacement = (int16_t)m68ki_read_imm_16();
    m68ki_push_32(REG_PC); REG_PC = pc + 2 + displacement;
}

int glue_C0FAA4_step(void) {
    uint32_t pc = REG_PC, value, address;
    uint16_t opcode;
    if (pc < 0xC0FAA4u || pc >= 0xC0FB28u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC0FAA4:
        value = m68k_read_memory_8(m68ki_read_imm_32());
        address = m68ki_read_imm_32(); m68k_write_memory_8(address, value);
        flags_logic_b(value); break;
    case 0xC0FAAE: case 0xC0FAB6: case 0xC0FAC6: case 0xC0FB02:
        D(0) = (uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(0)); break;
    case 0xC0FAB0: case 0xC0FAB8: case 0xC0FAC0: case 0xC0FAC8: case 0xC0FACE:
        m68k_write_memory_8(m68ki_read_imm_32(), D(0)); flags_logic_b(D(0)); break;
    case 0xC0FABE:
        step_subtract_byte(&D(0), 1); break;
    case 0xC0FAD4: case 0xC0FAE2: case 0xC0FAFA: case 0xC0FB16:
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32();
        m68k_write_memory_8(address, value); flags_logic_b(value); break;
    case 0xC0FADC: case 0xC0FAEA: case 0xC0FB10:
        transition_jsr(); break;
    case 0xC0FAF0:
        transition_bsr(pc); break;
    case 0xC0FAF4:
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC0FB04: case 0xC0FB0A:
        step_write_word(m68ki_read_imm_32(), D(0)); flags_logic_w(D(0)); break;
    case 0xC0FB1E:
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value);
        flags_logic_w(value); break;
    case 0xC0FB26:
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C0840E_step(void) {
    uint32_t pc = REG_PC, value, address;
    uint16_t opcode, mask;
    if (pc < 0xC0840Eu || pc >= 0xC08488u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC0840E:
        value = m68ki_read_imm_32(); step_write_long(m68ki_read_imm_32(), value);
        flags_logic_l(value); break;
    case 0xC08418: case 0xC08428: case 0xC08430: case 0xC08438: case 0xC08440:
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32();
        m68k_write_memory_8(address, value); flags_logic_b(value); break;
    case 0xC08420:
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value);
        flags_logic_w(value); break;
    case 0xC08448: case 0xC0844E: case 0xC08454:
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC0845A:
        step_write_word(m68ki_read_imm_32(), 0); flags_logic_w(0); break;
    case 0xC08460:
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC08464:
        A(0) = m68ki_read_imm_32(); break;
    case 0xC0846A: case 0xC0847A:
        A(0) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC0846E:
        D(0) = (uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(0)); break;
    case 0xC08470:
        D(4) = (uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(4)); break;
    case 0xC08472:
        A(1) = A(0); break;
    case 0xC08474:
        step_write_long(A(1), 0); A(1) += 4; flags_logic_l(0); break;
    case 0xC08476:
        step_dbf(pc, &D(4)); break;
    case 0xC0847E:
        step_dbf(pc, &D(0)); break;
    case 0xC08482:
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC08486:
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C095C0_step(void) {
    uint32_t pc = REG_PC, value, address;
    uint16_t opcode;
    if (pc < 0xC095C0u || pc >= 0xC09620u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC095C0:
        A(1) = m68ki_read_imm_32(); break;
    case 0xC095C6: case 0xC095CA: case 0xC095CE: case 0xC095E2: case 0xC095EA:
        step_write_word(step_displacement(A(1)), 0); flags_logic_w(0); break;
    case 0xC095D2: case 0xC095D6: case 0xC095DA: case 0xC095DE: case 0xC095E6:
        step_write_long(step_displacement(A(1)), 0); flags_logic_l(0); break;
    case 0xC095EE:
        m68k_write_memory_8(step_displacement(A(1)), 0); flags_logic_b(0); break;
    case 0xC095F2:
        value = m68ki_read_imm_16(); address = step_displacement(A(1));
        m68k_write_memory_8(address, value); flags_logic_b(value); break;
    case 0xC095F8:
        value = m68ki_read_imm_16(); address = step_displacement(A(1));
        value &= m68k_read_memory_16(address); step_write_word(address, value);
        flags_logic_w(value); break;
    case 0xC095FE: case 0xC09604:
        step_write_long(m68ki_read_imm_32(), 0); flags_logic_l(0); break;
    case 0xC0960A: case 0xC09610:
        step_write_word(m68ki_read_imm_32(), 0); flags_logic_w(0); break;
    case 0xC09616:
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value);
        flags_logic_w(value); break;
    case 0xC0961E:
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C09620_step(void) {
    uint32_t pc = REG_PC, value, address, bit;
    uint16_t opcode;
    if (pc < 0xC09620u || pc >= 0xC096AAu) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC09620:
        A(0) = m68ki_read_imm_32(); break;
    case 0xC09626:
        bit = m68ki_read_imm_16(); address = step_displacement(A(0));
        value = m68k_read_memory_8(address); FLAG_Z = value & (1u << (bit & 7));
        m68k_write_memory_8(address, value & ~(1u << (bit & 7))); break;
    case 0xC0962C: case 0xC09684:
        value = m68ki_read_imm_16(); address = step_displacement(A(0));
        m68k_write_memory_8(address, value); flags_logic_b(value); break;
    case 0xC09632:
        transition_jsr(); break;
    case 0xC09638: case 0xC09644:
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(0)), value);
        flags_logic_w(value); break;
    case 0xC0963E:
        value = m68ki_read_imm_16(); address = step_displacement(A(0));
        value |= m68k_read_memory_16(address); step_write_word(address, value);
        flags_logic_w(value); break;
    case 0xC0964A: case 0xC09650: case 0xC09656: case 0xC0965C:
    case 0xC09662: case 0xC09668: case 0xC0966E: case 0xC09692:
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC09674: case 0xC0968A:
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value);
        flags_logic_w(value); break;
    case 0xC0967C: case 0xC096A0:
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32();
        m68k_write_memory_8(address, value); flags_logic_b(value); break;
    case 0xC09698:
        flags_logic_b(m68k_read_memory_8(m68ki_read_imm_32())); break;
    case 0xC0969E:
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC096A8:
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C0924A_step(void) {
    uint32_t pc = REG_PC, value, address, mask;
    uint16_t opcode, register_mask;
    unsigned reg;
    if (pc < 0xC0924Au || pc >= 0xC095C0u) return 0;
    opcode = step_begin(pc);
    reg = (opcode >> 9) & 7u;
    switch (pc) {
    case 0xC0924A: case 0xC09250: case 0xC09336: case 0xC0938A:
    case 0xC09390: case 0xC09396: case 0xC0939C: case 0xC094AC:
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC09256: case 0xC0925E: case 0xC092A0: case 0xC092CC:
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32();
        m68k_write_memory_8(address, value); flags_logic_b(value); break;
    case 0xC09266: case 0xC09270: case 0xC0927A: case 0xC09284:
        value = m68k_read_memory_32(m68ki_read_imm_32());
        step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC0928E:
        address = m68ki_read_imm_32(); value = m68k_read_memory_32(address);
        step_add_long(&value, 4); step_write_long(address, value); break;
    case 0xC09294: case 0xC0929A: case 0xC092B0:
        step_write_word(m68ki_read_imm_32(), 0); flags_logic_w(0); break;
    case 0xC092A8: case 0xC092AC: case 0xC0952A:
        transition_bsr(pc); break;
    case 0xC092B6: case 0xC09488: case 0xC0948A: case 0xC09524:
        D(reg) = (uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(reg)); break;
    case 0xC092B8: case 0xC092F0: case 0xC09330:
        value = D((opcode >> 0) & 7u); m68k_write_memory_8(m68ki_read_imm_32(), value);
        flags_logic_b(value); break;
    case 0xC092BE:
        renderer_asl_word(&D(0), 3); break;
    case 0xC09492:
        SET_W(D(5), D(7)); flags_logic_w(D(5)); break;
    case 0xC092C0: case 0xC092C6: case 0xC093BE: case 0xC093C4:
    case 0xC093EC: case 0xC09402:
        value = D(opcode & 7u); step_write_word(m68ki_read_imm_32(), value);
        flags_logic_w(value); break;
    case 0xC092D4: case 0xC0933C: case 0xC093A2: case 0xC093CE:
    case 0xC09416: case 0xC094B6: case 0xC094BC: case 0xC0952E:
        A(reg) = m68ki_read_imm_32(); break;
    case 0xC092DA: case 0xC093A8:
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(reg), value);
        flags_logic_b(value); break;
    case 0xC092E0:
        value = m68ki_read_imm_16(); step_compare_byte(value, D(1)); break;
    case 0xC092E4: case 0xC094AA:
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC092E6: case 0xC092EC: case 0xC0950C: case 0xC09520: case 0xC09526:
        value = m68ki_read_imm_16(); SET_W(D(reg), value); flags_logic_w(value); break;
    case 0xC092EA: case 0xC0931E: case 0xC09494: case 0xC094B2:
        step_branch(pc, opcode, 1); break;
    case 0xC092F6:
        A(0) += (uint32_t)(int32_t)(int16_t)D(0); break;
    case 0xC092F8:
        register_mask = m68ki_read_imm_16(); renderer_load(A(0), register_mask, 4, -1); break;
    case 0xC092FC:
        D(0) = m68ki_read_imm_32(); flags_logic_l(D(0)); break;
    case 0xC09302:
        register_mask = m68ki_read_imm_16(); address = m68ki_read_imm_32();
        renderer_store(address, register_mask, 4, -1); break;
    case 0xC0930A:
        A(1) = m68k_read_memory_32(m68ki_read_imm_32()); break;
    case 0xC09310:
        value = m68k_read_memory_16(A(1)); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC09312: case 0xC093BA: case 0xC094D2:
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC09314: case 0xC09324: case 0xC09498: case 0xC09572:
    case 0xC09576: case 0xC09598: case 0xC0959C:
        value = m68ki_read_imm_16(); reg = opcode & 7u;
        SET_W(D(reg), D(reg) & value); flags_logic_w(D(reg)); break;
    case 0xC09318:
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC0931A: case 0xC09320:
        value = m68k_read_memory_16(step_displacement(A(1)));
        SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC09328:
        value = m68k_read_memory_8(step_indexed(A(1))); SET_B(D(4), value);
        flags_logic_b(value); break;
    case 0xC0932C:
        value = m68ki_read_imm_16(); SET_B(D(4), D(4) & value); flags_logic_b(D(4)); break;
    case 0xC09342: case 0xC09348: case 0xC0934E:
        value = m68ki_read_imm_16(); address = step_displacement(A(1));
        value &= m68k_read_memory_16(address); step_write_word(address, value);
        flags_logic_w(value); break;
    case 0xC09354: case 0xC09366: case 0xC09372: case 0xC09378: case 0xC0937E:
        mask = 1u << (m68ki_read_imm_16() & 7u); address = step_displacement(A(1));
        value = m68k_read_memory_8(address); FLAG_Z = value & mask;
        m68k_write_memory_8(address, value & ~mask); break;
    case 0xC0935A:
        value = m68ki_read_imm_16(); address = step_displacement(A(1));
        value |= m68k_read_memory_16(address); step_write_word(address, value);
        flags_logic_w(value); break;
    case 0xC09360: case 0xC0936C:
        value = m68ki_read_imm_16(); address = step_displacement(A(1));
        value &= m68k_read_memory_8(address); m68k_write_memory_8(address, value);
        flags_logic_b(value); break;
    case 0xC09384:
        value = m68ki_read_imm_16(); address = step_displacement(A(1));
        m68k_write_memory_8(address, value); flags_logic_b(value); break;
    case 0xC093AE: case 0xC093E4: case 0xC093FE:
        reg = opcode & 7u; SET_W(D(reg), (int16_t)(int8_t)D(reg));
        flags_logic_w(D(reg)); break;
    case 0xC093B0: case 0xC094C2: case 0xC094C6:
        reg = opcode & 7u; renderer_asl_word(&D(reg), (opcode >> 9) & 7u); break;
    case 0xC093B2:
        A(0) += (uint32_t)(int32_t)(int16_t)D(0); break;
    case 0xC093B4:
        register_mask = m68ki_read_imm_16(); renderer_load(A(0), register_mask, 2, 0); break;
    case 0xC093B8:
        flags_logic_w(D(0)); break;
    case 0xC093CA:
        m68k_write_memory_8(step_displacement(A(1)), D(2)); flags_logic_b(D(2)); break;
    case 0xC093D4: case 0xC0949C: case 0xC094C4: case 0xC0956E: case 0xC09570:
        value = D(opcode & 7u); SET_W(D(reg), value); flags_logic_w(value); break;
    case 0xC093D6: case 0xC093E6: case 0xC09400: case 0xC094A0: case 0xC094C8:
        value = D(opcode & 7u); step_add_word(&D(reg), value); break;
    case 0xC093D8: case 0xC093F2:
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(5), value);
        flags_logic_w(value); break;
    case 0xC093DE: case 0xC093F8: case 0xC09414:
        reg = opcode & 7u; renderer_asl_word(&D(reg), 2); break;
    case 0xC093E0: case 0xC093FA:
        value = m68k_read_memory_8(step_indexed(A(2))); SET_B(D(7), value);
        flags_logic_b(value); break;
    case 0xC093E8: case 0xC09408: case 0xC09478: case 0xC0947C:
    case 0xC09550: case 0xC09554: case 0xC09590: case 0xC09594:
        value = D(opcode & 7u); step_write_word(step_displacement(A(1)), value);
        flags_logic_w(value); break;
    case 0xC0940C: case 0xC09410: case 0xC09566: case 0xC09568:
        step_swap(&D(opcode & 7u)); break;
    case 0xC0940E: case 0xC09412: case 0xC09422: case 0xC09424:
    case 0xC09436: case 0xC09438: case 0xC094F4:
        step_asl_long(&D(opcode & 7u), 8); break;
    case 0xC0941C:
        register_mask = m68ki_read_imm_16(); address = step_indexed(A(2));
        renderer_load(address, register_mask, 2, -1); break;
    case 0xC09426: case 0xC09428: case 0xC0943A: case 0xC0943C:
        step_asl_long(&D(opcode & 7u), 2); break;
    case 0xC0942A: case 0xC0942C: case 0xC0943E: case 0xC09440:
    case 0xC0944A: case 0xC0944C:
        value = D(opcode & 7u); step_add_long(&D(reg), value); break;
    case 0xC0942E:
        register_mask = m68ki_read_imm_16(); renderer_load(A(0), register_mask, 2, 0); break;
    case 0xC09432: case 0xC09434:
        step_asl_long(&D(opcode & 7u), 4); break;
    case 0xC09442: case 0xC09480:
        value = m68ki_read_imm_32(); step_write_long(step_displacement(A(1)), value);
        flags_logic_l(value); break;
    case 0xC0944E: case 0xC09452: case 0xC09534: case 0xC09538:
        value = D(opcode & 7u); step_write_long(step_displacement(A(1)), value);
        flags_logic_l(value); break;
    case 0xC09456: case 0xC09458: case 0xC09470: case 0xC09472:
        renderer_negate(&D(opcode & 7u), 4); break;
    case 0xC0945A: case 0xC09460:
        value = D(opcode & 7u); step_write_long(m68ki_read_imm_32(), value);
        flags_logic_l(value); break;
    case 0xC09466:
        value = m68ki_read_imm_32(); step_write_long(m68ki_read_imm_32(), value);
        flags_logic_l(value); break;
    case 0xC09474: case 0xC09476: case 0xC0954C: case 0xC0954E:
        step_asr_long(&D(opcode & 7u), 8); break;
    case 0xC0948C: /* MULU.W #10,D7; source-bit-dependent instruction time. */
        value = m68ki_read_imm_16(); mask = value;
        while (mask) { USE_CYCLES(2); mask &= mask - 1; }
        D(7) = (uint16_t)D(7) * value; flags_logic_l(D(7)); break;
    case 0xC09490:
        renderer_asl_word(&D(7), 3); break;
    case 0xC0949E:
        renderer_asl_word(&D(0), 8); break;
    case 0xC094A2:
        A(1) += (uint32_t)(int32_t)(int16_t)D(0); break;
    case 0xC094A4:
        mask = 1u << (m68ki_read_imm_16() & 7u);
        value = m68k_read_memory_8(step_displacement(A(1))); FLAG_Z = value & mask; break;
    case 0xC094CA:
        A(4) = m68k_read_memory_32(step_indexed(A(4))); break;
    case 0xC094CE:
        D(0) = m68k_read_memory_32(step_displacement(A(4))); flags_logic_l(D(0)); break;
    case 0xC094D4:
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value);
        flags_logic_w(value); break;
    case 0xC094DC: case 0xC095B8:
        transition_jsr(); break;
    case 0xC094E2:
        D(0) = 0; flags_logic_l(0); break;
    case 0xC094E4:
        mask = 1u << (m68ki_read_imm_16() & 31u); FLAG_Z = D(0) & mask;
        D(0) &= ~mask; break;
    case 0xC094E8: case 0xC0953C: case 0xC0953E: case 0xC09558:
        value = D(opcode & 7u); D(reg) = value; flags_logic_l(value); break;
    case 0xC094EA: case 0xC094F6:
        value = m68ki_read_imm_32(); step_add_long(&D(opcode & 7u), value); break;
    case 0xC094F0: case 0xC094FC:
        value = D(opcode & 7u); step_write_long(step_displacement(A(2)), value);
        flags_logic_l(value); break;
    case 0xC09500: case 0xC09506:
        value = m68ki_read_imm_16(); address = step_displacement(A(2));
        value |= m68k_read_memory_8(address); m68k_write_memory_8(address, value);
        flags_logic_b(value); break;
    case 0xC09510:
        step_write_word(step_indexed(A(2)), 0); flags_logic_w(0); break;
    case 0xC09514:
        value = m68k_read_memory_32(step_displacement(A(1)));
        step_write_long(step_displacement(A(2)), value); flags_logic_l(value); break;
    case 0xC0951A:
        value = m68k_read_memory_16(step_displacement(A(1)));
        step_write_word(step_displacement(A(2)), value); flags_logic_w(value); break;
    case 0xC09540: case 0xC09546: case 0xC0955A: case 0xC09560:
        value = m68ki_read_imm_32(); reg = opcode & 7u;
        D(reg) &= value; flags_logic_l(D(reg)); break;
    case 0xC0956A: case 0xC0956C:
        renderer_asr_word(&D(opcode & 7u), 4); break;
    case 0xC0957A: case 0xC0957E: case 0xC095A0: case 0xC095A4:
        step_subtract_byte(&D(opcode & 7u), 3); break;
    case 0xC0957C: case 0xC09580: case 0xC095A2: case 0xC095A6:
        renderer_negate(&D(opcode & 7u), 1); break;
    case 0xC09582: case 0xC09584: case 0xC09586:
    case 0xC095A8: case 0xC095AA: case 0xC095AC:
        value = D(opcode & 7u); renderer_add_byte(&D(reg), value); break;
    case 0xC09588: case 0xC095AE:
        value = D(opcode & 7u); m68k_write_memory_8(step_displacement(A(1)), value);
        flags_logic_b(value); break;
    case 0xC0958C: case 0xC0958E:
        renderer_asr_word(&D(opcode & 7u), 2); break;
    case 0xC095B2:
        register_mask = m68ki_read_imm_16(); address = step_displacement(A(1));
        renderer_load(address, register_mask, 2, -1); break;
    case 0xC095BE:
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C09266_step(void) { return glue_C0924A_step(); }
int glue_C092A0_step(void) { return glue_C0924A_step(); }
