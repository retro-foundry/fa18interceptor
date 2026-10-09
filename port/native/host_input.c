/* SDL keyboard boundary shared by the playable entry and integration check. */
#include "host_input.h"
int native_host_event(NativeFrontend *game,const SDL_Event *event,int recorded_input_only) {
    if(event->type==SDL_QUIT) return 0;
    if(recorded_input_only) return 1;
    if(event->type==SDL_MOUSEMOTION) native_frontend_mouse(game,event->motion.xrel,event->motion.yrel);
    if((event->type==SDL_MOUSEBUTTONDOWN || event->type==SDL_MOUSEBUTTONUP) &&
       (event->button.button==SDL_BUTTON_LEFT || event->button.button==SDL_BUTTON_RIGHT))
        native_frontend_button(game,event->button.button==SDL_BUTTON_LEFT?0:1,event->type==SDL_MOUSEBUTTONDOWN);
    if(event->type==SDL_KEYDOWN || event->type==SDL_KEYUP)
        native_host_keyboard_event(game,&event->key);
    return 1;
}
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
