#include "runtime_memory.h"
#undef malloc
#undef calloc
#undef realloc
#undef free
#undef fopen
#undef fclose
static int gameplay;
static size_t violations;
void amiga_runtime_memory_lock(int locked) {gameplay=locked;}
size_t amiga_runtime_memory_violations(void) {return violations;}
void *amiga_runtime_malloc(size_t size) {
    if(gameplay) {++violations;return NULL;}
    return malloc(size);
}
void *amiga_runtime_calloc(size_t count,size_t size) {
    if(gameplay) {++violations;return NULL;}
    return calloc(count,size);
}
void *amiga_runtime_realloc(void *pointer,size_t size) {
    if(gameplay) {++violations;return NULL;}
    return realloc(pointer,size);
}
void amiga_runtime_free(void *pointer) {
    if(!pointer) return;
    if(gameplay) {++violations;return;}
    free(pointer);
}
FILE *amiga_runtime_fopen(const char *path,const char *mode) {
    if(gameplay) {++violations;return NULL;}
    return fopen(path,mode);
}
int amiga_runtime_fclose(FILE *file) {
    if(gameplay) {++violations;return EOF;}
    return fclose(file);
}
