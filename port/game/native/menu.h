#ifndef FA18_NATIVE_MENU_H
#define FA18_NATIVE_MENU_H
#include "frontend.h"
void native_menu_initialize(void);
void native_menu_key(NativeFrontend *game,int key,int down);
unsigned native_menu_raw_key(int key,int down);
void native_menu_dispatch_raw(NativeFrontend *game,uint8_t raw);
void native_menu_dispatch_pending(NativeFrontend *game);
unsigned native_menu_selected_mode(const NativeFrontend *game);
void native_menu_tick(NativeFrontend *game);
#endif
