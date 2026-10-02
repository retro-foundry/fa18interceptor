/* Complete C1CCBC through C1D0A2; children C1D0A4/C1D0B6. */
#include "followup_placements.h"
#include "control_records.h"
#include "fixed_math.h"
#include "view_transform.h"
#include "fault.h"
#include "globals.h"

enum {
    OFFSET=0xC459AA, SELECTED_LIST=0xC4E98A, ALTERNATE_LIST=0xC4F6CA,
    ALTERNATE_OFFSET=0xC459B0, KIND=0xC459B4, SCALED_KIND=0xC459B6,
    SELECTED_KIND=0xC458DE, MODE=0xC45785, CLOCK=0xC458DA,
    HEADER=0xC4585B, VISIT=0xC458BD, GATE=0xC45ABA,
    EXTRA=0xC45932, POINT=0xC45B30, CONTROL=0xC45A36, AUX=0xC45A3A
};
static int32_t add32(int32_t a, int32_t b) { return (int32_t)((uint32_t)a+(uint32_t)b); }
static int32_t sub32(int32_t a, int32_t b) { return (int32_t)((uint32_t)a-(uint32_t)b); }
static int32_t absolute(int32_t a) { return a < 0 ? (int32_t)(0u-(uint32_t)a) : a; }
static int32_t shifted(int32_t a, unsigned n) {
    n &= 63u; return n >= 32 ? (a < 0 ? -1 : 0) : a >> n;
}
static void emit(const FollowupPlacementHooks *h, FollowupPlacementEvent e) {
    if (h->observe) h->observe(h->context, &e);
}
gaddr accumulate_selected_position(int workspace, uint16_t header, int32_t point[3]) {
    int16_t offset = (int16_t)(header & 0xff00u);
    gaddr record = workspace ? WORKSPACE_RECORDS+(gaddr)(int32_t)(offset >> 3)
                            : control_record(header);
    accumulate_record_position(record, header & 15u, &point[0], &point[1], &point[2]);
    return record;
}
static void distance(const FollowupPlacementHooks *h, gaddr cached) {
    FollowupPlacementEvent e = {.phase=FOLLOWUP_DISTANCE_BEGIN,.selected=!cached};
    unsigned i;
    for (i=0;i<3;++i) e.point[i]=rd_s16(BOUND_OFFSET_X+2*i);
    emit(h,e);
    e.value=target_distance((int16_t)e.point[0],(int16_t)e.point[1],(int16_t)e.point[2]);
    e.phase=FOLLOWUP_DISTANCE_END; emit(h,e);
    if (cached) wr_s16(cached,(int16_t)e.value);
}
static int32_t dispatch(const FollowupPlacementHooks *h, FollowupPlacementEvent e) {
    e.phase=FOLLOWUP_DISPATCH;
    e.routine=rd_u32(e.descriptor); e.parameters=rd_u32(e.descriptor+4);
    wr_u32(CONTROL,rd_u32(e.descriptor+8)); wr_u32(AUX,rd_u32(e.descriptor+12));
    emit(h,e);
    return h->consume(h->context,&e);
}
static void selected_records(const FollowupPlacementHooks *h) {
    for (;;) {
        gaddr cursor=SELECTED_LIST+(gaddr)(int32_t)rd_s16(OFFSET);
        int16_t index=rd_s16(cursor);
        FollowupPlacementEvent e={.phase=FOLLOWUP_SELECTED_SCAN,.record=cursor,.index=(uint16_t)index};
        int32_t raw_x,raw_z,mx,mz,my;
        uint16_t half;
        unsigned i;
        emit(h,e); if (index <= 0) return;
        wr_s16(KIND,index); wr_u16(SCALED_KIND,(uint16_t)(index*512u));
        e.record=CONTROL_RECORDS+(gaddr)(int32_t)(int16_t)(index*512u);
        raw_x=add32((int32_t)((uint32_t)(uint16_t)(rd_u16(e.record+6)-rd_u16(0xC4594C))<<22),
                    (int32_t)(rd_u32(e.record+0x14)&0x3fffffu));
        raw_z=add32((int32_t)((uint32_t)(uint16_t)(rd_u16(e.record+8)-rd_u16(0xC4594E))<<22),
                    (int32_t)(rd_u32(e.record+0x1c)&0x3fffffu));
        mx=absolute(add32(raw_x>>8,rd_s16(0xC45A72)))>>12;
        mz=absolute(add32(raw_z>>8,rd_s16(0xC45A76)))>>12;
        /* ADD/BGE uses the mathematical signed sum through N/V. The depth
         * can overflow even though x/z terms are bounded after their shift. */
        { int32_t a=rd_s32(e.record+0x10), b=rd_s32(0xC45A78);
          my=add32(a,b); if ((int64_t)a+b < 0) my=(int32_t)(0u-(uint32_t)my); my >>= 11; }
        { int16_t maximum=(int16_t)mx;
          if ((int16_t)mz>maximum) maximum=(int16_t)mz;
          if ((int16_t)my>maximum) maximum=(int16_t)my;
          half=(uint16_t)maximum>>1; }
        e.phase=FOLLOWUP_SELECTED_METRICS;
        e.point[0]=raw_x; e.point[2]=raw_z;
        e.terms[0]=mx; e.terms[1]=mz; e.terms[2]=my; e.value=half;
        emit(h,e);
        if (half>0xefu) { wr_u16(ERROR_CODE,0x29); fault_hook(); half=0xef; }
        e.header=half;
        e.shift=(uint16_t)(int16_t)rd_s8(0xC1DF46+(gaddr)(int32_t)(int16_t)half);
        wr_u16(BOUND_SHIFT,e.shift);
        e.value=shifted(add32(rd_s32(e.record+0x18),rd_s32(POSITION_BIAS)),e.shift);
        wr_s32(POSITION_LEVEL,e.value); wr_u8(POSITION_VALID,1);
        e.point[0]=shifted(raw_x,e.shift); e.point[1]=shifted(rd_s32(e.record+0x18),e.shift);
        e.point[2]=shifted(raw_z,e.shift);
        for (i=0;i<3;++i) wr_s32(POINT+4*i,e.point[i]);
        for (i=0;i<3;++i) { e.point[i] >>= 8; wr_s16(BOUND_OFFSET_X+2*i,(int16_t)e.point[i]); }
        e.phase=FOLLOWUP_SELECTED_POINT; emit(h,e); distance(h,0);
        e.selected=1; e.index=rd_u16(KIND);
        e.descriptor=0xC22188+(gaddr)(int32_t)(int16_t)(e.index*20u);
        dispatch(h,e);
        wr_u16(OFFSET,(uint16_t)(rd_u16(OFFSET)+2));
    }
}
static void alternate_records(const FollowupPlacementHooks *h) {
    wr_u16(OFFSET,rd_u16(ALTERNATE_OFFSET));
    for (;;) {
        gaddr record=ALTERNATE_LIST+(gaddr)(int32_t)rd_s16(OFFSET);
        uint16_t header=rd_u16(record);
        FollowupPlacementEvent e={.phase=FOLLOWUP_ALTERNATE_SCAN,.record=record,
                                 .header=header,.index=rd_u16(OFFSET)};
        unsigned i;
        emit(h,e); if (header==0xffffu) return;
        wr_u8(HEADER,(uint8_t)header); wr_u16(BOUND_SHIFT,header&15u);
        e.descriptor=rd_u32(record+2); e.phase=FOLLOWUP_ALTERNATE_DESCRIPTOR; emit(h,e);
        if (rd_s16(e.descriptor)<0 ||
            (rd_u32(e.descriptor)==0xC1ED48u && rd_s32(POSITION_BIAS)<-0x400000)) goto advance;
        for(i=0;i<3;++i) e.point[i]=rd_s16(record+6+2*i);
        wr_u32(EXTRA,rd_u32(record+12)); wr_u16(KIND,header>>8); wr_u8(POSITION_VALID,0);
        e.phase=FOLLOWUP_ALTERNATE_POINT; emit(h,e);
        e.transformed=(header&0x50u)!=0;
        if (e.transformed) {
            gaddr owner=accumulate_selected_position((header&0x40u)!=0,header,e.point);
            e.value=(int32_t)owner;
        } else {
            for(i=0;i<3;++i) wr_s16(BOUND_OFFSET_X+2*i,(int16_t)e.point[i]);
            for(i=0;i<3;++i) e.point[i]=(int32_t)((uint32_t)e.point[i]<<8);
        }
        for(i=0;i<3;++i) wr_s32(POINT+4*i,e.point[i]);
        if (e.transformed) for(i=0;i<3;++i) {
            e.point[i] >>= 8; wr_s16(BOUND_OFFSET_X+2*i,(int16_t)e.point[i]);
        }
        e.phase=FOLLOWUP_ALTERNATE_PACKED; emit(h,e);
        e.value=(int32_t)((uint32_t)(int32_t)rd_s16(record+16)<<(header&15u));
        e.phase=FOLLOWUP_CACHE; emit(h,e);
        if (e.value<0x100) distance(h,record+16);
        else if (e.value<0x400) {
            e.value=rd_u16(CLOCK)&3u; e.phase=FOLLOWUP_REFRESH_PHASE; emit(h,e);
            if ((header&0x100u) ? e.value==2 : e.value==0) distance(h,record+16);
        }
        if (!e.transformed) {
            wr_u16(GATE,rd_u16(record+20));
            if (rd_s16(GATE)<0) {
                int countdown=rd_s8(record+18)-1;
                wr_u8(record+18,(uint8_t)countdown);
                if (countdown>=0) goto advance;
                wr_u8(record+18,rd_u8(0xC458BC));
            }
        }
        wr_u8(record+19,(uint8_t)(rd_u8(record+19)-1)); wr_u8(VISIT,rd_u8(record+19));
        e.value=rd_s16(record+16); wr_s16(MAGNITUDE,(int16_t)e.value);
        wr_u16(SCALED_KIND,(uint16_t)((header&0xff00u)*2));
        if (rd_u16(SCALED_KIND)==rd_u16(SELECTED_KIND)) {
            if (!rd_u8(MODE)) e.value=0x7fff;
            wr_s16(0xC45B42,(int16_t)e.value);
        }
        e.value=dispatch(h,e);
        if ((int16_t)e.value<=0) e.value=-1;
        e.index=rd_u16(OFFSET); wr_s16(ALTERNATE_LIST+(gaddr)(int32_t)(int16_t)e.index+20,(int16_t)e.value);
        e.phase=FOLLOWUP_RESULT; emit(h,e);
advance:
        wr_u16(OFFSET,(uint16_t)(rd_u16(OFFSET)+24));
    }
}
static void relative_points(const FollowupPlacementHooks *h) {
    FollowupPlacementEvent e={0};
    uint16_t offset;
    wr_u8(0xC45835,0); wr_u8(0xC45838,0);
    if (rd_u8(MODE)) return;
    if (rd_s8(0xC45837)<=1) {
        e.value=rd_u16(CLOCK); e.phase=FOLLOWUP_CLOCK; emit(h,e);
        if (!(e.value&2u)) wr_u8(0xC45838,1);
        if (e.value&3u) return;
    } else wr_u8(0xC45838,1);
    wr_u16(OFFSET,0); e.phase=FOLLOWUP_RELATIVE_BEGIN; emit(h,e);
    for(offset=0;offset<0x2000u;offset+=0x200u) {
        int32_t maximum; unsigned shift=0,i;
        gaddr other=CONTROL_RECORDS+offset;
        gaddr selected=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(SELECTED_KIND);
        e.phase=FOLLOWUP_RELATIVE_SCAN; e.index=offset; e.header=rd_u16(SELECTED_KIND); emit(h,e);
        if (offset==rd_u16(SELECTED_KIND) || !(rd_u8(other+1)&0x40u)) goto next;
        for(i=0;i<3;++i) { e.point[i]=sub32(rd_s32(other+0x14+4*i),rd_s32(selected+0x14+4*i)); e.terms[i]=absolute(e.point[i]); }
        maximum=e.terms[0]; if(e.terms[1]>=maximum) maximum=e.terms[1];
        if(e.terms[2]>maximum) maximum=e.terms[2];
        while(maximum>0x7fffff) { shift+=2; maximum >>= 2; }
        for(i=0;i<3;++i) e.point[i]=shifted(e.point[i],shift+8);
        e.value=maximum; e.shift=(uint16_t)shift; e.phase=FOLLOWUP_RELATIVE_POINT; emit(h,e);
        append_list_point((int16_t)e.point[0],(int16_t)e.point[1],(int16_t)e.point[2],shift,(offset>>1)|0x10u);
        e.phase=FOLLOWUP_RELATIVE_STORED; emit(h,e);
next:
        e.phase=FOLLOWUP_RELATIVE_ADVANCE; e.index=offset+0x200u; emit(h,e);
    }
}
void visit_followup_placements(const FollowupPlacementHooks *hooks) {
    wr_u8(0xC45864,1); wr_u8(HEADER,4); wr_u8(VISIT,0); wr_u16(GATE,0);
    wr_u32(EXTRA,0); wr_u16(OFFSET,0);
    selected_records(hooks); alternate_records(hooks); relative_points(hooks);
}
