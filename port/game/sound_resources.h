#ifndef FA18_SOUND_RESOURCES_H
#define FA18_SOUND_RESOURCES_H
#include "memory.h"
typedef struct {
    int (*load)(void *context,const char *path,unsigned slot);
    int (*duplicate)(void *context,unsigned source,unsigned destination);
    void (*release)(void *context,unsigned slot);
    void *context;
} SoundResourceHooks;
/* Original C17510 and C1756A: disk-backed menu pairs and linked phrases. */
void load_intro_sound_resources(const SoundResourceHooks *hooks);
void load_menu_sound_resources(const SoundResourceHooks *hooks);
#endif
