# Native frame work measurement - 2026-10-07

Visible complete flight (2026-10-10): qualification and mission three now
pass all 42,706 visible presentations with sound, complete final RAM/counters
and the earned pilot unchanged. Maximum measured work is 16.3617 ms, with no
frame above 20 ms. This covers cockpit view 0 on this host; other missions,
combat and camera views remain open. Earlier input-poll failures are retained
without an inferred cause. Runtime is unchanged; recorded sound timing remains
open. See [visible-flight evidence](native_visible_complete_flight_milestone.md).

The complete retained demonstration input now has a measured native frame-work
checkpoint. All 10,910 host-loop frames call SDL presentation in a normally
paced, hidden Direct3D window. Measured work peaks at 6.9374 ms, below the
requested 20 ms budget. This establishes one demo case on the measured host;
visible presentation and complete mission/combat performance remain open.

## Connected measurement

The active build is port/recomp/CMakeLists.txt -> port/native/CMakeLists.txt ->
fa18_native. Optional `--frame-times PATH` measures the actual main-loop caller:
host input -> native_frontend_tick -> native_audio_render/amiga_pcm_write ->
palette-to-ARGB conversion -> SDL_UpdateTexture/RenderClear/RenderCopy/RenderPresent
-> existing frame pacer. No emulation dependency is removed by this batch;
it removes the lack of measured evidence for this native runtime path.

CSV rows report every host-loop frame, not just scene updates. Metadata is
sampled after the game tick: iteration, selected mode, stage, view and whether
the scene update count changed. `presented=1` means SDL_RenderPresent returned;
it does not establish compositor visibility. `--hidden` exercises the actual
window/renderer without taking desktop focus. Headless rows have zero
conversion/presentation time and `presented=0`.

Work is input + game + audio + conversion + presentation, measured with SDL's
performance counter. Wait is the subsequent pacer/tail interval. Total is work
+ wait. Consecutive start intervals include the preceding CSV write. CSV I/O
is outside work/total; asset loading and SDL/audio creation are before the
measured loop. The first start interval is zero. No warm-up frames are removed.
The source physics, timers, game calls and normal pacing are unchanged; no
frame skipping or accelerated game behavior is introduced.

## Demo checkpoint

Host: Intel Core i9-12900KS, NVIDIA GeForce RTX 3090, Windows, NVIDIA driver
32.0.16.1714, MSVC Release, SDL 2.30.11. The real window run selected `direct3d`
and opened the audio device. Both runs consume captures/native/demo01/input.fa18in
through recorded update 4,892, with 2,337 scene updates and 47,161 model calls.
Scene views observed are 0, 1, 2, 3, 4, 5, 6, 10, 11 and 13. Some have only a
few updates; these counts do not establish all view/mission coverage.

| Measured path | Frames | SDL presentations | Work p99 | Work maximum | Work >20 ms |
| --- | ---: | ---: | ---: | ---: | ---: |
| Headless input/game/audio | 10,910 | 0 | 1.0118 ms | 1.6407 ms | 0 |
| Paced hidden Direct3D with audio | 10,910 | 10,910 | 3.8464 ms | 6.9374 ms | 0 |

The window run averages 1.8291 ms of work and 18.1523 ms in the pacer/tail.
Start intervals average approximately 20 ms; 6,250 of the 10,909 noninitial
intervals exceed 20 ms, with a 21.3941 ms p99 and 22.3133 ms maximum. Thus this
run meets the measured work threshold but does not establish a hard 20 ms
wall-clock presentation deadline. Hidden Direct3D also does not establish
visible compositor behavior. Full mission/combat, other hardware/backends,
visible window behavior and startup latency remain outside this checkpoint.

Commands used (separate fresh save overlays):

```powershell
# intro.e9k: E9K_INPUT_V1, Space down at frame 30, up at frame 32.
# This is an ordinary credits acknowledgement; source timers remain intact.
build/native/fa18_native.exe --headless --frames 30000 --input captures/native/demo01/input.fa18in --replay build/native-flight/performance/intro.e9k --save-dir build/native-flight/performance/demo-pilot --frame-times build/native-flight/performance/demo-headless.csv
build/native/fa18_native.exe --hidden --frames 30000 --input captures/native/demo01/input.fa18in --replay build/native-flight/performance/intro.e9k --save-dir build/native-flight/performance/demo-window-pilot --frame-times build/native-flight/performance/demo-window.csv
python tools/native/report_frame_times.py build/native-flight/performance/demo-window.csv --output build/native-flight/performance/demo-window.json --require-budget --require-presentation
```

Raw CSV/logs stay in the bounded, ignored build cache. Aggregate phases, view
counts and executable/input/CSV hashes are retained in
[figures/native_frame_performance_checkpoint.json](figures/native_frame_performance_checkpoint.json).
Percentiles use nearest rank. `report_frame_times.py` rejects incomplete frame
sequences, invalid durations and inconsistent sums; optional budget and
presentation requirements fail explicitly.

## Validation

The new fa18_native_frame_times contract compares instrumented/uninstrumented
ordinary Free Flight through 6,500 frames: complete exported RAM, final pixels
and all runtime counters are identical. It checks every CSV row and phase sum,
eight SDL dummy-window presentations, headless zero presentation and report
creation failure. Host timing thresholds are measured separately, avoiding a
machine-speed assertion in CTest. Native Debug/Release builds and five affected
Release checks pass: frame_times, frontend, game_input, samples and artifact
cleanup. Dummy video keeps the contract checks from taking desktop focus;
the full demo measurement uses the real Direct3D/audio devices.

No additional gameplay fidelity is established by timing. Complete outcomes,
rare stage/reset/HUD contracts, readable typed state and audio fidelity remain
unfinished, alongside broader performance acceptance.
