#include "terrain_template_selector_pass.h"
#include <assert.h>
int main(void){uint8_t a[0xf64]={0},g[32]={0},x[0x160e]={0},gate[4]={0,0,0,1},ws[0x600]={0};FA18HunkSegment v[67]={{0}};FA18Hunks h={v,67};FA18TerrainTemplateSelectorPassInput i={&h,{0,0,0,0,0,0,-100000,0,0,0,0,gate,gate,gate,4,4,4},ws,sizeof ws,0};FA18TerrainTemplateSelectorPassResult r;
a[0x381]=2;a[0x382]=0;a[0x383]=0;a[0x384]=0xff;g[1]=8;g[9]=2;g[15]=20;g[22]=1;g[23]=2;g[24]=3;g[25]=4;g[26]=0xff;v[65]=(FA18HunkSegment){FA18_HUNK_CODE,a,sizeof a,0,0};v[66]=(FA18HunkSegment){FA18_HUNK_CODE,g,sizeof g,0,0};v[8]=(FA18HunkSegment){FA18_HUNK_CODE,x,sizeof x,0,0};assert(!fa18_run_terrain_template_selector_pass(&i,&r));assert(r.workspace.expanded_band_count==1&&ws[2]==1&&ws[6]==0xff);return 0;}
