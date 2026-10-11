# Connected native game clock

Runtime caller: `fa18_native` -> frontend tick -> native flight tick ->
`begin_main_loop_timers` / `poll_main_loop_timers` (C25312), then
`sample_main_loop_readout` (C2548A) and C0EFD4's game-counter increment.
Game updates are distinct from the runner's 50 Hz PAL clock. The original
C2502E disk table supplies the threshold; the current index 15 gives 67 ms.

The existing timer owner now exposes its prelude and one poll separately.
Its reference API still runs the same blocking loop. Native polling yields
until a later host clock sample satisfies the source's elapsed-time/divisor
comparison. The accumulator and second counters execute once per game update.
Pending polls do not repeat physics, scene drawing, HUD or final text. Counter
increment follows timer/readout/periodic work and respects the source pause gate.

The shared native clock supplies timer.device seconds/microseconds to C16D04;
setup, flight controls and frame polling reuse it. Periodic C28996 region work
now executes through source C28B16's mode gate. Reached C28E28 zone checks use
the existing resumable source owner. Native children pass explicit working
values; reference adapters keep their existing observer path.

Periodic page-top clearing uses two host plane-page allocations separate from
recorder/mask/scratch storage. C082B8 requests redraws; C10B90 remains the
distinct aircraft/root reset. C53F9C releases graphics sprite zero, which this
runner never allocates. Source tone requests use their mute/absent-voice gates;
this does not implement sample loading or audible native output.

Validation:

- Native MSVC build; connected startup, aircraft selection and throttle motion.
- Pending polls at PAL frames 6501/6502 preserve record, scene, HUD, glyph and
  game-counter counts and the complete player record; only polling advances.
- Pause holds the game counter at 6350/6370; resume advances it again.
- Seven view/record checkpoints match original non-stack RAM. Stick saturation
  is tested at 6880 instead of 6850, allowing enough source-rate game updates
  to reach the unchanged one-step ramp's limit 20 before release at 6900.
- 36 timer/readout cases match all non-stack RAM and acquisition counts against
  original instructions; 33 yield while waiting. Cases cover all eight divisor
  phases, reset flags, elapsed notifications, counter limits, first sampling,
  long elapsed sentinel and the uncapped disk-table entry.
- Two complete C1C63E passes at periodic phases 3/19 match original non-stack
  RAM, including reached C28996 region work.
- 115 HUD/message cases at each of 5300/6100/6500 match original non-stack RAM.
- Native frontend/link omission checks pass. Reference MSVC build and twelve
  CTests pass; GNU oracle builds pass.

Rough Free Flight startup wiring estimate: about 98%, previously 97%. This
excludes takeoff, full flight and recorded-run acceptance. The native timer
samples in 20 ms PAL increments; its oracle uses the same scripted external
clock events on both sides. This proves timer behavior under those samples,
not original CPU-paced display-frame timing. Complete C0EFD4 ownership,
C12950 control/audio actions, remaining HUD/record children, double-page
presentation and flight acceptance remain open. Copper fade is excluded.
No full sealed replay was repeated.
