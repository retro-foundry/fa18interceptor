#ifndef AMIGA_RUNTIME_MEMORY_H
#define AMIGA_RUNTIME_MEMORY_H
#include <stdlib.h>
#include <stdio.h>
/* Forced include on the native runner/runtime only. Startup loaders may use
 * the heap; project malloc/calloc/realloc/free are rejected in gameplay.
 * SDL uses its own fixed arena. CRT/OS internals are not intercepted. */
void *amiga_runtime_malloc(size_t size);
void *amiga_runtime_calloc(size_t count,size_t size);
void *amiga_runtime_realloc(void *pointer,size_t size);
void amiga_runtime_free(void *pointer);
FILE *amiga_runtime_fopen(const char *path,const char *mode);
int amiga_runtime_fclose(FILE *file);
void amiga_runtime_memory_lock(int locked);
size_t amiga_runtime_memory_violations(void);
#define malloc(size) amiga_runtime_malloc(size)
#define calloc(count,size) amiga_runtime_calloc(count,size)
#define realloc(pointer,size) amiga_runtime_realloc(pointer,size)
#define free(pointer) amiga_runtime_free(pointer)
#define fopen(path,mode) amiga_runtime_fopen(path,mode)
#define fclose(file) amiga_runtime_fclose(file)
#endif
