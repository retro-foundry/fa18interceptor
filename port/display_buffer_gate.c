#include "display_buffer_gate.h"
int fa18_prepare_display_buffer(uint16_t flags,const FA18DisplayBufferGateOps*o){FA18DisplayBufferStage s;if(!o)return -1;s=(flags&0x2000u)?o->alternate:o->active_planes;return s&&s(o->context)==0?0:-1;}
