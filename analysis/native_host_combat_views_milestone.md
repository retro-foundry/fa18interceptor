# Default-host combat and cameras — 2026-10-10

The current playable runner's **default windowed host clock** passes a complete
bounded final-mission combat/camera clip: **21,550/21,550 presentations**, live
sound, airborne raised gear, all **14 source camera modes**, and **zero frame
work measurements over 20 ms**. Maximum work is **16.2690 ms**. This is actual
default acquisition, extending the preceding grounded Free Flight check.
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

The flight trace retains 8,759 observations, including 3,293 airborne,
485 with active missiles and 8,548 with active aircraft. Gear raises at frame
10,707, after takeoff, and stays raised throughout this combat clip. The source
flight route is sealed; the checker adds only the recorded gear/camera controls.
The actual 15,305 timer acquisitions retain all 32 low-bit patterns.

The existing 32 MiB SDL arena satisfies two bounded gameplay pool requests
(71,152 bytes startup, 75,392 peak). Project gameplay heap violations and SDL
pool failures are zero. The PCM ring retains 131,072 frames / 512 KiB.
OS/driver allocations and compositor scanout are outside these observed scopes.
There is no gameplay memory allocation or timing implementation change in this
batch.

Evidence is retained under `build/native-flight/host-final-combat-views/`.
The [checkpoint](figures/native_host_combat_views_checkpoint.json) binds the
runner, actual pilot admission, complete route, every view's measured frames,
all timing rows, final outputs and compressed complete trace/RAM.
The preceding [clock evidence](native_host_clock_milestone.md) separately proves
the original timer owners and preserves deterministic diagnostic playback.

```powershell
python tools/native/measure_visible_combat_views.py --mode 8 --clock host --out build/native-flight/host-final-combat-views-new
```

The visible window closes automatically after about seven minutes. Other
default-host mission/view measurements, independent complete flights and
recorded original sound onset/handoff/waveform remain open. Campaign continuity
remains waived; named-state migration stays outside this goal.
