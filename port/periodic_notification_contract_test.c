#include "periodic_notification.h"

#include <assert.h>

int main(void) {
    FA18PeriodicNotificationState state = { 2, 0 };
    assert(fa18_update_periodic_notification(&state) == 0);
    assert(state.countdown == 1 && state.code == 0);
    assert(fa18_update_periodic_notification(&state) == 0);
    assert(state.countdown == 8 && state.code == 0x86);

    state = (FA18PeriodicNotificationState){ 5, 0 };
    assert(fa18_update_periodic_notification(&state) == 0);
    assert(state.countdown == 4 && state.code == 6);
    state = (FA18PeriodicNotificationState){ 7, 0 };
    assert(fa18_update_periodic_notification(&state) == 0);
    assert(state.countdown == 6 && state.code == 4);

    state = (FA18PeriodicNotificationState){ 10, 0x55 };
    assert(fa18_update_periodic_notification(&state) == 0);
    assert(state.countdown == 8 && state.code == 0);
    state = (FA18PeriodicNotificationState){ 0, 0x55 };
    assert(fa18_update_periodic_notification(&state) == 0);
    assert(state.countdown == 8 && state.code == 0x86);
    state = (FA18PeriodicNotificationState){ 0x80, 0x55 };
    assert(fa18_update_periodic_notification(&state) == 0);
    assert(state.countdown == 8 && state.code == 0);
    assert(fa18_update_periodic_notification(0) == -1);
    return 0;
}
