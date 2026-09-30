#ifndef FA18_GAME_ACTIVE_PLANES_H
#define FA18_GAME_ACTIVE_PLANES_H

/* $C2FD8C: four active cockpit-plane blits and the selected-table display
 * stage. This routine is not registered until its counted waits are proved. */
typedef struct ActivePlaneHooks {
    int (*select_record)(int wide, void *context);
    int (*prepare_polygon)(void *context);
    void (*composite)(void *context);
    void *context;
} ActivePlaneHooks;

void submit_active_planes(const ActivePlaneHooks *hooks);

#endif
