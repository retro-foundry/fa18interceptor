#ifndef FA18_NATIVE_FILES_H
#define FA18_NATIVE_FILES_H
#include "frontend.h"
/* Host-owned OS workspace, outside display/recorder/audio allocations. */
enum { NATIVE_FILE_INFO = 0x4a000, NATIVE_FILE_INFO_BYTES = 64 };
int native_files_open(NativeFrontend *game,const char *save_directory);
void native_frontend_refresh_log(NativeFrontend *game);
#endif
