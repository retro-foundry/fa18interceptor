#include "native_record_orientation.h"
#include "two_angle_matrix.h"

static int orientation_terms(const FA18FlightTrigData *data,const uint16_t angles[3],
                               FA18TrigTerms *terms) {
    unsigned i;
    for(i=0;i<3;++i)
        if(fa18_flight_lookup_trig_data(data,(uint16_t)(angles[i]>>3),
            terms->sine+i,terms->cosine+i)!=0) return 0;
    return 1;
}
int fa18_publish_native_record_orientation_with_axis(FA18NativeSceneRecord *record,
    const uint16_t angles[3],const FA18FlightTrigData *data,uint32_t *axis) {
    FA18TrigTerms terms;
    uint16_t original[3],inverse[3];
    unsigned i;
    if(!record || !record->aircraft || !record->geometry || !angles || !data) return 0;
    /* Keep caller aliasing safe: publication can overwrite supplied angles. */
    for(i=0;i<3;++i) original[i]=angles[i];
    record->angle_first=original[0]; record->geometry->angle=original[1]; record->angle_third=original[2];
    if(!orientation_terms(data,original,&terms) || fa18_compose_rotation_terms(&terms,record->forward)!=0) return 0;
    for(i=0;i<3;++i) inverse[i]=original[i]?(uint16_t)(0x7080u-original[i]):0;
    if(!orientation_terms(data,inverse,&terms)) return 0;
    if(axis) *axis=(uint16_t)terms.sine[2];
    return fa18_compose_attitude_terms(&terms,record->geometry->inverse)==0;
}
int fa18_publish_native_record_orientation(FA18NativeSceneRecord *record,
    const uint16_t angles[3],const FA18FlightTrigData *data) {
    return fa18_publish_native_record_orientation_with_axis(record,angles,data,NULL);
}
int fa18_reset_native_record_orientation(FA18NativeSceneRecord *record,
    const uint16_t angles[3],const FA18FlightTrigData *data) {
    if(!record || !record->aircraft) return 0;
    record->aircraft->secondary_flags&=UINT16_C(0xfffb);
    return fa18_publish_native_record_orientation(record,angles,data);
}
