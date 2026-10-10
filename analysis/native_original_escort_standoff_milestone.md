# Original escort deck contact and missed wire — 2026-10-10

The external pilot now reaches the original carrier deck at the correct height,
with gear down and hook extended. It misses the arrestor and rolls off without
a new escort grade. All **65,000 observations**, complete final RAM/registers
and consumed inputs reproduce exactly in an unmodified independent original
replay. This changes the next input investigation from descent height to the
actual wire geometry; it does not qualify a successful full escort flight.

`--wait-for-approach-height standoff` waits for the existing requested approach
height instead of the higher final-mission gate. Bare
`--wait-for-approach-height` retains its earlier meaning. Defaults and existing
native validation callers remain unchanged. These options affect only the
validation controller's ordinary keyboard choices, not game clocks, physics,
randomness or landing rules. The adapter still runs through the original
physical keyboard IRQ and is not part of `fa18_native`.

The driver now records its executable, controller C/header and adapter hashes
before launching the process. Later workspace edits cannot relabel the run.
There is no playable dependency removal or new gameplay allocation. The native
executable remains SHA-256
`36cad74383f5beaad810378f03a973b8a93a9778b906655de112ffad39f80287`.

The first **46,126 complete observations** match the preceding higher-gate
recording. The first difference is 46,127; combat succeeds at the same original
42,816 boundary. The new height gate admits final approach at **817.5625**,
below its actual **819.03125** target, with horizontal approach distance
227.523106. No fitted timing or coordinate offset is supplied to the game.

At **46,958**, the aircraft contacts the deck at
`(1136674.53125, 119.03125, 1071022.828125)`, contact `8082`, deck region `C0`
and gear byte zero. It retains **30 grounded deck observations**, through
46,987, with the hook bit set. The arrest flag (`4000`) never appears.
The final position is `(1136674.53125, 2.9296875, 1073419.546875)`, speed zero,
contact `8082`, mode four, phase one and one completed mission.

Original C26EBE (`flight_geometry.c`) checks the grounded aircraft's second
probe point against the carrier's vertices at record offsets **488/494/500**,
with the original scaling and heading test, before setting arrest bit 14.
The actual original carrier geometry retained in complete RAM is:

| Wire vertex | World X | World Y | World Z |
| --- | ---: | ---: | ---: |
| 488 | 1,136,584 | 112 | 1,070,684 |
| 494 | 1,136,704 | 112 | 1,070,520 |
| 500 | 1,136,560 | 112 | 1,070,548 |

Its centroid is `(1136616, 112, 1070584)`. The controller currently aims its
landing path through the takeoff home `(1136684, 119.03125, 1071520)`.
The recorded deck touchdown occurs beyond the wire triangle. The next
validation input should aim through the live original wire geometry, preserving
the observed aircraft clearance and original arrest decisions. These numbers
are evidence, not captured coordinates to hardcode into gameplay.

Validation comprises the external original runner build, exact earned prefix,
the complete independent original replay, Python compilation and clean diffs.
Three profile controls reject either mismatched target and establish that
legacy metadata with the bare flag passes profile validation before a deliberately
corrupt trace rejects. No additional expensive replay is used for those controls.
The route contains **3,722** repeated steering events. Its input target and
source identities are recorded explicitly; old metadata defaults to the earlier
final gate.

Complete compressed evidence and keys remain under
`build/native-flight/original-escort-standoff-height`. Raw passing captures and
duplicate replay files are removed; the build pruner completes. The
[checkpoint](figures/native_original_escort_standoff_checkpoint.json) binds the
actual outcome, source geometry, complete replay, profile guards and hashes.

```powershell
python tools/native/check_original_mission_recording.py --mode 4 --source-prefix build/native-flight/original-mission-three-patrol-runway --repeat-steering --wait-for-approach-height standoff --out build/native-flight/original-escort-standoff-height
```

Successful original escort landing, broader independent native full-flight
comparisons and original sound timing remain open. Campaign continuity remains
waived; named-state cleanup remains a separate task.
