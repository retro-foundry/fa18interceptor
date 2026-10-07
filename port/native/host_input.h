#ifndef FA18_NATIVE_HOST_INPUT_H
#define FA18_NATIVE_HOST_INPUT_H
#include "../game/native/frontend.h"
#include <SDL.h>
void native_host_keyboard_event(NativeFrontend *game,const SDL_KeyboardEvent *event);
#endif
