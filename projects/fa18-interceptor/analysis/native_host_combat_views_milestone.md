# Default-host combat and cameras — 2026-10-10

The current playable runner's **default windowed host clock** passes bounded
combat/camera clips in **mission five and the final mission**: a combined
**43,100/43,100 presentations**, live sound, airborne raised gear, all **14 source
camera modes in each mission**, and **zero frame work measurements over 20 ms**.
Maximum work is **16.2690 ms**. This is actual default acquisition, extending
the preceding grounded Free Flight check.
It does not establish complete mission outcome or original whole-flight/sound
parity.

The runner is unchanged from `8017cd10`, with executable SHA-256
`36cad74383f5beaad810378f03a973b8a93a9778b906655de112ffad39f80287`.
The active path remains `port/native/main.c -> native_frontend_tick /
native_audio_render -> SDL renderer and timing writer`. The checker loads
the actual previously earned pilot through the existing save loader and
replays ordinary keys. Its added G/keypad commands reach `command_selection.c`
and `view_commands.c`. It supplies no flight RAM, original clock samples,
new random seeds, physics adjustment or scene override.

`measure_visible_combat_views.py --clock host` deliberately omits the runner's
clock option to check the actual windowed default. Its complete trace and every
timing row establish observed coverage. Existing `--clock pal` checks retain
their strict independent headless/window RAM/PCM/pixels/save/counter equality.
The host check has different acquired inputs; it makes no such equality claim
and does not weaken the PAL comparison. It rejects missing presentations,
missing rendered camera modes, missing airborne gear/combat, timer precision
loss, model faults, resets, pending input, memory violations or frame work over
budget. Accepted or rejected host clips retain complete compressed RAM/trace
after lossless equality verification. Timing rows remain uncompressed.

Final-mission measurements:

| Camera | Rendered scene frames | Maximum work (ms) |
| --- | ---: | ---: |
| 0 | 2,510 | 16.2690 |
| 1 | 96 | 10.5636 |
| 2 | 95 | 15.4757 |
| 3 | 85 | 8.7165 |
| 4 | 84 | 11.7763 |
| 5 | 77 | 15.4222 |
| 6 | 73 | 9.5612 |
| 7 | 85 | 10.5563 |
| 8 | 77 | 14.1173 |
| 9 | 74 | 7.8315 |
| 10 | 70 | 8.8856 |
| 11 | 64 | 13.6271 |
| 12 | 60 | 6.2295 |
| 13 | 60 | 13.0229 |

The final-mission flight trace retains 8,759 observations, including 3,293 airborne,
485 with active missiles and 8,548 with active aircraft. Gear raises at frame
10,707, after takeoff, and stays raised throughout this combat clip. The source
flight route is sealed; the checker adds only the recorded gear/camera controls.
The actual 15,305 timer acquisitions retain all 32 low-bit patterns.

The final-mission 32 MiB SDL arena satisfies two bounded gameplay pool requests
(71,152 bytes startup, 75,392 peak). Project gameplay heap violations and SDL
pool failures are zero. The PCM ring retains 131,072 frames / 512 KiB.
OS/driver allocations and compositor scanout are outside these observed scopes.
There is no gameplay memory allocation or timing implementation change in this
batch.

Mission five's corrected clip also presents all 21,550 frames with sound,
all 14 rendered views and no resets or model fault. Its maximum work is
**16.2689 ms** (mean 1.7917, p95 4.6180, p99 7.9445 ms). The complete trace
retains 9,106 observations: 3,147 airborne, 247 with active missiles and 8,895
with active aircraft. Takeoff is observed at frame 10,931; the ordinary G
press at 11,100 raises gear at 11,107. It stays raised through this clip.
All 14,666 actual timer acquisitions preserve all 32 low-bit patterns.
Its arena startup/peak use is 71,264/75,504 bytes, with two bounded pool
requests, zero pool failures and zero project gameplay heap violations.

The earlier mission-five clip remains rejected: its fixed G press at 10,700
precedes flight entry at 10,724 and takeoff at 10,930. Gear stays down through
the first crash; two source gameplay restart paths are observed. It still
presents every frame, renders every camera and stays within the work budget,
but does not qualify continuous gear-up combat. This is a game crash/restart
outcome, not a missing native model-command abort.

The checker now accepts a declared `--gear-frame` and independently requires
that press to follow observed takeoff and precede retraction. Four controls
reject actual early commands and wrong early/late annotations. It also avoids counting a destroyed
aircraft's gear bit as successful retraction. The retained final-mission run
passes the stronger check without another window run; the premature mission-
five command rejects. Original controllers, sealed routes and game code remain
unchanged. The corrected added G command is the only replay input change.

A controlled PAL comparison confirms the early G fails to raise gear while
the late G raises it after takeoff; **both PAL runs have zero restarts**.
Both actual host runs also have identical initial 16 record cores, but acquire
their own subsequent timer inputs. The earlier host restarts are therefore
not attributed solely to gear. Their cause remains unqualified; no game rule
is changed or failure erased to produce the passing clip. Full reports, both
complete host traces and the controlled PAL traces/RAM remain compressed.
Passing diagnostic native WAV copies are removed after their hashes are
checked, with those hashes retained; original recordings are untouched.

Evidence is retained under `build/native-flight/host-final-combat-views/`,
`build/native-flight/host-stolen-combat-views-gear11100/` and the rejected
`build/native-flight/host-stolen-combat-views/`. The controlled gear comparison
is `build/native-flight/stolen-combat-gear-controls.json`; actual host flight
entries are retained in `build/native-flight/stolen-host-entry-comparison.json`.
The [checkpoint](figures/native_host_combat_views_checkpoint.json) binds the
runner, actual pilot admission, complete route, every view's measured frames,
all timing rows, final outputs and compressed complete trace/RAM.
The preceding [clock evidence](native_host_clock_milestone.md) separately proves
the original timer owners and preserves deterministic diagnostic playback.

```powershell
python tools/native/measure_visible_combat_views.py --mode 8 --clock host --out build/native-flight/host-final-combat-views-new
python tools/native/measure_visible_combat_views.py --mode 5 --clock host --gear-frame 11100 --out build/native-flight/host-stolen-combat-views-new
```

The visible window closes automatically after about seven minutes. Other
default-host mission/view measurements, independent complete flights and
recorded original sound onset/handoff/waveform remain open. Campaign continuity
remains waived; named-state migration stays outside this goal.
