#include "terrain_template_cursor_resolver.h"
#include <assert.h>
int main(void){uint8_t a[0xf64]={0},g[0x201]={0},x[0x160e]={0},gate[4]={1};FA18HunkSegment v[67]={{0}};FA18Hunks h={v,67};FA18TerrainTemplateCursorState s={0,0,0,0,0,0,-100000,7,9,0,0,gate,gate,gate,sizeof gate,sizeof gate,sizeof gate};FA18TerrainTemplateCursor o;
a[0x380]=0;a[0x381]=2;v[65]=(FA18HunkSegment){FA18_HUNK_CODE,a,sizeof a,0,0};v[66]=(FA18HunkSegment){FA18_HUNK_CODE,g,sizeof g,0,0};v[8]=(FA18HunkSegment){FA18_HUNK_CODE,x,sizeof x,0,0};assert(!fa18_resolve_terrain_template_cursor(&h,&s,&o));assert(o.data.band_control==a+0x382&&o.data.group_directory_offset==0&&o.gate==gate&&o.row_term==7&&o.group_term==9&&!o.derived_root);return 0;}
