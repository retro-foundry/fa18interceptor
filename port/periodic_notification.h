#ifndef FA18_PERIODIC_NOTIFICATION_H
#define FA18_PERIODIC_NOTIFICATION_H

#include <stdint.h>

/* Direct mutable state at `$C45890/$C4588E`, owned by the parent-update
 * prefix.  The notification-code meaning remains intentionally unassigned. */
typedef struct {
    uint8_t countdown;
    uint8_t code;
} FA18PeriodicNotificationState;

/* `$C11B44-$C11BAF`: decrement the signed byte countdown, publishing the
 * observed periodic codes and preserving the source's signed clamp test. */
int fa18_update_periodic_notification(FA18PeriodicNotificationState *state);

#endif
