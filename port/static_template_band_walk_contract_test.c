#include "static_template_band_walk.h"
#include <assert.h>
static int s(void*c,int16_t a,int16_t b,uint8_t*w,size_t z){int16_t*x=c;x[0]=a;x[1]=b;w[0]=(uint8_t)z;return 0;}int main(void){uint8_t c[]={1,0,255},w[0x600]={0};int8_t t[]={0,0},d[]={2,-3};int16_t got[2]={0};FA18TemplateBandWalkInput i={c,3,t,2,d,1,4,5,0,w,sizeof w,s,got};FA18TemplateBandWalkResult r;assert(!fa18_walk_static_template_bands(&i,&r)&&r.accepted==1&&r.status==0x52&&got[0]==6&&got[1]==2);}
