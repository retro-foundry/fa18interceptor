# Native gun shoot-down acceptance — 2026-10-08

The mode-eight gun shoot-down probe now passes 317 input/stage intervals and
275 bodies against original compared RAM/drawing. This includes all 101
consecutive bodies from the first damage hit through enemy expiry accounting
and eventual inactivation, with every body serial required. The former
C4F6DE return-cache mismatch is fixed in the connected native map renderer.
No gun damage threshold, collision, aircraft motion or expiry timer changed.

## Connected fix and source authority

The actual playable path is `native_frontend_tick` -> `native_flight_tick` ->
`native_scene_draw` -> `UPDATE_MAP` -> map `polygon` -> polygon clipping,
followed by placement dispatch -> `aircraft_descriptor` -> `record_vertices`
-> `record_finish`. Source C1F074 takes early aircraft expiry before the normal
C1F712 accumulator initialization; C1F8DE returns its retained -$7C(A6) word,
and C1CFBA stores that return in the alternate placement's +20 cache.

In the complete original failing body, that word is at C7FE78. The final
preceding writers are C24970 and C24956, the positive-X clip stage's calls
into C24996. Their return-address high word is $00C2. The map clip frame's
-$1E overlaps the later model frame's -$7C. Another writer occurs when the
negative-Y closing boundary calls positive-X directly: C249D6 in its final
negative-X child saves the current Y coordinate in the same word. If these
branches do not write it, the incoming value remains intact.

`clip_and_draw_polygon_retained` publishes those actual branch outputs through
an optional value pointer; the native map callback carries that value into the
existing model scratch. Other polygon callers use the unchanged ordinary
entry. The pure clipping owner and source geometry determine the output; no
reference RAM or CPU/stack interpreter is used by the playable runner. This
removes the map-to-expiry path's dependence on an unrelated initial host
scratch value while preserving the original's observable reuse of storage.
The shared legacy clip adapter initializes the optional output to null.

The expanded raster oracle checks 256 independently executed original clip
calls and complete drawing: 231 return-address residues, 20 saved-coordinate
residues and five unchanged values match. It also retains its 128 line-return,
128 clipped-segment-return and 160 polygon checks plus horizon/map comparison.
The new loop runs the reference last, preserving its completed hardware clock
between cases rather than rewinding it while leaving global blitter state live.
No comparison exclusion or gameplay fallback was added.

## Observed gun sequence and proof limits

The validation-only pilot still emits ordinary keyboard events through the
shared frontend. It does not write flight, target, projectile or outcome RAM.
The saved-pilot availability fixture is prepared before normal disk startup.

- Body 6534, ticks 14035–14037: gun log +60 advances 0 -> 1; enemy ten's
  damage advances 0 -> 1, flags $70C1 unchanged.
- Body 6535, ticks 14038–14041: gun counter 1 -> 2, damage 1 -> 2.
- Body 6536, ticks 14042–14044: gun counter 2 -> 3; damage remains two and
  flags become $72C1, selecting source destruction.
- Body 6537, tick 14047: the model starts the source 15-tick expiry, flags
  $54C1. Body 6540, tick 14058, now stores the exact original cache $00C2.
- Body 6552, tick 14098: the first expiry advances the enemy-aircraft expiry
  counter 0 -> 1 and clears the expiry flag. Source flags are $70C1 here;
  the record remains active and is followed through its later source updates.
- Body 6634, tick 14375: the same record is inactive, flags $0081. All 101
  bodies from first damage through this boundary match original RAM/drawing.

These are original instructions executed from native before-states, including
every body in the acceptance interval. They establish the actual damage,
destruction and subsequent source lifecycle in this connected scenario. They
do not establish an independent complete original mission replay, successful
mission completion, or every possible caller of retained model storage.

## Validation and restart

```powershell
python tools/native/check_mode_two.py --mode 8 --kill --gun --out build/native-flight/gun-kill-fixed
ctest --test-dir build/native-cmake -C Release --output-on-failure -j 1 -R '^fa18_native_(gun_hit|gun_kill|radar_kill|infrared_kill|raster|models|frontend|artifact_cleanup)$'
```

Gun shoot-down is now a CTest acceptance gate. Debug/Release playable and
fixture builds and all eight affected CTests pass. Compact sequence, hashes
and validation evidence are in `analysis/figures/native_gun_kill_checkpoint.json`.
Passing RAM is temporary and deleted after comparison. The resolved old
failure is retained locally compressed as `frame.79.*.dat.gz`; no raw capture
or media is committed. Optional `FA18_FRAME_TRACE_MODEL=1` and
`FA18_FRAME_TRACE_WORD=c7fe78` trace the independent original word's producers.

Next work is successful complete mission sequences and independent full-flight
comparison. Remaining callback contracts, typed game state, audio fidelity
and broader visible/combat performance remain open. The complete-port goal
stays active.
