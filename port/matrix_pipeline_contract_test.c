#include "matrix_pipeline.h"
#include "hunk.h"
#include "run075_trig_asset.h"
#include "two_angle_matrix.h"
#include <assert.h>
#include <string.h>
typedef struct { int calls; int32_t selector; } Calls;
static int transform(void *p, const uint8_t *r, int32_t s, int32_t o[3]) { Calls *c=p; (void)r; ++c->calls; c->selector=s; o[0]=1;o[1]=2;o[2]=3; return 0; }
static int update(void *p, const int32_t r[3], int32_t a, int16_t x, int16_t z, int16_t o[2]) { (void)p; assert(r[0]==1&&r[1]==2&&r[2]==3&&a==-1&&x==7&&z==8);o[0]=720;o[1]=0;return 0; }
int main(void) {
 uint8_t bytes[0xae8+sizeof fa18_run075_trig_bytes]={0}, store[0xa4]={0}; FA18HunkSegment seg[64]={{0}}; FA18Hunks hunks={seg,64}; FA18FlightTrigTable trig; Calls calls={0}; FA18MatrixPipelineOps ops={transform,update,&calls}; FA18MatrixPipelineTailState out, expected;
 memcpy(bytes+0xae8,fa18_run075_trig_bytes,sizeof fa18_run075_trig_bytes);seg[63]=(FA18HunkSegment){.data=bytes,.size=sizeof bytes};assert(fa18_load_two_angle_trig_table(&hunks,&trig)==0);
 FA18MatrixPipelineState state={.enable_state=1,.skip_coordinate_update=1,.matrix_input={720,0},.row_scale={168,252,128}};
 assert(fa18_run_matrix_pipeline_tail(&trig,&(FA18MatrixPipelineTailInput){720,0,{168,252,128},{0,0,0}},&expected)==0);
 assert(fa18_run_matrix_pipeline(&trig,store,sizeof store,&state,&ops,&out)==0&&calls.calls==1&&calls.selector==1);assert(!memcmp(&out,&expected,sizeof out));
 state.skip_coordinate_update=0;state.matrix_input[0]=7;state.matrix_input[1]=8;state.selection_cache=1;assert(fa18_run_matrix_pipeline(&trig,store,sizeof store,&state,&ops,&out)==0&&state.matrix_input[0]==720&&state.matrix_input[1]==0);assert(!memcmp(&out,&expected,sizeof out));
 state.enable_state=0;assert(fa18_run_matrix_pipeline(&trig,store,sizeof store,&state,&ops,&out)==-2);return 0;
}
