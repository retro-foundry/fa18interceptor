#include "graphics.h"

uint16_t fa18_os_vbeam_row(uint32_t shifted_beam_position) {
    return (uint16_t)(shifted_beam_position & 0x1FFu);
}
