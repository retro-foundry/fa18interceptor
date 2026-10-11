# Native scene update ordering

The actual `fa18_native` path is frontend tick -> `native_flight_tick` ->
`native_scene_draw` -> `run_game_scene_sequence` -> existing host renderer
children. Native composition no longer duplicates the C0F048-C0F124 gates.
The reference C0EFD4 owner calls exactly the same shared sequence.

This fixes three deviations: followup/control drawing now stays inside the
signed position-bias gate; the flagged route reads C457AD (`ORIGIN_GATE_MODE`),
not `ORIGIN_ENABLE`; and that route calls only C1518C, omitting range selection
and followup drawing. Normal range branches preserve their opposite source
orders. Map gates and stage markers are owned by the shared source sequence.

C0D730's normal display child calls direct host active-plane rendering.
Its alternate C0DA38 page-presentation contract remains missing and aborts
explicitly if reached; the previous native early return silently continued
the rest of the enclosing frame. The shared sequence preserves an explicit
owner-finished result for the reference child and future native page owner.

Validation:

- MSVC native build and GNU source oracle build pass.
- Existing complete C0EFD4 controlled-child oracle: 1024 cases match every
  register, PC, SR and RAM byte; all 210 parent instruction boundaries reached.
  This proves parent contracts, separately from actual renderer child behavior.
- Native 7000-tick throttle/stick scenario reaches C10DAE: 677 record passes,
  675 scene/HUD passes, 12959 model calls, 9730 terrain polygons, game tick 329.
- Three native renderer checkpoints pass existing descriptor/vertex/plane and
  complete grid/control/followup parent comparisons against original bytes.
- Native setup, aircraft selection, pause/resume checks pass.
- Reference MSVC build and all twelve CTests pass; native link omission passes.

Copper fade is excluded. No full sealed replay repeated. Startup wiring
estimate remains roughly 98%; this normal-display scene-ordering batch is
complete, but complete frame ownership, Stores incoming-value contract,
end-of-frame drawing, double-page presentation, takeoff and recorded-flight
acceptance remain open. No new gameplay rules or substitute geometry added.
