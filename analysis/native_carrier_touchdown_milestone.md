# Native carrier touchdown milestone

Scope: the live C149BE -> C083E2 touchdown reset in `fa18_native`, using
original consumed keyboard input. This bounded landing milestone is 100%
complete. The complete qualification result/message sequence remains open.

The caller is native frontend -> native flight tick -> C1C63E record update
-> C25B66 dynamics -> C149BE controls -> `FC_RESET_CONTROL`. The existing
`begin_mission_reset()` now replaces that missing boundary directly. It posts
message $4005, replenishes player mission fields, requests redraws and clears
all 164 bytes in records 1-3. No CPU, chipset or capture-loaded game state is
used by this runtime path.

The original C083FA-C08408 instructions access player phase C45798. The
older helper incorrectly used attempts-left C45898. The helper and its comment
are corrected; mode 6 preserves player phases 1/$FF and clears other phases,
without changing the attempts counter.

Validation:

- Eight original C083E2 cases compare all non-stack RAM through the actual
  native touchdown child. Both modes 6/9, player phases 0/1/2/$FF, an independent
  attempts counter and dirty complete records 1-3 are exercised. The comparison
  caught the wrong field before correction and passes afterward.
- Native cold startup and consumed carrier keys through update 6288 match the
  original iteration-6289 aircraft record except the pre-existing region bit 2
  and countdown offset of 16. Position, all orientation matrices, velocities,
  speed, carrier ground/contact flags $C082, deck height $7708, stopped aircraft
  and mission reset fields agree. No crash reset occurred.
- `tools/native/check_touchdown.py` repeats that bounded native comparison with
  supplied original input/RAM evidence, without rerunning the reference replay.

Next: successful qualification reaches the currently missing C11078 result
callback, then C110A4 result messages/pilot-log update. One of three sealed
scenarios is functionally complete; carrier landing itself is now demonstrated,
but complete carrier outcome and video-frame parity are not yet accepted.
Copper fade is excluded. Original startup flag/countdown differences, full
frame ownership, audio and demo gameplay remain open.
