#include "exec.h"

uint8_t fa18_os_disable_depth(uint8_t old_depth) {
    return (uint8_t)(old_depth + 1u);
}

uint8_t fa18_os_enable_depth(uint8_t old_depth) {
    return (uint8_t)(old_depth - 1u);
}
