#include "display_buffer_gate.h"
#include <assert.h>
static int a(void*x){++*(int*)x;return 0;}static int b(void*x){*(int*)x+=10;return 0;}int main(void){int n=0;FA18DisplayBufferGateOps o={a,b,&n};assert(!fa18_prepare_display_buffer(0,&o)&&n==1);assert(!fa18_prepare_display_buffer(0x2000,&o)&&n==11);return 0;}
