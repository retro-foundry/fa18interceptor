# Complete native PAL input callback

The actual caller is `fa18_native` -> `native_frontend_tick` ->
`native_viewport_tick`, before the C50158 voice callback. It now executes the
complete C1718E owner, rather than entering only the C172DE palette/fade tail.
The missing prefix updates counter deltas, wraps them at signed byte limits,
halves negative Y when PLAYER_READY is clear, clamps both control accumulators,
saves counters and increments INPUT_TICKS. The existing tail still updates
viewport state and master audio volume during suspended gameplay frames.

C0F51A-C0F534 supplies -960..960 to C17104. Native startup now calls the shared
bounds owner with these values instead of retaining the executable's earlier
0..319/199 UI bounds. C1712C initializes the acquired counters and C17456
publishes the original callback node. Its host registration connects the PAL
callback already driven by the frontend; no Exec server or chipset is emulated.
This is **1/1 (100%) of the complete PAL input-callback connection**. It is not
a whole-game parity percentage.

The shared game owner has explicit counter-sample entries. Reference callers
retain their original hardware acquisition; native callers supply ordinary
host byte counters. Both invoke the same delta/clamp/state/palette logic.
SDL relative mouse motion and left/right edges now feed that input boundary.
The native E9K frontend replay supports the original `m` and `b` row meanings
for device 0/4 alongside existing keyboard rows. Unsupported devices/buttons
fail explicitly. Raw consumed game-input recordings retain their K-only scope.
Physical gameport input and original mouse velocity/scanline acquisition timing
remain outside this batch; the supplied counter sample's semantics are proven.

Validation:

- 128 complete source C17104/C1712C/C17456 startup cases compare all non-stack
  state against the actual native adapter, including host registration.
- 128 sequences of 50 complete C1718E callbacks (6,400 ticks) compare all
  non-stack state and RGB4 publication. Cases exercise both readiness states,
  positive/negative movement, signed-byte wrap edges, ordinary/inverted signed
  clamp bounds, input-tick overflow and viewport/master-volume transitions.
- The existing reference hardware caller passes 128 complete original-byte
  comparisons, including all registers, PC, full SR and all RAM. Its documented
  structural-write-log build variant is required by that oracle.
- Actual native mouse replay checks counter wrapping and saturation at
  +960/-960. Left/right press/release rows reach source C0F3C4's INPUT_STATE_WORD
  during active Free Flight. INPUT_TICKS advances while gameplay/display
  counters are suspended. Settled frontend pixels, pilot save/reload, SDL and
  CPU/chipset link omission checks pass.
- All eight sampled assembled gameplay bodies still match every compared
  state/display byte; their oracle now runs the complete PAL input callback
  across the supplied interval. The unchanged drawing comparison continues to
  ignore Copper fade, with scratch/async voice/busy exclusions documented.
- One independent demo gameplay checkpoint, aligned at game tick 222, matches
  both original 320x200 four-plane page buffers and player motion/pose/matrices
  byte for byte. This reuses the existing source checkpoint and excludes
  loading duration; it is stronger than an isolated frame-body comparison.
- Voice cadence, complete native demo, carrier success/save/restart/reload,
  crash/menu/re-entry and twelve reference host/loader contracts pass. No
  original full replay suite was repeated.

The latest user acceptance scope is **gameplay frames**; intro/loading duration
may differ. The old 36-update lead is a preflight observation, not by itself an
acceptance blocker. Full gameplay-sequence parity remains unaccepted for all
three recordings. See `native_gameplay_acceptance.md` for the current gate and
the next state/timer alignment investigation.
