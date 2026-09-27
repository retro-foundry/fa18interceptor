#include "record_scan_scalar.h"

#include <assert.h>

int main(void) {
    uint16_t words[258];
    for (unsigned index = 0; index != 258; ++index) words[index] = 0x4000;
    const FA18SceneMagnitudeTable table = {words, 258};
    int16_t result[3];
    assert(fa18_calculate_record_scan_scalar(&table, 4, 4, 0, 0, result) == 0);
    assert(result[0] == 128 && result[1] == 0 && result[2] == 0);
    assert(fa18_calculate_record_scan_scalar(&table, -4, 4, 0, 0, result) == 0);
    assert(result[0] == -128 && result[1] == 0 && result[2] == 0);
    assert(fa18_calculate_record_scan_scalar(&table, 0, 4, 0, 0, result) == 1);
    assert(fa18_calculate_record_scan_scalar(&table, 4, 0, 0, 0, result) == -1);
    return 0;
}
