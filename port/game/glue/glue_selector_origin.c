#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "selector_origin.h"
#include "globals.h"
#include <stdlib.h>

typedef struct { int small, scan, preset_call; } OriginCPU;
static void load(unsigned first,gaddr address,unsigned count) {
    unsigned i; for(i=0;i<count;++i) D(first+i)=rd_u32(address+4*i);
}
static void save(unsigned first,unsigned last) {
    unsigned i=last+1; while(i>first) step_predecrement_long(D(--i));
}
static void restore(unsigned first,unsigned last) {
    unsigned i; for(i=first;i<=last;++i) { D(i)=rd_u32(A(7)); A(7)+=4; }
}
static void halve_outputs(void) {
    unsigned i; for(i=0;i<3;++i) step_asr_long(&D(i),1);
}
static void add_saved(void) {
    unsigned i; for(i=0;i<3;++i) step_add_long(&D(i),D(i+3));
    halve_outputs();
}
static void origin_outputs(void *context,const SelectorOriginEvent *e) {
    OriginCPU *cpu=context; unsigned i;
    switch(e->phase) {
    case ORIGIN_BYTE_TEST: flags_logic_b(e->value); break;
    case ORIGIN_BYTE_COMPARE: step_compare_byte(e->previous,e->value); break;
    case ORIGIN_BYTE_PUBLISH: flags_logic_b(e->value); break;
    case ORIGIN_WORD_PUBLISH: flags_logic_w(e->value); break;
    case ORIGIN_LONG_COMPARE: step_compare_long(e->previous,e->value); break;
    case ORIGIN_THRESHOLD:
        D(4)=e->parameter; flags_logic_l(D(4));
        if(e->previous) step_compare_long(e->previous,e->value);
        break;
    case ORIGIN_DIRECT:
        A(0)=e->address; D(5)=rd_u32(A(0)+0x14); D(6)=rd_u32(SELECTOR_ORIGIN_MIDDLE); D(7)=rd_u32(A(0)+0x1c);
        flags_logic_l(D(7)); break;
    case ORIGIN_ANGLE:
        A(0)=e->address; SET_B(D(0),rd_u8(A(0)+0x62)&0xf0u); SET_B(D(7),D(0));
        step_compare_byte(0x30,D(0));
        if((uint8_t)D(0)!=0x30) {
            SET_W(D(0),e->value); SET_W(D(1),e->previous);
            step_subtract_word(&D(1),D(0));
            if(COND_LT()) renderer_negate(&D(1),2);
            flags_logic_w(D(0)); step_compare_word(0x1c20,D(1));
            if(COND_GE()) {
                step_compare_word(0x5460,D(1));
                if(COND_LE()) {
                    SET_B(D(0),e->parameter); flags_logic_b(D(0));
                    if((uint8_t)D(0)) {
                        step_compare_byte(8,D(0));
                        if(COND_LT()) {
                            step_compare_byte(4,D(0));
                            if((uint8_t)D(0)!=4) {
                                renderer_add_byte(&D(0),4); step_compare_byte(8,D(0));
                                if(COND_GE()) step_subtract_byte(&D(0),8);
                                flags_logic_b(D(0));
                            }
                        }
                    }
                }
            }
        }
        break;
    case ORIGIN_MATRIX_INPUTS:
        A(1)=e->address; SET_B(D(0),e->value); SET_W(D(0),(int8_t)D(0)); flags_logic_w(D(0));
        step_add_word(&D(0),D(0)); SET_W(D(1),D(0));
        step_add_word(&D(0),D(0)); step_add_word(&D(0),D(1));
        for(i=0;i<3;++i) D(3+i)=(uint32_t)(int32_t)rd_s16(A(1)+(uint32_t)(int32_t)(int16_t)D(0)+2*i);
        SET_B(D(0),e->previous); flags_logic_b(D(0));
        if((int8_t)D(0)>=0) {
            SET_W(D(0),(int8_t)D(0)); step_compare_word(1,D(0));
            if(COND_GT()) {
                step_subtract_word(&D(0),1);
                for(i=3;i<6;++i) renderer_asl_word(&D(i),D(0));
            } else {
                for(i=0;i<3;++i) SET_W(D(i),D(3+i));
                for(i=0;i<3;++i) renderer_asr_word(&D(i),1);
                for(i=0;i<3;++i) step_add_word(&D(3+i),D(i));
            }
        }
        break;
    case ORIGIN_MATRIX_SAVE:
        step_predecrement_long(A(0)); flags_logic_l(A(0)); SET_B(D(0),e->previous);
        step_compare_byte(0x30,D(7));
        if((uint8_t)D(7)!=0x30) {
            flags_logic_b(D(0)); if((uint8_t)D(0)) step_compare_byte(4,D(0));
        }
        break;
    case ORIGIN_MATRIX_RESULT:
        load(5,ORIGIN_CANDIDATE_TRIPLE,3); A(0)=rd_u32(A(7)); A(7)+=4; break;
    case ORIGIN_FLOOR:
        SET_B(D(0),rd_u8(A(0)+4)); SET_B(D(0),D(0)&0xc0u); flags_logic_b(D(0));
        if((uint8_t)D(0)) SET_W(D(0),rd_u16(A(0)+0x4e)); else D(0)=0;
        step_add_word(&D(0),7); D(0)=(uint32_t)(int32_t)(int16_t)D(0); step_asl_long(&D(0),8);
        step_compare_long(D(0),D(6));
        if(COND_LT()) { D(6)=D(0); flags_logic_l(D(6)); } break;
    case ORIGIN_DELTAS:
        load(5,ORIGIN_CANDIDATE_TRIPLE,3); load(0,SELECTOR_ORIGIN,3);
        for(i=0;i<3;++i) {
            step_subtract_long(&D(5+i),D(i)); D(i)=D(5+i); flags_logic_l(D(i));
            if((int32_t)D(i)<0) renderer_negate(&D(i),4);
        }
        step_compare_long(D(0),D(1));
        if(COND_GT()) { step_compare_long(D(1),D(2)); D(3)=COND_LE()?D(1):D(2); }
        else { step_compare_long(D(0),D(2)); D(3)=COND_GT()?D(2):D(0); }
        flags_logic_l(D(3)); break;
    case ORIGIN_DISPATCH:
        SET_B(D(0),e->value); SET_W(D(0),(int8_t)D(0)); renderer_asl_word(&D(0),2);
        A(0)=rd_u32(ORIGIN_MODE_TABLE+(uint32_t)(int32_t)(int16_t)D(0)); break;
    case ORIGIN_BLEND_SAVE: save(3,6); break;
    case ORIGIN_BLEND_START:
        A(2)=e->address; load(0,SELECTOR_ORIGIN,3);
        for(i=0;i<3;++i) step_add_long(&D(i),rd_u32(A(2)+0x14+4*i));
        halve_outputs(); break;
    case ORIGIN_BLEND_FIRST:
        save(0,2); step_add_long(&D(0),rd_u32(SELECTOR_ORIGIN)); step_add_long(&D(2),rd_u32(SELECTOR_ORIGIN_THIRD));
        halve_outputs(); restore(3,5); break;
    case ORIGIN_BLEND_SECOND: add_saved(); save(0,2); break;
    case ORIGIN_BLEND_THIRD: add_saved(); restore(3,5); break;
    case ORIGIN_BLEND_LAST: add_saved(); restore(3,6); break;
    case ORIGIN_PRESET_SAVE: step_predecrement_long(0xc2948cu); cpu->preset_call=1; break;
    case ORIGIN_PRESET_RESULT:
        load(0,e->address,3); if(cpu->preset_call) { A(7)+=4; cpu->preset_call=0; } break;
    case ORIGIN_SMALL_SAVE: save(3,7); cpu->small=1; break;
    case ORIGIN_SMALL_INPUTS:
        D(4)=e->parameter?1:4;
        if(e->parameter) D(5)=6;
        if(e->value==1 || e->value==2) {
            D(3)=6; if(!e->parameter) D(5)=3;
        } else { D(3)=0xfffffffau; if(!e->parameter) D(5)=0xfffffff7u; }
        flags_logic_l(e->parameter?D(3):D(5)); break;
    case ORIGIN_SMALL_RESTORE: restore(3,7); cpu->small=0; break;
    case ORIGIN_COUNTDOWN:
        { uint32_t value=e->previous; step_subtract_byte(&value,1); } break;
    case ORIGIN_STATUS: flags_logic_w(e->value); break;
    case ORIGIN_REDUCE:
        for(i=5;i<8;++i) step_asr_long(&D(i),2);
        step_asr_long(&D(3),2); break;
    case ORIGIN_NORMALIZE_SAVE:
        A(7)-=2; wr_u16(A(7),(uint16_t)D(4)); flags_logic_w(D(4));
        SET_W(D(0),0x200); flags_logic_w(D(0)); break;
    case ORIGIN_NORMALIZE_RESULT:
        SET_W(D(4),rd_u16(A(7))); A(7)+=2; flags_logic_w(D(4));
        for(i=5;i<8;++i) { D(i)=(uint32_t)(int32_t)(int16_t)D(i); flags_logic_l(D(i)); }
        for(i=5;i<8;++i) step_asl_long(&D(i),D(4));
        break;
    case ORIGIN_SMOOTH:
        load(0,ORIGIN_SMOOTHED_DELTA,3); D(3)=D(0)|D(1)|D(2); flags_logic_l(D(3));
        if(D(3)) {
            for(i=0;i<3;++i) step_subtract_long(&D(5+i),D(i));
            D(4)=1;
            for(i=5;i<8;++i) step_asr_long(&D(i),D(4));
            for(i=0;i<3;++i) step_add_long(&D(5+i),D(i));
        } break;
    case ORIGIN_ADD:
        load(0,SELECTOR_ORIGIN,3); for(i=0;i<3;++i) step_add_long(&D(5+i),D(i)); break;
    case ORIGIN_COMPANION:
        D(5)&=0x3fffffu; D(7)&=0x3fffffu;
        for(i=5;i<8;++i) renderer_negate(&D(i),4);
        break;
    case ORIGIN_SCAN_START: cpu->scan=1; A(0)=CONTROL_RECORDS; A(2)=e->address; break;
    case ORIGIN_SCAN_TEST: flags_logic_w(e->value); break;
    case ORIGIN_SCAN_RECORD:
        /* Retain both source word operations and their carry/extend output. */
        SET_W(D(3),rd_u16(A(2)+4)); A(2)=e->address;
        { uint16_t old=(uint16_t)D(3); SET_W(D(3),old<<8); flags_logic_w(D(3)); FLAG_X=FLAG_C=((old>>8)&1u)<<8; }
        step_add_word(&D(3),D(3)); SET_B(D(1),e->previous&0xf0u); flags_logic_b(D(1));
        step_compare_byte(0x10,D(1)); break;
    case ORIGIN_SCAN_ACTIVE: FLAG_Z=e->value&0x40u; break;
    case ORIGIN_SCAN_SELECTED: flags_logic_w(e->value); FLAG_Z=rd_u8(e->address+1)&8u; break;
    case ORIGIN_SCAN_FINISH: flags_logic_w(e->value); break;
    case ORIGIN_SCAN_RESULT:
        SET_W(D(3),e->value);
        D(0)=rd_u32(A(0)+(uint32_t)(int32_t)(int16_t)D(3)+0x14);
        D(2)=rd_u32(A(0)+(uint32_t)(int32_t)(int16_t)D(3)+0x1c);
        D(1)=rd_u32(SELECTOR_ORIGIN_MIDDLE); D(3)=D(1); step_asr_long(&D(3),2);
        step_subtract_long(&D(1),D(3)); break;
    }
}
static SelectorOriginTriple consume(void *context,enum SelectorOriginChild child,
                                   const SelectorOriginTriple *inputs) {
    OriginCPU *cpu=context; SelectorOriginTriple result; unsigned i;
    /* The observer materializes source high words and CCR independently.
     * Validate the readable producer's child inputs at that real boundary. */
    if(inputs) for(i=0;i<3;++i) {
        unsigned first=child==ORIGIN_NORMALIZE?5:3;
        if(child==ORIGIN_NORMALIZE) { if(D(first+i)!=inputs->component[i]) abort(); }
        else if((uint16_t)D(first+i)!=(uint16_t)inputs->component[i]) abort();
    }
    switch(child) {
    case ORIGIN_PREPARE: glue_complete_child(0xc2daf2u,0xc29048u); break;
    case ORIGIN_MATRIX_A: glue_complete_child(0xc091a8u,0xc2918eu); break;
    case ORIGIN_MATRIX_B: glue_complete_child(0xc091ceu,cpu->small?0xc294fau:0xc29196u); break;
    case ORIGIN_REGENERATE: glue_complete_child(0xc0910cu,cpu->scan?0xc293dcu:0xc2933cu); break;
    case ORIGIN_FALLBACK: glue_complete_child(0xc06c02u,0xc293d6u); break;
    case ORIGIN_NORMALIZE: glue_complete_child(0xc2574au,0xc29566u); break;
    }
    for(i=0;i<3;++i) result.component[i]=D((child==ORIGIN_NORMALIZE?5:0)+i);
    return result;
}
int glue_C29042(void) {
    OriginCPU cpu={0}; SelectorOriginHooks hooks={consume,origin_outputs,&cpu};
    publish_selector_origin(&hooks); return glue_return();
}
/* Shared cold bodies remain parts of the original owner. Register an
 * independent entry only with original external call-site evidence. */
int glue_origin_control_record(void) {
    OriginCPU cpu={0}; SelectorOriginHooks hooks={consume,origin_outputs,&cpu};
    select_origin_control_record(&hooks); return glue_return();
}
int glue_origin_candidate_preset(uint32_t preset) {
    OriginCPU cpu={0}; SelectorOriginHooks hooks={consume,origin_outputs,&cpu};
    load_origin_candidate_preset(&hooks,preset); return glue_return();
}
