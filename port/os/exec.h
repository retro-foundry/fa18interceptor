#ifndef FA18_OS_EXEC_H
#define FA18_OS_EXEC_H

#include <stdint.h>

/* Kickstart 1.3 Exec Disable/Enable nesting byte at ExecBase+$126. */
uint8_t fa18_os_disable_depth(uint8_t old_depth);
uint8_t fa18_os_enable_depth(uint8_t old_depth);

#endif
