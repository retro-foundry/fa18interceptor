# Visible final-combat camera performance - 2026-10-10

A paced, visible final-mission combat clip with sound passes **all 21,550
presentations** and **all 14 source camera modes**. Maximum measured frame
work is **17.7416 ms**, with **no frame above 20 ms**. Every complete PCM,
final RAM, final pixel and saved-pilot byte matches an independent headless
replay; all runtime counters agree except the actual audio-device flag.
This measures a partial mission on this host, not a complete final-mission
outcome, all other missions or compositor scanout.

| Camera mode | Rendered scene frames | Maximum frame work (ms) |
|---|---:|---:|
| 0 | 2,521 | 17.7416 |
| 1 | 95 | 7.8449 |
| 2 | 94 | 9.1881 |
| 3 | 84 | 13.2371 |
| 4 | 83 | 9.4315 |
| 5 | 76 | 12.1209 |
| 6 | 72 | 12.1762 |
| 7 | 84 | 12.4898 |
| 8 | 76 | 10.3530 |
| 9 | 73 | 11.8882 |
| 10 | 69 | 8.4438 |
| 11 | 63 | 8.7711 |
| 12 | 60 | 8.2903 |
| 13 | 60 | 9.6035 |

The native executable remains `7ebadbd5...`. Its real caller is
`port/native/main.c` -> `native_frontend_tick` / `native_audio_render` -> the
existing SDL presentation and frame-time writer. Ordinary keypad events reach
`command_selection.c` and `view_commands.c`; no camera state is written by the
checker. The benchmark adds normal G and camera keys to the retained final
flight route, loading the actual earned new-pilot save admitted by the earlier
tour. Native RAM is never seeded, and game clocks/RNG remain unchanged.

The headless trace records 8,759 pre-input observations, including 612 with
active missiles and 8,548 with active aircraft. Gear retracts at frame 10,707,
after the aircraft becomes airborne, and remains raised through this clip.
The visible replay uses a real audio device and the Direct3D renderer.
The existing 32 MiB SDL arena satisfies two gameplay pool requests with no
failures; guarded project gameplay heap violations are zero. The PCM ring
remains 131,072 frames. Private OS/driver allocations are unobserved.

Two rejected headless preparations remain retained. The first pressed G
before takeoff and reached only four camera modes because target selection
reset the camera. The next preparation reaches all modes with gear raised,
but continuing the retained gear-down control route with G added reaches a
damage/crash/restart outcome after frame 21,666. The accepted performance clip
ends at 21,550, after all camera modes and before that later outcome. No
visible timing rows were excluded or used to choose this endpoint; the full
visible clip preserves its independently measured headless state exactly.
These are benchmark input outcomes, not evidence of a native runtime fault.

The checker requires every timing row, every SDL presentation, scene rendering
in each camera, complete state/PCM/pixel/save equality and the 20 ms work gate.
Passing duplicate WAV/RAM/pixel/save files are removed only after direct whole
byte equality. All timing rows and reports remain; reusable native RAM and
the diagnostic flight trace are retained losslessly compressed. Earlier failed
preparations are compressed with their actual outcomes and retention hashes.
The normal capture and build-cache budgets remain enabled.

The [checkpoint](figures/native_visible_combat_views_checkpoint.json) binds
the route, actual earned pilot, executable, commands, every view's measurements,
complete output hashes, failed preparations and retention. Evidence is under
`build/native-flight/visible-final-combat-views-v3`. The earlier complete visible
qualification/mission-three result remains separate. Visible coverage of other
missions and original/native sound timing/waveform acceptance remain open.

```powershell
python tools/native/measure_visible_combat_views.py --mode 8 --out build/native-flight/visible-final-combat-views-new --prepare-only
python tools/native/measure_visible_combat_views.py --mode 8 --out build/native-flight/visible-final-combat-views-new
python scripts/prune_build_artifacts.py --quiet
```

Use a new output directory. Leave the visible window open for about seven
minutes; the checker closes it when the complete measured clip ends.
