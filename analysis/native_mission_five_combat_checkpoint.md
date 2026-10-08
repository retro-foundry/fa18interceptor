# Mode-five combat diagnostic checkpoint - 2026-10-08

Ordinary input reaches seven gun-hit increments across six observed frame
bodies in mode five. All 26 input/stage intervals and the first 57 sampled
bodies match original compared RAM/drawing, including those six hit bodies.
The next sampled body exposes a reproducible reset-vector mismatch. This is
an investigation checkpoint, not acceptance of a completed mode-five mission.

## Connected path and validation changes

`fa18_native_mission_success_test` links the existing playable runtime. Its
keyboard pilot has moved into `tools/native/mission_pilot.c`, linked only into
the test executable. It reads flight records and emits ordinary frontend key
events; it never writes flight/outcome RAM or injects original reference RAM.
Mode three remains the default and its existing CTest acceptance is unchanged.
Optional final argument `5` selects the mode-five briefing with F3 and cycles
Return three times to select the gun. Space fires only during the chosen aim
intervals. The observer captures actual gun/radar counter changes and reports
record flags and expiry lifetimes. Its contact expectation follows C0A3EA:
carrier region bits $C0 for pose three, runway bit $04 otherwise.

The real runtime path remains `native_frontend_tick` -> `native_flight_tick`
-> native record updates. The mode-five pilot pursues the stolen aircraft and
nearby enemies, but does not complete the objective or carrier return. It
crashes and stops after one natural reset at tick 20,352. Completion count
stays 3; phase is 4; the fixture returns failure. The ADF starts with a radar
hit count of 1, which remains unchanged; this is not a new radar hit.

## Unresolved original/native mismatch

Capture 57 is body 8,705, iteration 13,164, stage C11830, before/after tick
20,352 and saved tick 2,810. The original frame oracle reports two gameplay
byte differences and zero display-byte differences:

| Address | Original | Native |
| --- | --- | --- |
| C45A4F | $F7 | $F8 |
| C45A51 | $94 | $95 |

The complete normalised component words are source $FFF7/$FF94 (-9/-108)
and native $FFF8/$FF95 (-8/-107). With `FA18_FRAME_TRACE_WORD=C45A4E`, the
original publishes $FFF7 at C257C2 with D0=$01800180, D1=$00007FFF and D2=16.
That identifies the normalisation publication; it does not yet establish the
cause. No rounding adjustment, gameplay fix or comparison-mask change was made.

Trace the actual arguments and length through
`port/game/main_loop_flight_controls.c`'s `normalise_main_loop_control_vector`
and the connected `root_control_child` length callback in
`port/game/native/records.c`. Compare C25754/C2574A and C1D974 outputs with the
original before changing those owners. After fixing the reset comparison,
capture all relevant enemy expiry boundaries and finish the normal-key
mode-five objective/carrier return. Expiry accounting and complete mission
success are not proven by the observed hit bodies.

## Validation and retained evidence

Debug and Release fixture builds pass. Debug mode-three success and cleanup
pass; Release mode-three success, frontend, artifact policy and cleanup pass.
Mode three still compares 36 input/stage intervals and 184 bodies, including
all 96 consecutive landing bodies and the actual config write. The final
mode-five fixture reproduces the same input bytes and the same failing reset
before/after RAM as the original diagnostic. The playable executable remains
SHA256 `35228ba51693afc342ae88e94728608c89cd4beb011c98c63e5df21c7c71c0bd`.

The tracked metadata is
`analysis/figures/native_mission_five_combat_checkpoint.json`. Original comparison
logs and the compressed failed case remain local under
`build/native-flight/mission-five-gun-source-check/`; the fresh fixture replay
is under `build/native-flight/mission-five-checkpoint/`. Decompress the three
`failure/frame.body.57.*.dat.gz` files into temporary storage to repeat the
original comparison, then run:

```powershell
python scripts/build_recomp.py --output build/recomp/native_frame_body_oracle.exe --main tools/native/native_frame_body_oracle.c
build/recomp/native_frame_body_oracle.exe <temp>/frame.body.57.before.dat <temp>/frame.body.57.after.dat 20352 20352 2810 <temp>/frame.body.57.source.dat
```

The expected current result is failure with exactly the two bytes above.
Original oracle builds must run serially. Existing masks remain unchanged;
these comparisons execute original instructions from native before-states
and do not independently replay a complete original flight. Capture exports
remain capped at 240 cases/480 MiB, passing RAM is temporary and removed,
and build/CTest pruning retains the 4 GiB budget. Preserve media, sealed
recordings, user settings and the three protected validation files.
