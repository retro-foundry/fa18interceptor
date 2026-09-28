#include "template_workspace_append.h"
#include <assert.h>
#include <string.h>
int main(void){uint8_t a[0x200*16]={0},b[0x20*16]={0},o[16]={0};size_t n;a[1]=0x50;a[6]=0;a[7]=9;a[8]=0;a[9]=7;a[10]=3;b[0x20+1]=0x50;b[0x20+6]=0;b[0x20+7]=9;b[0x20+8]=0;b[0x20+9]=7;b[0x20+10]=3;FA18TemplateWorkspaceAppend s={a,sizeof a,b,sizeof b,o,sizeof o};assert(!fa18_append_template_workspace_matches(&s,3,7,9,&n)&&n==4&&!memcmp(o,(uint8_t[]){0x10,0,0x40,1,0xff},5));assert(!(a[1]&0x10)&&!(b[0x21]&0x10));assert(!fa18_append_template_workspace_matches(&s,-1,7,9,&n)&&n==0);}
