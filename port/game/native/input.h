#ifndef FA18_NATIVE_INPUT_H
#define FA18_NATIVE_INPUT_H
#include "frontend.h"
void native_input_enqueue(NativeFrontend *game,int key,int down);
void native_input_enqueue_raw(NativeFrontend *game,uint8_t raw);
/* C0F3C4 executes once before C0F5F8 and the flight record pass. */
void native_input_process(NativeFrontend *game);
#endif
