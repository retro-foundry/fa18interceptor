/* SDL keyboard boundary shared by the playable entry and integration check. */
#include "host_input.h"
void native_host_keyboard_event(NativeFrontend *game,const SDL_KeyboardEvent *event) {
    if((event->type!=SDL_KEYDOWN && event->type!=SDL_KEYUP) || event->repeat) return;
    int key=event->keysym.sym;
    if(key>='a' && key<='z') key-=32;
    if(key==SDLK_RETURN) key='\r';
    if(key==SDLK_BACKSPACE) key='\b';
    if(key>=SDLK_F1 && key<=SDLK_F10) key=282+key-SDLK_F1;
    if(key==SDLK_UP) key=273;
    if(key==SDLK_DOWN) key=274;
    if(key==SDLK_RIGHT) key=275;
    if(key==SDLK_LEFT) key=276;
    if(key==SDLK_LSHIFT) key=304;
    if(key==SDLK_RSHIFT) key=303;
    native_frontend_event(game,key,event->type==SDL_KEYDOWN);
}
