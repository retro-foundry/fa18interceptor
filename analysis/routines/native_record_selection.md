# Native record selection and readiness leaves

`port/native_record_selection.c/.h` implements `$C230B0`, `$C230E8`,
`$C23116` and `$C231A2` against the shared sixteen-record native bank. The
behavioral authority is `port/game/control_records.c` and
`port/game/flight_record_actions.c`, with their existing sealed readable-C
proofs and source-step adapters.

The selection release resolves the signed selected-record offset to an actual
native record. It retains an active record unless status bit one is set;
otherwise it clears the selected offset, active byte and marker in source
order. Invalid positive offsets fail explicitly because no unrelated guest RAM
exists in the native owner.

The two action selectors share the root record's low action nibble. The primary
entry permits release actions; the secondary entry does not. Release clears
the low nibble before its child, action nine gates the sound child on the live
origin-enable byte, action one publishes the three source bytes, and action
three calls the manoeuvre child. The ordinary decision result remains separate
from child completion.

Paired readiness examines the scheduler's retained companion record, matching
source A2 rather than the currently prepared A1 slot. It preserves exclusion,
active, partner, state and class tests and consumes the one-shot override only
on a ready result. Signed negative partner selectors resolve to the same native
bank; values outside its sixteen records fail explicitly.

These four leaves now run directly inside `native_control_record_update`.
Only their three actual nested action children remain explicit. Focused tests
cover retained and dropped selection, every action route, override and normal
pair readiness, partner exclusion and scheduler composition. The module has no
CPU state, guest addresses, bus, interpreter or machine dependency.
