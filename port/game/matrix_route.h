#ifndef FA18_GAME_MATRIX_ROUTE_H
#define FA18_GAME_MATRIX_ROUTE_H

/* $C2DB18: select and publish the active control record's matrix route.
 * Hooks expose the two boundaries where temporary 68000 glue needs the
 * unscaled matrix and the return state of the nested transform. */
typedef struct MatrixRouteHooks {
    void (*after_transform)(void *context);
    void (*before_final_scale)(void *context);
    void *context;
} MatrixRouteHooks;

void update_control_record_matrix_route(const MatrixRouteHooks *hooks);

/* $C2D99C: dispatch to the view or control-record matrix route. */
void dispatch_matrix_route(void (*view_route)(void), void (*record_route)(void));

#endif
