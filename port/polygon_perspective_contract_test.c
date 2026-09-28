#include "polygon_perspective.h"

#include <assert.h>

int main(void) {
    FA18PolygonPerspectivePoint points[3];
    FA18PolygonPerspectiveOutput output = {0, points, 3};
    const FA18PolygonPerspectiveTriplet triangle[3] = {
        { -160, -160, 160 }, { 160, 160, 160 }, { 0, 0, 160 }
    };
    const FA18PolygonPerspectiveTriplet rejected[3] = {
        { 0, 0, 1 }, { 0, 0, 0 }, { 0, 0, 1 }
    };

    assert(fa18_project_polygon_perspective(triangle, 3, &output) == 1);
    assert(output.count == 3);
    assert(points[0].x == 0 && points[0].y == 0);
    assert(points[1].x == 319 && points[1].y == 179);
    assert(points[2].x == 160 && points[2].y == 90);
    assert(fa18_project_polygon_perspective(rejected, 3, &output) == 0);
    assert(output.count == 0);
    assert(fa18_project_polygon_perspective(triangle, 2, &output) == 0);
    assert(fa18_project_polygon_perspective(triangle, 4, &output) == -1);
    return 0;
}
