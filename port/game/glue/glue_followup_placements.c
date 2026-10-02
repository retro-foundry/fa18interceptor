/* C1CCBC and the complete shared C1D0A4/C1D0B6 position body. */
#include "glue.h"
#include "glue_child_call.h"
#include "followup_placements.h"
#include "globals.h"

void target_distance_registers(int apply,int32_t result);
void append_list_point_registers(int apply);

static void position_outputs(gaddr owner,uint16_t shift,const int32_t point[3]) {
    uint32_t level=rd_u32(owner+0x18), bias=rd_u32(POSITION_BIAS), sum=level+bias;
    D(1)=rd_u32(POSITION_LEVEL); D(2)=(uint32_t)point[0];
    D(3)=(uint32_t)point[1]; D(4)=(uint32_t)point[2];
    SET_W(D(6),shift); A(2)=owner;
    flags_logic_b(1);
    FLAG_X=shift ? ((sum>>(shift-1))&1u)<<8 : (sum<level)<<8;
}
int glue_selected_position(int workspace) {
    int32_t point[3]={(int32_t)D(2),0,(int32_t)D(4)};
    uint16_t header=(uint16_t)D(7);
    gaddr owner=accumulate_selected_position(workspace,header,point);
    position_outputs(owner,header&15u,point);
    return glue_return();
}
int glue_C1D0A4(void) { return glue_selected_position(1); }

static void followup_outputs(void *context,const FollowupPlacementEvent *e) {
    unsigned i;
    (void)context;
    switch(e->phase) {
    case FOLLOWUP_SELECTED_SCAN:
        A(0)=e->record; SET_W(D(0),e->index); break;
    case FOLLOWUP_SELECTED_METRICS:
        A(0)=e->record; D(0)=(uint32_t)e->terms[2];
        D(2)=(uint32_t)e->point[0]; D(4)=(uint32_t)e->point[2];
        D(6)=(uint32_t)e->terms[1]; D(7)=(uint32_t)e->terms[0]; SET_W(D(7),e->value); break;
    case FOLLOWUP_SELECTED_POINT:
        A(1)=0xc1df46u; SET_W(D(7),e->header); SET_W(D(6),e->shift);
        D(0)=(uint32_t)e->value;
        for(i=0;i<3;++i) D(2+i)=(uint32_t)e->point[i]; break;
    case FOLLOWUP_DISTANCE_BEGIN:
        if (!e->selected) for(i=0;i<3;++i) D(2+i)=(uint32_t)e->point[i]; break;
    case FOLLOWUP_DISTANCE_END: target_distance_registers(0,e->value); break;
    case FOLLOWUP_ALTERNATE_SCAN:
        A(0)=e->record+2; SET_W(D(0),e->index); SET_W(D(7),e->header); break;
    case FOLLOWUP_ALTERNATE_DESCRIPTOR:
        SET_B(D(0),e->header); SET_W(D(0),D(0)&15u);
        A(0)=e->record+6; A(1)=e->descriptor; break;
    case FOLLOWUP_ALTERNATE_POINT:
        A(0)=e->record+12; SET_W(D(1),e->header>>8);
        for(i=0;i<3;++i) D(2+i)=(uint32_t)e->point[i]; break;
    case FOLLOWUP_ALTERNATE_PACKED:
        if (e->transformed) {
            /* Source helper outputs packed values before the parent's ASRs. */
            int32_t packed[3];
            for(i=0;i<3;++i) packed[i]=rd_s32(0xc45b30u+4*i);
            position_outputs((gaddr)e->value,e->header&15u,packed);
        }
        for(i=0;i<3;++i) D(2+i)=(uint32_t)e->point[i]; break;
    case FOLLOWUP_CACHE: D(1)=(uint32_t)e->value; break;
    case FOLLOWUP_REFRESH_PHASE:
        SET_W(D(0),e->value); SET_W(D(1),e->header); break;
    case FOLLOWUP_DISPATCH:
        if(e->selected) {
            uint16_t small=(uint16_t)(e->index*4u), large=(uint16_t)(e->index*16u);
            SET_W(D(7),small); SET_W(D(1),(uint16_t)(large+small));
            FLAG_X=((uint32_t)large+small>0xffffu)<<8;
        } else {
            SET_W(D(1),e->value); SET_W(D(7),(e->header&0xff00u)*2u);
            FLAG_X=((e->header&0xff00u)*2u>0xffffu)<<8;
        }
        A(1)=e->descriptor+16; A(2)=e->routine; A(0)=e->parameters;
        flags_logic_l(rd_u32(0xc45a3au)); break;
    case FOLLOWUP_RESULT:
        A(0)=0xc4f6cau; SET_W(D(1),e->index); D(0)=(uint32_t)e->value; break;
    case FOLLOWUP_CLOCK:
        SET_W(D(0),e->value&3u); SET_W(D(1),e->value&2u); break;
    case FOLLOWUP_RELATIVE_BEGIN: A(1)=CONTROL_RECORDS; D(0)=0; break;
    case FOLLOWUP_RELATIVE_SCAN: SET_W(D(1),e->header); break;
    case FOLLOWUP_RELATIVE_POINT:
        for(i=0;i<3;++i) D(2+i)=(uint32_t)e->point[i];
        D(1)=e->shift; D(5)=(uint32_t)e->terms[0]; SET_W(D(5),e->shift+8u);
        D(6)=(uint32_t)e->terms[1]; D(7)=(uint32_t)e->value; SET_W(D(7),(e->index>>1)|16u);
        flags_logic_w(D(7)); FLAG_Z=0; FLAG_X=FLAG_C=0; break;
    case FOLLOWUP_RELATIVE_STORED: append_list_point_registers(0); break;
    case FOLLOWUP_RELATIVE_ADVANCE: SET_W(D(0),e->index); break;
    }
}
static int32_t consume(void *context,const FollowupPlacementEvent *call) {
    (void)context;
    return glue_complete_child(call->routine,call->selected ? 0xc1ce2eu : 0xc1cfa8u);
}
int glue_C1CCBC(void) {
    FollowupPlacementHooks hooks={consume,followup_outputs,NULL};
    visit_followup_placements(&hooks);
    return glue_return();
}
