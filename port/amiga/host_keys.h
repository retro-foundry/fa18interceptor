#ifndef AMIGA_HOST_KEYS_H
#define AMIGA_HOST_KEYS_H
/* Physical Amiga raw key identities; -1 denotes an unmapped host key.
 * Character/SDL2 symbols and SDL1/libretro replay symbols are separate APIs. */
int amiga_host_raw_key(int key);
int amiga_host_legacy_raw_key(int key);
#endif
