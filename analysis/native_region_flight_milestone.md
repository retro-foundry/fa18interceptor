# Native region flight, NPC missiles and restart

Normal mode-4 input now takes off, spawns region aircraft, follows their zone
exit placement, launches NPC missiles and reaches the player's hit/restart
sequence. The shared playable runtime runs 30,000 host ticks and 5,970
scene/HUD frames. The test supplies throttle, stick, Return, T and Space through
the normal input interface; it seeds no gameplay records or pilot unlocks.

The real caller is `native_frontend_tick` -> `native_flight_tick` ->
`native_records_update` (C1C63E) -> control-record update (C22C80). Its periodic
C28996 region branch calls readable C28B16/C28B34. Native composition now
connects their existing C28F16 placement and C2D954 orientation children.
The reached C28E28 zone exit also continues through C28F16 instead of aborting.
These remove missing-child boundaries in the playable runner; the reusable
placement and matrix operations retain their existing domain ownership.

Rendering the spawned model reaches command $B4, directory entry C21EF8.
`derive_workspace_midpoint_extensions` writes seven workspace points at
+$1E..+$42 with wrapped midpoint, reflection, quarter-displacement and
translation arithmetic. It consumes no operand and returns zero. All 64 new
complete original geometry parents match result and non-stack RAM/display;
the combined derived-tail regression now checks 320 parents.

Complete-body comparison exposed a caller error at the NPC launch gate:
C231A2 tests the owning aircraft in A2, whereas native composition passed its
inactive paired missile. The native caller now supplies the aircraft. The
probe observes missile records 9 and 13 (mask $2200), region-spawned records
12/13 ($3000), and record 12's zone exit ($1000).

Following this combat route exposed two more live composition defects:

- C207FE's flagged-view latch ends a carrier surface, but C1F7DA's whole-list
  completion flag still applies at C1F716. Native rendering now preserves
  that completion gate instead of interpreting adjacent address data as $C3.
- The selected-reference heading arm can reach C241A6's sight tail without
  replacing its caller's viewer. Native record composition now carries the
  explicit viewer between root-view and record-dispatch operations. Original
  tracing at the first C11788 restart body showed record 4 uses the root
  viewer and its $7FFF range; using address zero caused an extra normalized
  vector publication. The corrected body matches without masking that vector.

`build/native-flight/region-flight/viewer-fixed/captures.json` retains 56 actual
input/stage intervals and 47 sampled frame bodies. Every interval and body
matches the original instructions' compared RAM/display with zero differences
and unchanged exclusions. Samples include C11788, C11830 and C11872 after the
player's hit transition. This is sampled parent parity from the native
trajectory, not an independent original replay of the whole flight or proof
of all collision branches, mission outcomes, audio fidelity or performance.

Reproduce with:

```powershell
python tools/native/check_mode_two.py --mode 4 --flight --out build/native-flight/region-flight/original-check
python tools/native/check_models.py --runner build/native-cmake/native/Release/fa18_native.exe
```

The added `fa18_native_region_flight` CTest uses the same runtime objects as
the actual runner. It checks movement during the flight, region spawn/exit and
NPC missile activity; final position may reset during postflight. The three
default setup-model comparisons pass, including 320 geometry parents each.
The reference runners build and all 12 affected host/loading checks pass.
All 22 affected native integration tests pass in 335.12 seconds, including
all mission entries, ejection, the three firing probes, countermeasures,
scene exit, expiry, frame bodies, frontend/menu/input and cockpit assets.
The affected ejection/restart comparison also passes all 46 input/stage
intervals and 43 sampled bodies under the new viewer and carrier composition;
evidence is in `build/native-flight/region-flight/ejection-regression/`.
Full gameplay sequences, other combat/outcome branches, exact audio and the
20 ms presentation target remain unaccepted. The complete-port goal stays active.

Validated public executable: `build/native/fa18_native.exe`, SHA256
`346e4e8bc2b519d6fc8437564040b744326651ffed13adb5f62cbaf8cefc874e`.
