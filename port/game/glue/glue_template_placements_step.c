/* Complete C1D10C selector/placement parent and C1D722 column-fill timing.
 * Readable work is in template_placements.c/control_records.c/render_state.c.
 * This switch accepts only their bounded source PCs and operand families. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
extern int glue_C1D3F4_step(void);

static int template_step(uint32_t first, uint32_t end) {
    uint32_t pc = REG_PC, value, address, other;
    uint16_t opcode, mask;
    unsigned mode, reg, destination, width, count;
    if (pc < first || pc >= end) return 0;
    /* This child's source islands lie inside the parent's overall span.
     * Resume its already-proven instruction bridge before fetching here. */
    if (pc >= 0xC1D3F4u && pc < 0xC1D722u) return glue_C1D3F4_step();
    opcode = step_begin(pc);
    mode = (opcode >> 3) & 7u; reg = opcode & 7u;
    destination = (opcode >> 9) & 7u;
    switch (pc) {
    case 0xC1D10C:
        m68ki_push_32(A(reg)); A(reg) = A(7);
        A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC1D110: case 0xC1DC08: case 0xC1DC18: case 0xC1DC1C:
    case 0xC1DC2C: case 0xC1DC32: case 0xC1DC38: case 0xC1DDE8:
    case 0xC1DE04: case 0xC1E09A: case 0xC1E0AE: case 0xC1E0B0:
    case 0xC1E1B6: case 0xC1E1B8: case 0xC1E300:
        width = 1u << ((opcode >> 6) & 3u);
        cache_step_write(mode, reg, width, 0); cache_step_logic(0, width); break;
    case 0xC1D116: case 0xC1D120: case 0xC1D18E: case 0xC1D1B2:
    case 0xC1D1C2: case 0xC1D28A: case 0xC1D2A4: case 0xC1D35E:
    case 0xC1DC0C: case 0xC1DCA6: case 0xC1DD9A: case 0xC1DE0C:
    case 0xC1DE7A: case 0xC1E0C2: case 0xC1E0EE: case 0xC1E12E:
    case 0xC1E136: case 0xC1E182: case 0xC1E296: case 0xC1E2E0:
    case 0xC1E2E8:
        width = 1u << ((opcode >> 6) & 3u);
        cache_step_logic(cache_step_read(mode, reg, width), width); break;
    case 0xC1D11C: case 0xC1D126: case 0xC1D1B8: case 0xC1DCAC:
    case 0xC1DD30: case 0xC1DD5A: case 0xC1DDE6: case 0xC1DE4A:
    case 0xC1DE54: case 0xC1DEB8: case 0xC1DEF0: case 0xC1E0F4:
    case 0xC1E134: case 0xC1E188: case 0xC1E1A8: case 0xC1E1EA:
    case 0xC1E1FA: case 0xC1E2E6: case 0xC1E308: case 0xC1E310:
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D128: case 0xC1D132: case 0xC1D13A: case 0xC1D142:
    case 0xC1D14A: case 0xC1D152: case 0xC1D15A: case 0xC1D162:
    case 0xC1D16A: case 0xC1D172: case 0xC1D17A: case 0xC1D186:
    case 0xC1D1A2: case 0xC1D1BA: case 0xC1D1CA: case 0xC1D1D2:
    case 0xC1D1DA: case 0xC1D1E2: case 0xC1D1EC: case 0xC1D1F4:
    case 0xC1D1FC: case 0xC1D204: case 0xC1D20C: case 0xC1D214:
    case 0xC1D21C: case 0xC1D224: case 0xC1D22C: case 0xC1D23A:
    case 0xC1D240: case 0xC1D250: case 0xC1D254: case 0xC1D260:
    case 0xC1D26E: case 0xC1D278: case 0xC1D27C: case 0xC1D292:
    case 0xC1D29E: case 0xC1D2AE: case 0xC1D2B4: case 0xC1D2BA:
    case 0xC1D2C0: case 0xC1D2C8: case 0xC1D2D0: case 0xC1D2DE:
    case 0xC1D2EE: case 0xC1D2F6: case 0xC1D302: case 0xC1D30C:
    case 0xC1D322: case 0xC1D326: case 0xC1D32E: case 0xC1D338:
    case 0xC1D34C: case 0xC1D350: case 0xC1D366: case 0xC1D36C:
    case 0xC1D370: case 0xC1D376: case 0xC1D382: case 0xC1D388:
    case 0xC1D396: case 0xC1D39A: case 0xC1D3A0: case 0xC1D3A4:
    case 0xC1D3B8: case 0xC1D3C6: case 0xC1D3D6: case 0xC1D3E6:
    case 0xC1DBE8: case 0xC1DBF8: case 0xC1DC14: case 0xC1DC1A:
    case 0xC1DC22: case 0xC1DC44: case 0xC1DC52: case 0xC1DC56:
    case 0xC1DC5C: case 0xC1DC66: case 0xC1DC6A: case 0xC1DC94:
    case 0xC1DC9C: case 0xC1DCB4: case 0xC1DCCE: case 0xC1DCD0:
    case 0xC1DCDA: case 0xC1DCE0: case 0xC1DCE4: case 0xC1DCE8:
    case 0xC1DCF2: case 0xC1DD04: case 0xC1DD0E: case 0xC1DD1E:
    case 0xC1DD36: case 0xC1DD38: case 0xC1DD3E: case 0xC1DD46:
    case 0xC1DD54: case 0xC1DD68: case 0xC1DD76: case 0xC1DD88:
    case 0xC1DD98: case 0xC1DDA6: case 0xC1DDAE: case 0xC1DDBA:
    case 0xC1DDBE: case 0xC1DDCA: case 0xC1DDD0: case 0xC1DDF0:
    case 0xC1DE00: case 0xC1DE0A: case 0xC1DE1E: case 0xC1DE2A:
    case 0xC1DE38: case 0xC1DE3E: case 0xC1DE40: case 0xC1DE5C:
    case 0xC1DE6E: case 0xC1DE88: case 0xC1DE94: case 0xC1DEBA:
    case 0xC1DEC6: case 0xC1DED8: case 0xC1DEF2: case 0xC1DF08:
    case 0xC1DF0E: case 0xC1DF16: case 0xC1DF20: case 0xC1DF2E:
    case 0xC1DF38: case 0xC1E042: case 0xC1E04A: case 0xC1E04C:
    case 0xC1E054: case 0xC1E056: case 0xC1E05E: case 0xC1E060:
    case 0xC1E07E: case 0xC1E098: case 0xC1E09C: case 0xC1E0A0:
    case 0xC1E0A6: case 0xC1E0AA: case 0xC1E0B4: case 0xC1E0D0:
    case 0xC1E0E4: case 0xC1E0F8: case 0xC1E104: case 0xC1E144:
    case 0xC1E150: case 0xC1E152: case 0xC1E16C: case 0xC1E170:
    case 0xC1E178: case 0xC1E18E: case 0xC1E19E: case 0xC1E1B0:
    case 0xC1E1BE: case 0xC1E1C0: case 0xC1E1C6: case 0xC1E1D8:
    case 0xC1E1E0: case 0xC1E1E2: case 0xC1E1EC: case 0xC1E1F2:
    case 0xC1E202: case 0xC1E220: case 0xC1E22C: case 0xC1E23A:
    case 0xC1E23C: case 0xC1E23E: case 0xC1E242: case 0xC1E244:
    case 0xC1E248: case 0xC1E24A: case 0xC1E24E: case 0xC1E250:
    case 0xC1E254: case 0xC1E256: case 0xC1E258: case 0xC1E25C:
    case 0xC1E25E: case 0xC1E262: case 0xC1E264: case 0xC1E268:
    case 0xC1E26A: case 0xC1E26E: case 0xC1E270: case 0xC1E274:
    case 0xC1E278: case 0xC1E27C: case 0xC1E280: case 0xC1E284:
    case 0xC1E286: case 0xC1E288: case 0xC1E28A: case 0xC1E28E:
    case 0xC1E29A: case 0xC1E29E: case 0xC1E2A2: case 0xC1E2A8:
    case 0xC1E2B4: case 0xC1E2CA: case 0xC1E2F6: case 0xC1E312:
    case 0xC1E314: case 0xC1D722: case 0xC1D726: case 0xC1D72A:
    case 0xC1D72E: case 0xC1D732: case 0xC1D736: case 0xC1D73A:
    case 0xC1D73E: case 0xC1D742: case 0xC1D746: case 0xC1D74A:
    case 0xC1D74E: case 0xC1D752: case 0xC1D756: case 0xC1D75A:
    case 0xC1D75E:
        width = (opcode >> 12) == 1 ? 1 : (opcode >> 12) == 3 ? 2 : 4;
        value = cache_step_read(mode, reg, width); mode = (opcode >> 6) & 7u;
        cache_step_write(mode, destination, width, value);
        if (mode != 1) cache_step_logic(value, width); break;
    case 0xC1D130: case 0xC1D182: case 0xC1D1B0: case 0xC1D1EA:
    case 0xC1D2BE: case 0xC1D2FA: case 0xC1D30A: case 0xC1D31A:
    case 0xC1D36A: case 0xC1D394: case 0xC1D398: case 0xC1D39E:
    case 0xC1D3A2: case 0xC1D3A8: case 0xC1D3C2: case 0xC1D3D4:
    case 0xC1D3E4: case 0xC1DBF6: case 0xC1DC06: case 0xC1DD6C:
    case 0xC1DDEE: case 0xC1DDFE: case 0xC1DE66: case 0xC1DF10:
    case 0xC1DF42: case 0xC1E08A: case 0xC1E0E8: case 0xC1E11A:
    case 0xC1E14A: case 0xC1E168: case 0xC1E18C: case 0xC1E19C:
    case 0xC1E1F0: case 0xC1E276: case 0xC1E2A4: case 0xC1E2DC:
        step_branch(pc, opcode, 1); break;
    case 0xC1D194: case 0xC1D1C8: case 0xC1D290: case 0xC1D2AA:
    case 0xC1D364: case 0xC1DC12: case 0xC1DD90: case 0xC1DD96:
    case 0xC1DDA0: case 0xC1DDDC: case 0xC1DE12: case 0xC1DE80:
    case 0xC1DEAE: case 0xC1DEE6: case 0xC1E072: case 0xC1E07C:
    case 0xC1E0C8: case 0xC1E13C: case 0xC1E1CC: case 0xC1E1D4:
    case 0xC1E2EE:
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1D196: case 0xC1D344: case 0xC1DC4A: case 0xC1DCD2:
    case 0xC1DD2C: case 0xC1DF1A: case 0xC1E0DC: case 0xC1E10C:
    case 0xC1E13E: case 0xC1E1F6: case 0xC1E2BE: case 0xC1E31E:
        width = 1u << ((opcode >> 6) & 3u);
        value = width == 4 ? m68ki_read_imm_32() : m68ki_read_imm_16();
        other = cache_step_read(mode, reg, width);
        if (width == 1) step_compare_byte(value, other);
        else if (width == 2) step_compare_word(value, other);
        else step_compare_long(value, other); break;
    case 0xC1D1A0: case 0xC1D32A: case 0xC1D392: case 0xC1DF1E:
    case 0xC1E0E2: case 0xC1E1DC: case 0xC1E298:
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC1D1AA: case 0xC1D33E: case 0xC1DD34: case 0xC1E062:
    case 0xC1E0A2: case 0xC1E0BC: case 0xC1E0F6: case 0xC1E158:
    case 0xC1E15E: case 0xC1E27A: case 0xC1E27E: case 0xC1E290:
    case 0xC1E2BC: case 0xC1E2C4: case 0xC1E318:
        width = 1u << ((opcode >> 6) & 3u); value = destination ? destination : 8;
        if (mode == 1) {
            if (opcode & 0x0100u) A(reg) -= value; else A(reg) += value;
        } else {
            address = mode == 0 ? 0 : cache_step_address(mode, reg, width);
            other = mode == 0 ? D(reg) : cache_step_read_memory(address, width);
            if (opcode & 0x0100u) {
                if (width == 1) step_subtract_byte(&other, value);
                else step_subtract_word(&other, value);
            } else if (width == 1) renderer_add_byte(&other, value);
            else step_add_word(&other, value);
            if (mode == 0) cache_step_write(0, reg, width, other);
            else cache_step_write_memory(address, other, width, 0);
        }
        break;
    case 0xC1D234: case 0xC1D248: case 0xC1D258: case 0xC1D266:
    case 0xC1D272: case 0xC1D298: case 0xC1D2CA: case 0xC1D2D8:
    case 0xC1D2E6: case 0xC1D2FC: case 0xC1D330: case 0xC1D358:
    case 0xC1D3B2: case 0xC1DC26: case 0xC1DC3E: case 0xC1DC5E:
    case 0xC1DCAE: case 0xC1DD0A: case 0xC1DD5C: case 0xC1DD80:
    case 0xC1DDA8: case 0xC1DE18: case 0xC1DE56: case 0xC1DE68:
    case 0xC1DF32: case 0xC1E0CA: case 0xC1E222: case 0xC1E230:
    case 0xC1E2AA: case 0xC1E2F0: case 0xC1E2FA:
        A(destination) = cache_step_address(mode, reg, 4); break;
    case 0xC1D23E: case 0xC1D244: case 0xC1D296: case 0xC1D2A2:
    case 0xC1D2B8: case 0xC1D2C6: case 0xC1D2D4: case 0xC1D2E2:
    case 0xC1D2F4: case 0xC1D306: case 0xC1D31C: case 0xC1D34A:
    case 0xC1D354: case 0xC1D374: case 0xC1D386: case 0xC1D3BA:
    case 0xC1DC50: case 0xC1DC5A: case 0xC1DC6E: case 0xC1DC70:
    case 0xC1DCBA: case 0xC1DCDE: case 0xC1DCE6: case 0xC1DD00:
    case 0xC1DDB4: case 0xC1DE24: case 0xC1DF40: case 0xC1E21E:
    case 0xC1E2A6:
        SET_W(D(reg), (int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC1D246: case 0xC1D2AC: case 0xC1D2D6: case 0xC1D2E4:
    case 0xC1DC9A: case 0xC1DCCA: case 0xC1DCCC: case 0xC1DCFC:
    case 0xC1DCFE: case 0xC1DD02: case 0xC1DDC2: case 0xC1DE30:
    case 0xC1DE62: case 0xC1DE74: case 0xC1E210: case 0xC1E212:
    case 0xC1E214: case 0xC1E236:
        count = (opcode & 0x20u) ? D(destination) & 63u : destination ? destination : 8;
        if (opcode & 0x40u) renderer_asl_word(&D(reg), count); else step_asl_long(&D(reg), count);
        break;
    case 0xC1D24E: case 0xC1D25E: case 0xC1D26C: case 0xC1D2EC:
    case 0xC1D32C: case 0xC1D3BC: case 0xC1D3BE: case 0xC1DD86:
    case 0xC1DE64: case 0xC1DE78: case 0xC1E116: case 0xC1E14C:
    case 0xC1E164: case 0xC1E1AC: case 0xC1E20A: case 0xC1E238:
    case 0xC1E2D8: case 0xC1E31A: case 0xC1D724: case 0xC1D728:
    case 0xC1D72C: case 0xC1D730: case 0xC1D734: case 0xC1D738:
    case 0xC1D73C: case 0xC1D740: case 0xC1D744: case 0xC1D748:
    case 0xC1D74C: case 0xC1D750: case 0xC1D754: case 0xC1D758:
    case 0xC1D75C: case 0xC1D760:
        value = (uint32_t)(int32_t)(int16_t)cache_step_read(mode, reg, 2);
        if ((opcode >> 12) == 9) A(destination) -= value; else A(destination) += value;
        break;
    case 0xC1D256: case 0xC1D264: case 0xC1D308: case 0xC1D31E:
    case 0xC1D320: case 0xC1D356: case 0xC1D37A: case 0xC1D38C:
    case 0xC1DC64: case 0xC1DC72: case 0xC1DC7E: case 0xC1DC92:
    case 0xC1DCBC: case 0xC1DCBE: case 0xC1DCC6: case 0xC1DCC8:
    case 0xC1DCEE: case 0xC1DCF0: case 0xC1DD06: case 0xC1DD08:
    case 0xC1DD72: case 0xC1DD74: case 0xC1DD78: case 0xC1DD7A:
    case 0xC1DD7C: case 0xC1DDA2: case 0xC1DDA4: case 0xC1DDB6:
    case 0xC1DDB8: case 0xC1DDC4: case 0xC1DDC6: case 0xC1DDC8:
    case 0xC1DE14: case 0xC1DE16: case 0xC1DE26: case 0xC1DE28:
    case 0xC1DE32: case 0xC1DE34: case 0xC1DE36: case 0xC1DE76:
    case 0xC1DE92: case 0xC1DE9C: case 0xC1DEC4: case 0xC1DECE:
    case 0xC1DEF6: case 0xC1E0EA: case 0xC1E0EC: case 0xC1E228:
    case 0xC1E22A: case 0xC1E240: case 0xC1E246: case 0xC1E24C:
    case 0xC1E252: case 0xC1E25A: case 0xC1E260: case 0xC1E266:
    case 0xC1E26C: case 0xC1E282: case 0xC1E28C: case 0xC1E2B0:
    case 0xC1E2B2:
        width = (opcode & 0x40u) ? 2 : 4; value = cache_step_read(mode, reg, width);
        if ((opcode >> 12) == 9) {
            if (width == 2) step_subtract_word(&D(destination), value);
            else step_subtract_long(&D(destination), value);
        } else if (width == 2) step_add_word(&D(destination), value);
        else step_add_long(&D(destination), value); break;
    case 0xC1D280: case 0xC1D336: case 0xC1DD20: case 0xC1DD8A:
    case 0xC1DDD2: case 0xC1E068:
        D(destination) = (uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC1D282: case 0xC1D3B4:
        value = (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16();
        m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC1D286: case 0xC1E292:
        step_dbf(pc, &D(reg)); break;
    case 0xC1D314: case 0xC1D3CE: case 0xC1D3DE: case 0xC1DBF0:
    case 0xC1DC00: case 0xC1DDF8: case 0xC1DF28: case 0xC1E196:
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC1D33A: case 0xC1D340: case 0xC1D37C: case 0xC1D38E:
    case 0xC1DC46: case 0xC1DD4E: case 0xC1E114: case 0xC1E1E4:
    case 0xC1E206: case 0xC1E322:
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1D348: case 0xC1D380: case 0xC1DC4E: case 0xC1DCD6:
    case 0xC1E142: case 0xC1E160: case 0xC1E1A6: case 0xC1E272:
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC1D37E: case 0xC1D390: case 0xC1DF04: case 0xC1DF0A:
    case 0xC1DF12: case 0xC1E0FC: case 0xC1E1A4: case 0xC1E2D0:
        width = 1u << ((opcode >> 6) & 3u); value = cache_step_read(mode, reg, width);
        if (width == 1) step_compare_byte(value, D(destination));
        else if (width == 2) step_compare_word(value, D(destination));
        else step_compare_long(value, D(destination)); break;
    case 0xC1D3EE:
        REG_PC = m68ki_read_imm_32(); break;
    case 0xC1DC78: case 0xC1DC84: case 0xC1DD28: case 0xC1DE9E:
    case 0xC1DED0: case 0xC1DEFC: case 0xC1DF06: case 0xC1DF0C:
    case 0xC1DF14: case 0xC1E0A4: case 0xC1E102: case 0xC1E2C2:
    case 0xC1E2D6:
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1DC7A: case 0xC1DC86: case 0xC1DD14: case 0xC1E108:
        width = 1u << ((opcode >> 6) & 3u);
        value = width == 4 ? m68ki_read_imm_32() : m68ki_read_imm_16();
        address = mode == 0 ? 0 : cache_step_address(mode, reg, width);
        other = mode == 0 ? D(reg) : cache_step_read_memory(address, width);
        if (width == 2) step_add_word(&other, value); else step_add_long(&other, value);
        if (mode == 0) cache_step_write(0, reg, width, other);
        else cache_step_write_memory(address, other, width, 0); break;
    case 0xC1DC8A: case 0xC1DD4C: case 0xC1E080:
        count = (opcode & 0x20u) ? D(destination) & 63u : destination ? destination : 8;
        renderer_asl_word(&D(reg), count); FLAG_V = 0; break;
    case 0xC1DC8C: case 0xC1DD3A: case 0xC1DD62: case 0xC1DE8C:
    case 0xC1DEBE: case 0xC1E1E6: case 0xC1E1FE: case 0xC1E20C:
    case 0xC1E216: case 0xC1E21A:
        width = 1u << ((opcode >> 6) & 3u);
        value = width == 4 ? m68ki_read_imm_32() : m68ki_read_imm_16();
        other = cache_step_read(mode, reg, width) & value;
        cache_step_write(0, reg, width, other); cache_step_logic(other, width); break;
    case 0xC1DCA0: case 0xC1DCC0: case 0xC1DCF6: case 0xC1E1C2:
        mask = m68ki_read_imm_16();
        address = mode == 3 ? A(reg) : cache_step_address(mode, reg, 2);
        renderer_load(address, mask, 2, mode == 3 ? (int)reg : -1); break;
    case 0xC1DD22:
        step_compare_long(cache_step_read(mode, reg, 4), A(destination)); break;
    case 0xC1DD44: case 0xC1DD66: case 0xC1DD6E: case 0xC1DF18:
        count = (opcode & 0x20u) ? D(destination) & 63u : destination ? destination : 8;
        SET_W(D(reg), step_lsr_word_value(D(reg), count)); break;
    case 0xC1DD52: case 0xC1DF3C: case 0xC1E084: case 0xC1E08C:
        width = 1u << ((opcode >> 6) & 3u);
        if (opcode & 0x0100u) {
            address = mode == 0 ? 0 : cache_step_address(mode, reg, width);
            value = (mode == 0 ? D(reg) : cache_step_read_memory(address, width)) | D(destination);
            if (mode == 0) cache_step_write(0, reg, width, value);
            else cache_step_write_memory(address, value, width, 0);
        } else { value = cache_step_read(mode, reg, width) | D(destination);
            cache_step_write(0, destination, width, value); }
        cache_step_logic(value, width); break;
    case 0xC1DD56: case 0xC1DD8C: case 0xC1DD92: case 0xC1DDD4:
    case 0xC1DDDE: case 0xC1DE42: case 0xC1DE4C: case 0xC1DE82:
    case 0xC1DEA6: case 0xC1DEB0: case 0xC1DEDE: case 0xC1DEE8:
    case 0xC1E06A: case 0xC1E074: case 0xC1E094: case 0xC1E1C8:
    case 0xC1E1D0: case 0xC1E302: case 0xC1E30A:
        count = m68ki_read_imm_16() & (mode == 0 ? 31u : 7u);
        address = mode == 0 ? 0 : cache_step_address(mode, reg, 1);
        value = mode == 0 ? D(reg) : cache_step_read_memory(address, 1);
        FLAG_Z = value & (1u << count);
        if (opcode & 0x0080u) {
            if (opcode & 0x0040u) value |= 1u << count; else value &= ~(1u << count);
            if (mode == 0) D(reg) = value; else cache_step_write_memory(address, value, 1, 0);
        }
        break;
    case 0xC1DD70: case 0xC1DD7E: case 0xC1E082: case 0xC1E0B2:
    case 0xC1E0BA:
        step_swap(&D(reg)); break;
    case 0xC1DDC0: case 0xC1DE2E: case 0xC1DE90: case 0xC1DE9A:
    case 0xC1DEC2: case 0xC1DECC:
        D(reg) = (uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC1DEA0: case 0xC1DED2: case 0xC1DEFE: case 0xC1E0D6:
        renderer_negate(&D(reg), 4); break;
    case 0xC1DEA2: case 0xC1DEA4: case 0xC1DED4: case 0xC1DED6:
    case 0xC1DF00: case 0xC1DF02: case 0xC1E048: case 0xC1E052:
    case 0xC1E05C: case 0xC1E0D8: case 0xC1E0DA:
        count = (opcode & 0x20u) ? D(destination) & 63u : destination ? destination : 8;
        if (opcode & 0x40u) renderer_asr_word(&D(reg), count); else step_asr_long(&D(reg), count);
        break;
    case 0xC1E092:
        count = destination ? destination : 8;
        value = D(reg); other = count & 31u;
        if (other) value = (value >> other) | (value << (32 - other));
        D(reg) = value; flags_logic_l(value); FLAG_C = (value >> 31) << 8;
        USE_CYCLES(count << CYC_SHIFT); break;
    case 0xC1E29C: case 0xC1E2A0:
        value = D(destination); D(destination) = A(reg); A(reg) = value; break;
    case 0xC1E324:
        A(7) = A(reg); A(reg) = m68ki_pull_32(); break;
    case 0xC1E326: case 0xC1D762:
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C1D10C_step(void) { return template_step(0xC1D10C, 0xC1E328); }
int glue_C1D722_step(void) { return template_step(0xC1D722, 0xC1D764); }
