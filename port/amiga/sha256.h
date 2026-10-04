#ifndef AMIGA_SHA256_H
#define AMIGA_SHA256_H
#include <stddef.h>
/* Hash arbitrary host bytes; independent of CPU, disk format and game. */
void amiga_sha256_hex(const void *data,size_t size,char hex[65]);
#endif
