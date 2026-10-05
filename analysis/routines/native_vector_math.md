# Native vector normalization and origin adjustment

`port/native_vector_math.c/.h` implements complete `$C2574A` and the argument
entry `$C25754` on ordinary numeric inputs and shared magnitude/output owners.
Both run the actual `$C1D974` primitive. The sealed original instructions are
authoritative; `port/game/` reconstruction corroborates the arithmetic.

Component and scale low words retain source wrapping, including the negative
`$8000` absolute value. `$C25754` takes direction from its first argument's
high word independently of the low-word scale. Zero scale clears the three
output words while preserving magnitude and carried axis. Nonzero scale
publishes magnitude before the factor search; zero magnitude clears outputs.
The signed power-four search, DIVU overflow, signed low-word products,
six-bit shift count and direction reversal retain original order and widths.
The full magnitude preserves high bits when its signed clamp replaces the low
word. Planar MULU now uses unsigned multiplication before signed conversion,
removing C signed-overflow behavior exposed by the original-byte proof.

The original upward search can reach zero and repeat forever. The oracle proves
that invariant directly, without treating an instruction limit as completion.
The native routine returns failure, retaining earlier magnitude/axis writes and
leaving the final output words unwritten. Missing field data also fails.

`fa18_load_scene_magnitude_window` binds original Hunk-8 bytes at offset `$1710`,
retaining preceding and following hunk data for signed table offsets. The disk
proof compares all 258 words to the sealed source and checks the preceding
word/window identity. Data outside that hunk requires explicit adjacent owners.
Production does not import captures or recreate a guest address space.

Record view invokes this routine directly with scale 192; its normalization
callback is removed. The scheduler requires the same magnitude table/word as
selected range and the same output triple as view. Record-update composition
requires active-origin adjustment to share that same math owner.

`native_selector_origin.c` delegates `$C29548` to the existing ordinary-state
`terrain_selector_origin_adjustment.c` body. It now runs actual normalization
with scale 512 and publishes shared magnitude/output words. Candidate reduction,
saved shift, signed scaling, smoothing, origin addition and masked/negated
companion publication preserve source behavior. The tail API exposes its live
arrays rather than CPU temporaries; its proof compares those arrays and all
game RAM. The surrounding active-origin adapter retains four real lower
families: matrix preparation, transforms A/B and candidate regeneration.

Validation on 2026-10-05:

- `check_native_vector_math.py`: 24,576 calls, 98/98 boundaries, 23,367 complete
  calls and 1,209 proven source loops; no child contracts. Full magnitude,
  carried axis, normalized words and game RAM match.
- `check_native_selector_origin_adjustment.py`: 32,768 calls, 143/143 boundaries,
  31,559 complete calls and the same 1,209 core-entry loops; no child contracts.
  All 8,192 origin-tail calls complete and match their live outputs.
- Updated record-view proof: 24,576 calls, 482/482 boundaries, 9,104 actual
  normalization calls and 759 actual release fault returns; no child contracts.
- Updated dispatch proof: 106,496 calls, 1,095/1,095 boundaries, 9,728 actual
  normalization calls and 774 actual release fault returns; no child contracts. Independent typed
  records, decisions and returned companion identity also match.
- Selected-range proof rerun after the shared magnitude change: 8,192 calls,
  137/137 boundaries, actual magnitude and 3,277 explicit sound contracts.
- MSVC Release native game/affected builds, strict GNU compilation of fifteen
  affected units, ten affected CTests and the unchanged 513-file native guard.

CPU, ROM and captured RAM are used only by the original-byte validation tools.
The native implementations have no CPU dependency. Full startup aliases,
remaining pose/control/flight/render/audio owners and native-main integration
are still required for the whole game to become independent of emulation.
