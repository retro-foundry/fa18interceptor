#include "current_record_matrix.h"

static int16_t read_be16(const uint8_t *bytes) {
    return (int16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

int fa18_build_current_record_matrix_value(uint16_t record_angle,
    const FA18FlightTrigData *trig,int16_t matrix[3][3]) {
    int16_t angle=record_angle?(int16_t)(0x7080u-record_angle):0,sine,cosine,lookup;
    if(!trig || !matrix) return -1;
    lookup=angle>=0?(int16_t)(angle>>3):(int16_t)-((-(int32_t)angle+7)>>3);
    if(fa18_flight_lookup_trig_data(trig,(uint16_t)lookup,&sine,&cosine)!=0) return -1;
    matrix[0][0]=cosine; matrix[0][1]=0; matrix[0][2]=sine;
    matrix[1][0]=0; matrix[1][1]=0x4000; matrix[1][2]=0;
    matrix[2][0]=(int16_t)(uint16_t)(0u-(uint16_t)sine); matrix[2][1]=0; matrix[2][2]=cosine;
    return 0;
}
int fa18_build_current_record_matrix(FA18CurrentRecordMatrixState *state) {
    FA18FlightTrigData trig;
    if (!state || !state->active_record ||
        state->active_record_size < FA18_CURRENT_RECORD_MATRIX_RECORD_BYTES ||
        !state->trig_table)
        return -1;
    trig=(FA18FlightTrigData){.bytes=state->trig_table->bytes,.byte_count=state->trig_table->byte_count};
    return fa18_build_current_record_matrix_value((uint16_t)read_be16(state->active_record+0x68),
        &trig,state->matrix);
}

void fa18_build_current_record_matrix_callback(void *context) {
    (void)fa18_build_current_record_matrix(context);
}
