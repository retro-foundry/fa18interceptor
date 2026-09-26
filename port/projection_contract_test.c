#include "projection.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    /* Synthetic arithmetic contract. A run060+ tuple/output fixture is still
     * required before this primitive is promoted to scenario parity. */
    const FA18ViewVertex sample[4] = {
        {-799, 793, 2444}, {-866, 786, 2421},
        {-1474, 882, 3455}, {-1407, 889, 3478}
    };
    const FA18ScreenPoint expected[4] = {{211, 60}, {216, 60}, {227, 67}, {223, 66}};
    FA18ScreenPolygon polygon;
    memset(&polygon, 0, sizeof polygon);
    if (fa18_project_polygon(sample, 4, &polygon) != 1 || polygon.count != 4) {
        fputs("projection rejected\n", stderr);
        return 1;
    }
    for (uint16_t index = 0; index < polygon.count; ++index) {
        if (polygon.point[index].x != expected[index].x ||
            polygon.point[index].y != expected[index].y) {
            fputs("projection arithmetic fixture failed\n", stderr);
            return 1;
        }
    }
    const FA18ViewVertex clipped[3] = {{30000, -30000, 1}, {-30000, 30000, 1}, {0, 0, 1}};
    if (fa18_project_polygon(clipped, 3, &polygon) != 1 ||
        polygon.point[0].x != 0 || polygon.point[0].y != 179 ||
        polygon.point[1].x != 319 || polygon.point[1].y != 0 ||
        polygon.point[2].x != 159 || polygon.point[2].y != 89) {
        fputs("projection clamp contract failed\n", stderr);
        return 1;
    }
    const FA18ViewVertex rejected[3] = {{1, 2, 3}, {4, 5, 0}, {6, 7, 8}};
    polygon.count = 99;
    if (fa18_project_polygon(rejected, 3, &polygon) != 0 || polygon.count != 0 ||
        fa18_project_polygon(sample, 2, &polygon) != -1) {
        fputs("projection rejection contract failed\n", stderr);
        return 1;
    }
    puts("polygon projection contract passed");
    return 0;
}
