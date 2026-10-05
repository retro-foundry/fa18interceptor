#include "native_record_orientation.h"
#include "run075_trig_asset.h"
#include <assert.h>
#include <string.h>

int main(void) {
    static FA18NativeSceneRecords bank;
    FA18CommandInput commands={0};
    uint8_t original[16*512],work[16*32],bytes[512];
    FA18FlightTrigData data={.bytes=fa18_run075_trig_bytes,.byte_count=sizeof fa18_run075_trig_bytes};
    uint16_t angles[3]={0,0,0};
    int16_t sine,cosine;
    unsigned row,column;
    memset(original,0xa5,sizeof original); memset(work,0x5a,sizeof work);
    assert(fa18_import_native_scene_records(&bank,&commands,original,sizeof original,work,sizeof work));
    assert(fa18_publish_native_record_orientation(bank.records+7,angles,&data));
    assert(bank.aircraft[7].secondary_flags==0xa5a5);
    for(row=0;row<3;++row) for(column=0;column<3;++column) {
        int16_t value=row==column?0x4000:0;
        assert(bank.records[7].forward[row][column]==value && bank.geometry[7].inverse[row][column]==value);
        assert(bank.records[7].unported[0x80+row*6+column*2]==0);
    }
    assert(fa18_read_native_scene_record(bank.records+7,0,bytes,512));
    assert(!memcmp(bytes+0xa4,original+7*512+0xa4,512-0xa4));
    assert(fa18_reset_native_record_orientation(bank.records+7,angles,&data));
    assert(bank.aircraft[7].secondary_flags==0xa5a1);
    bank.records[7].angle_first=123; bank.geometry[7].angle=456; bank.records[7].angle_third=789;
    bank.records[7].forward[0][0]=111; bank.geometry[7].inverse[0][0]=222;
    assert(fa18_publish_native_record_inverse_with_axis(bank.records+7,angles,&data,NULL));
    assert(bank.records[7].angle_first==123 && bank.geometry[7].angle==456 && bank.records[7].angle_third==789);
    assert(bank.records[7].forward[0][0]==111 && bank.geometry[7].inverse[0][0]==0x4000);
    assert(bank.aircraft[7].secondary_flags==0xa5a1);
    assert(fa18_publish_native_record_orientation(bank.records+7,angles,&data));
    /* Missing forward/inverse data must retain stores completed earlier. */
    angles[0]=0xffff;
    assert(!fa18_publish_native_record_orientation(bank.records+7,angles,&data));
    assert(bank.records[7].angle_first==0xffff && bank.records[7].forward[0][0]==0x4000);
    angles[0]=0x7081; bank.records[7].forward[0][0]=123; bank.geometry[7].inverse[0][0]=456;
    assert(!fa18_publish_native_record_orientation(bank.records+7,angles,&data));
    assert(bank.records[7].forward[0][0]==0x4000 && bank.geometry[7].inverse[0][0]==456);
    assert(!fa18_publish_native_record_orientation(NULL,angles,&data));
    assert(fa18_flight_lookup_trig_data(&data,0x4000,&sine,&cosine)==-1);
    {
        int16_t preceding=(int16_t)-32768,following=0x1234;
        PortFieldByte before[]={{.word=&preceding,.shift=8},{.word=&preceding}};
        PortFieldByte after[]={{.word=&following,.shift=8},{.word=&following}};
        FA18FlightTrigData bound=data;
        bound.byte_count=0x70a; bound.before=before; bound.before_count=2;
        bound.after=after; bound.after_count=2;
        /* ADD.W doubles $FFFF to signed -2; cosine indexes +$70A. */
        assert(fa18_flight_lookup_trig_data(&bound,0xffff,&sine,&cosine)==0);
        assert(sine==(int16_t)-32768 && cosine==0x1234);
        preceding=0x5678; following=(int16_t)-32768;
        assert(fa18_flight_lookup_trig_data(&bound,0xffff,&sine,&cosine)==0);
        assert(sine==0x5678 && cosine==(int16_t)-32768);
        bound.after=NULL;
        assert(fa18_flight_lookup_trig_data(&bound,0xffff,&sine,&cosine)==-1);
    }
    return 0;
}
