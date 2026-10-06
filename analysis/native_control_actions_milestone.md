# Connected native control and sound actions

`fa18_native` -> frontend tick -> native flight tick now calls
`update_control_actions` before HUD drawing and in the inactive update branch,
matching C12950's C0EFD4 call sites C0F132/C0F370.

`port/game/control_actions.c` is the typed implementation of C12950 and its
C131BE/C133B2 magnitude/step calculations. It uses ordinary C locals and sound
arguments, with existing `five_eighths`, `long_divide` and audio consumers.
The instruction-observable reference owners remain in `control_readouts.c`.
The runtime action owner has no CPU working state, source stack or instruction
helpers. Source gates, signed-word overflow cases, pending-event priority,
nine-long programs, action variants and countdown mutations are retained.

The actual 7000-PAL-frame run executes 2454 active/inactive control passes and
reaches C10DAE. Compared with the preceding clock batch, three source bytes
change: the event countdown, action selector and pending sound events are now
consumed. Scene/record/control behavior remains connected.

`check_actions.py` builds the bounded original-instruction oracle and exercises
the real runner. The oracle checks 720 source-state cases and 1469 exact sound
requests, including function kind, argument count/order/values and every
non-stack RAM byte. It records original calls and executes their original
instructions; the typed path records its requests and calls the existing audio
consumers. No recording, ROM or CPU dependency enters the native runtime.
Cases include all action variants, eight phase values, six view values, both
factor routes, signed division, word absolute overflow, pending-event priority,
record-kind/blocked-event routes and four early exit gates.

Native MSVC and GNU oracle builds pass. The seven native view/record checkpoint
comparisons pass after connection. The native link map contains the new owner
and omits CPU, translation, glue, bus and chipset symbols. Copper fade is
excluded; no full sealed replay was repeated.

Free Flight startup wiring remains roughly 98%. This batch closes a complete
frame-owner dependency, but does not establish takeoff or recorded-frame
acceptance. Native sound requests still use source mute/absent-voice gates;
sample loading and audible output are unfinished. Remaining work includes
complete C0EFD4 composition, stores/end-of-frame owners, remaining record
children, double-page presentation and active-flight acceptance.
