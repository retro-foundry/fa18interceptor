# Free Flight interpolated-segment crash — 2026-10-09

The user reported a Free Flight location-three abort:

```text
native model stream C3ABEA parameters C3AAC4 record B800 code 4018
native model missing draw command at 000018
```

The connected caller is `port/native/main.c` → `native_frontend_tick` →
`native_flight_tick` → `native_scene_draw` → `visit_followup_placements` →
`native_scene_placement` → `native_model_draw` → the model command dispatcher.
The original directory at C1FCE8+$18 points to C206E4. The native dispatcher
omitted that valid command; its explicit unsupported-command abort caused this
failure. The existing descriptor C228B8 selects the reported disk model C3AAC4.

`draw_interpolated_segments` in the existing `port/game/draw_stream.c` now owns
the complete C206E4-C207FC operation. Six stream words supply a division count,
two endpoint pairs and a colour. It stores the two interleaved intermediate
lanes at C4AD90, preserving signed DIVS overflow and low-word extension. It rounds
each coordinate in sixteenths using the original ASR carry, draws count-1 segments
through the existing C2EE4A clipped-line owner, and preserves the caller's -$30
countdown and -$7E accumulated result. `native/model.c` dispatches index $18 to
this owner. No geometry is skipped and no substitute drawing is introduced.

All native model missing-path diagnostics now print the player coordinates
from CONTROL_RECORDS +20/+24/+28 divided by 256, plus the original pose index,
mode and stage, flushing stderr before aborting. Pose index is the internal
zero-based preset value, rather than the numbered Free Flight menu label.

Validation uses the original instructions with ports OFF:

- 72 complete command cases cover counts 2..7, signed rounding, visible and
  rejected/clipped segments, and DIVS overflow. The test invokes the native
  `4018` dispatcher and the complete original C206E4, comparing all non-stack
  RAM/display, stream position and returned word.
- 16 component camera poses render the actual immutable C3AAC4 model through
  the full scene descriptor and C1ED48 model owner. Eight original C206E4 calls
  are required. Model returns, transformed vertices, records, non-stack source
  data and plane bytes match. These camera inputs belong to the isolated oracle;
  they are not seeded into playable flight.
- The existing `fa18_native_models` gate runs both new comparisons alongside
  its existing connected setup, scenery, aircraft and scene-parent checks.
- The location-three ordinary-key Release smoke reaches 50,000 host ticks,
  13,416 scene frames and active flight C10DAE with no crash reset. Its counters
  match the same straight flight before the fix; it is not an exact automated
  reproduction of the user's route.
- The user subsequently flew past the same affected area and reported no
  crash. This is manual playable evidence, without a sealed input recording.

Release and Debug builds pass. `fa18_native_models`,
`fa18_native_flight_start` and the required artifact cleanup pass in both
configurations. A temporary, oracle-only copy of the startup snapshot replaced
the reported command with an unsupported ID to exercise the failure diagnostic;
stderr reported the exact stored player coordinates divided by 256, pose index,
mode and stage before termination. The temporary snapshot was removed; no disk
asset or playable state was patched. Both final builds include the diagnostic,
and canonical `build/native/fa18_native.exe` is refreshed. Hashes and results
are in `analysis/figures/native_free_flight_interpolation_checkpoint.json`.

The independently started whole-flight, campaign, audio, performance and named
state acceptance work remains open. The unsupported-command guard remains for
other unimplemented commands, now with the requested player-position output.
