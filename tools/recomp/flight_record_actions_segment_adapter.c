/* Test-only access to two internal source segments. Neither is registered. */
#include "../../port/game/glue/glue_flight_record_actions.c"
int fa18_test_flight_record_append(void) { append_flight_record_stream(working(),&hooks); return glue_return(); }
int fa18_test_flight_record_motion(void) { apply_flight_record_action_motion(working(),&hooks); return glue_return(); }
