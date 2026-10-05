# Native record pose and motion history

`port/native_record_pose.c/.h` implements complete `$C25B66` and the actual
`$C2651E` motion-history child against the shared sixteen-record bank. The
scheduler calls pose directly for root and accepted dispatches. The authority
is the sealed instruction graph, corroborated by
`port/game/flight_dynamics.c`. Working inputs are angles, velocity, candidate
points, message/slot choices and sound arguments; no CPU register file or
guest-memory interface is exposed.

The owner preserves the non-root cell route before the event gate, selection
and expiry handling, control/alert/matrix routes, fixed-width integration,
coordinate packing, grid-cell publication, action/view decay, collision
classification, motion-slot selection and final history update. The source's
negative-X path clears Z while retaining negative X. Signed-overflow gates
use the mathematical result before wrapped comparisons. The high-nibble decay
uses the original masked low nibble, rather than retaining a new action code.

Motion history retains the stride/selected-history/event gates, signed row
offset, duplicate tuple while count is below five, six-row cursor wrapping,
count saturation and per-record byte countdown. It writes through caller-bound
field owners. Negative rows can overwrite count/index/selected-history fields;
later operations re-read those live owners. Missing fields fail with preceding
stores retained. Invalid record identities fail explicitly. The seventh tuple
and preceding owners must be bound when the original route accesses them.

The quarter-rate arm `C26058-C26068` cannot execute from this complete entry:
the unchanged class byte is loaded, masked and tested twice, with a zero-class
exit after the first test and a nonzero branch after the second. The audit
checks all eight exact instructions before excluding that fall-through arm.

Fourteen lower boundaries remain explicit: cell matrix (`C2D970`), zone/selected
record (`C28E28`), record action (`C2C392`), controls (`C1B27E`), messages
(`C25704` at three actual return sites), selector (`C13D84`), matrix (`C2D408`),
flight (`C149BE`), motion candidate (`C26EBE`), sound (`C17F8C`), fault
(`C06C02`), ground projection (`C26322`), region probe (`C2B05A`) and motion
slot (`C26352`). Their successful contracts preserve caller identity, allow
shared record mutations and return a separate candidate status/clear decision.
Their actual native implementations remain open.

Validation: `python tools/recomp/check_native_record_pose.py` passes 16,384
complete calls (8,192 each for pose and history), at all 595/595 reachable
original boundaries. Actual motion-history instructions run inside the pose
oracle. All Chip/Slow RAM matches except CPU ABI stack `C7FD00..C7FF00`;
typed aircraft, matrix and position owners are independently verified. Fixtures
cannot overwrite audited instruction bytes. They cover all sixteen callers,
cell and event routes, expiry, signs/overflow, packed positions, action decay,
collision outcomes, warning suppression, history duplication/wrapping and
negative rows with aliased metadata.

Ordered child contracts check cell angles, candidate points, projection
velocities, message/slot choices, both sound arguments and original return
sites. They vary live records, collision status/decision and region flags.
Counts in enum order are 44/2773/1053/2858/271/286/3827/1927/5626/3/46/388/388/907.
These are parent contract comparisons, not proof of the actual lower children.
See `analysis/figures/native_record_pose_checkpoint.json`.

MSVC Release game and affected contracts, strict GNU compilation, thirteen
affected CTests and the 501-file native build guard pass. Composed scheduler
tests exercise real action decay before readiness, direct secondary playback,
dispatch/pose decisions and shared-state rejection. Bootstrap assertions now
observe the actual matrix child within root pose. Five scheduler children
remain: periodic, primary/secondary placement, dispatch and finish. The native
main, original runtime asset/state bindings and actual lower owners remain open.
