#include "native_selector_origin.h"

#include <assert.h>
#include <string.h>

typedef struct {
    FA18CommandInput input; FA18NativeSceneRecords records;
    FA18NativeSelectorOrigin selector; FA18NativeSelectorOriginOps ops;
    FA18NativeSelectorOriginTables tables; FA18NativeSceneRecord *active;
    uint8_t source[16*512],work[16*32],table_bytes[64];
    int32_t root_preset[3],origin[3],candidate[3],smoothed[3],negated[3],aux_delta;
    uint16_t angle_history,status;
    uint8_t enable,gate_b,gate_a,gate_mode,detail,index,mode,threshold,auxiliary,variant,counter;
    unsigned calls[4];
    FA18NativeVectorMath math; PortFieldWindow magnitude_table;
    uint8_t magnitude_bytes[2]; uint16_t magnitude; int16_t normalized[3];
} Fixture;

static int consume(void *context,FA18NativeSelectorOrigin *state,
                   FA18NativeSelectorOriginChild child,const int32_t input[3],int32_t output[3]) {
    Fixture *f=context; assert(state==&f->selector && child<4); ++f->calls[child];
    if(child==FA18_SELECTOR_ORIGIN_PREPARE) { assert(!input && !output); return 1; }
    assert(output);
    if(child==FA18_SELECTOR_ORIGIN_REGENERATE) {
        assert(!input); output[0]=7; output[1]=8; output[2]=9; return 1;
    }
    assert(input);
    assert(input[0]==1 && input[1]==2 && input[2]==3);
    output[0]=0x1000; output[1]=-100; output[2]=0x3000; return 1;
}
static void put16(FA18NativeSceneRecord *record,size_t offset,uint16_t value) {
    uint8_t bytes[2]={(uint8_t)(value>>8),(uint8_t)value};
    assert(fa18_write_native_scene_record(record,offset,bytes,2));
}
static void initialize(Fixture *f) {
    PortFieldWindow window;
    memset(f,0,sizeof *f);
    assert(fa18_import_native_scene_records(&f->records,&f->input,
        f->source,sizeof f->source,f->work,sizeof f->work));
    f->magnitude_bytes[0]=0x40;
    f->magnitude_table=(PortFieldWindow){.bytes=f->magnitude_bytes,.byte_count=2};
    f->math=(FA18NativeVectorMath){&f->magnitude_table,&f->magnitude,f->normalized};
    f->active=f->records.records; f->ops=(FA18NativeSelectorOriginOps){consume,f};
    window=(PortFieldWindow){.bytes=f->table_bytes,.byte_count=sizeof f->table_bytes,.origin=0};
    f->tables=(FA18NativeSelectorOriginTables){window,window,window,window};
    f->selector=(FA18NativeSelectorOrigin){.records=&f->records,.active_record=&f->active,
        .ops=&f->ops,.vector_math=&f->math,.tables=&f->tables,.root_preset=f->root_preset,.origin=f->origin,
        .candidate=f->candidate,.smoothed_delta=f->smoothed,.negated_companion=f->negated,
        .auxiliary_delta=&f->aux_delta,.angle_history=&f->angle_history,.status_word=&f->status,
        .enable=&f->enable,.gate_b=&f->gate_b,.gate_a=&f->gate_a,.gate_mode=&f->gate_mode,
        .detail_mode=&f->detail,.detail_index=&f->index,.adjustment_mode=&f->mode,
        .threshold_flag=&f->threshold,.auxiliary_flag=&f->auxiliary,
        .variant_selector=&f->variant,.detail_counter=&f->counter};
}
int main(void) {
    static Fixture f;
    initialize(&f); f.enable=f.gate_b=f.gate_mode=1; f.origin[1]=0x12345678;
    f.active->geometry->position[0]=0x10800000; f.active->geometry->position[2]=0x11000000;
    assert(fa18_update_native_selector_origin(&f.selector));
    assert(f.calls[FA18_SELECTOR_ORIGIN_PREPARE]==1 && f.origin[0]==0x10800000 &&
           f.origin[1]==0x12345678 && f.origin[2]==0x11000000);
    assert(!f.negated[0] && (uint32_t)f.negated[1]==0xedcba988u && !f.negated[2]);

    initialize(&f); f.enable=1;
    assert(fa18_update_native_selector_origin(&f.selector));
    assert(f.calls[FA18_SELECTOR_ORIGIN_PREPARE]==1 && !f.origin[0]);

    initialize(&f); f.enable=0xff; f.gate_b=1; f.active->aircraft->equipment_kind=0x11;
    f.table_bytes[0]=0; f.table_bytes[1]=1; f.table_bytes[2]=0; f.table_bytes[3]=2;
    f.table_bytes[4]=0; f.table_bytes[5]=3; f.active->byte_04=0xc0; put16(f.active,0x4e,2);
    assert(fa18_update_native_selector_origin(&f.selector));
    assert(f.calls[FA18_SELECTOR_ORIGIN_MATRIX_B]==1 && f.candidate[1]==-100);
    assert(f.origin[0]==0x1000 && f.origin[1]==0x900 && f.origin[2]==0x3000);

    initialize(&f); f.enable=f.gate_b=1; f.detail=6; f.mode=5; f.candidate[0]=0x1000;
    assert(fa18_update_native_selector_origin(&f.selector));
    assert(f.magnitude==0x1000 && f.normalized[0]==512 && f.origin[0]==0x400 &&
           f.smoothed[0]==0x400 && f.negated[0]==-0x400);

    initialize(&f); f.enable=f.gate_b=1; f.detail=3; f.mode=7; f.auxiliary=1;
    assert(fa18_update_native_selector_origin(&f.selector));
    assert(f.calls[FA18_SELECTOR_ORIGIN_REGENERATE]==1 && f.detail==3 && f.mode==8 &&
           f.candidate[0]==7 && f.candidate[2]==9);
    return 0;
}
