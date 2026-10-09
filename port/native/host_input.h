#ifndef FA18_NATIVE_HOST_INPUT_H
#define FA18_NATIVE_HOST_INPUT_H
#include "../game/native/frontend.h"
#include <SDL.h>
void native_host_keyboard_event(NativeFrontend *game,const SDL_KeyboardEvent *event);
/* Return zero on window close. Recorded-input diagnostics still poll SDL,
 * but physical controls must not change the recorded scenario. */
int native_host_event(NativeFrontend *game,const SDL_Event *event,int recorded_input_only);
#endif
