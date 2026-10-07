/* Shared host keyboard identities for the Amiga physical key layout.
 * Extracted unchanged from the reference runner; no game or SDL dependency. */
#include "host_keys.h"
#include <stdint.h>
#include <string.h>

int amiga_host_raw_key(int k) {
    static const char row0[] = "`1234567890-=\\";
    static const char row1[] = "qwertyuiop[]";
    static const char row2[] = "asdfghjkl;'";
    static const char row3[] = "zxcvbnm,./";
    const char *p;
    if (k >= 'A' && k <= 'Z') k += 'a' - 'A';
    if (k > 0 && k < 128) {
        if ((p = strchr(row0, k)) != NULL) return (int)(p - row0);
        if ((p = strchr(row1, k)) != NULL) return 0x10 + (int)(p - row1);
        if ((p = strchr(row2, k)) != NULL) return 0x20 + (int)(p - row2);
        if ((p = strchr(row3, k)) != NULL) return 0x31 + (int)(p - row3);
    }
    switch (k) {
    case ' ': return 0x40;
    case '\b': return 0x41;
    case '\t': return 0x42;
    case '\r': return 0x44;
    case 27: return 0x45;
    case 127: return 0x46;
    /* SDLK_* values for non-character keys are scancode | 1<<30. */
    case 0x40000052: return 0x4C; /* up */
    case 0x40000051: return 0x4D; /* down */
    case 0x4000004F: return 0x4E; /* right */
    case 0x40000050: return 0x4F; /* left */
    case 0x400000E1: return 0x60; /* left shift */
    case 0x400000E5: return 0x61; /* right shift */
    case 0x40000039: return 0x62; /* caps lock */
    case 0x400000E0: case 0x400000E4: return 0x63; /* ctrl */
    case 0x400000E2: return 0x64; /* left alt */
    case 0x400000E6: return 0x65; /* right alt */
    case 0x400000E3: return 0x66; /* left GUI = left Amiga */
    case 0x400000E7: return 0x67; /* right GUI = right Amiga */
    case 0x40000049: return 0x5F; /* insert = Help */
    case 0x40000062: return 0x0F; /* keypad 0 */
    case 0x40000059: return 0x1D;
    case 0x4000005A: return 0x1E;
    case 0x4000005B: return 0x1F;
    case 0x4000005C: return 0x2D;
    case 0x4000005D: return 0x2E;
    case 0x4000005E: return 0x2F;
    case 0x4000005F: return 0x3D;
    case 0x40000060: return 0x3E;
    case 0x40000061: return 0x3F; /* keypad 9 */
    case 0x40000063: return 0x3C; /* keypad . */
    case 0x40000056: return 0x4A; /* keypad - */
    case 0x40000058: return 0x43; /* keypad enter */
    case 0x40000054: return 0x5C; /* keypad / */
    case 0x40000055: return 0x5D; /* keypad * */
    case 0x40000057: return 0x5E; /* keypad + */
    default: break;
    }
    if (k >= 0x4000003A && k <= 0x40000043) return 0x50 + (k - 0x4000003A); /* F1-F10 */
    return -1;
}

int amiga_host_legacy_raw_key(int k) {
    static const uint8_t keypad[10] = {0x0F, 0x1D, 0x1E, 0x1F, 0x2D, 0x2E, 0x2F, 0x3D, 0x3E, 0x3F};
    if (k < 256) return k == 0 ? -1 : amiga_host_raw_key(k); /* ASCII range is shared */
    if (k <= 265) return keypad[k - 256];
    if (k >= 282 && k <= 291) return 0x50 + (k - 282); /* F1-F10 */
    switch (k) {
    case 266: return 0x3C; /* keypad . */
    case 267: return 0x5C; /* keypad / */
    case 268: return 0x5D; /* keypad * */
    case 269: return 0x4A; /* keypad - */
    case 270: return 0x5E; /* keypad + */
    case 271: return 0x43; /* keypad enter */
    case 273: return 0x4C; /* up */
    case 274: return 0x4D; /* down */
    case 275: return 0x4E; /* right */
    case 276: return 0x4F; /* left */
    case 277: case 315: return 0x5F; /* insert, help = Help */
    case 301: return 0x62; /* caps lock */
    case 303: return 0x61; /* right shift */
    case 304: return 0x60; /* left shift */
    case 305: case 306: return 0x63; /* ctrl */
    case 307: return 0x65; /* right alt */
    case 308: return 0x64; /* left alt */
    case 309: case 312: return 0x67; /* right Amiga */
    case 310: case 311: return 0x66; /* left Amiga */
    default: return -1;
    }
}

