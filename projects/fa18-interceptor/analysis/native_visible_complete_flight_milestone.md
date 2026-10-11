# Visible qualification and mission-three performance - 2026-10-10

The current native executable passes the complete visible qualification and
mission-three replay with sound. All **42,706 frames** return from SDL
presentation, all measured frame work stays below **20 ms**, and complete final
RAM, every runtime counter except audio-device availability, and the earned
pilot agree with the accepted headless reference. Maximum work is **16.3617 ms**;
mean/p95/p99 are **1.7416 / 4.7501 / 7.7786 ms**. No frames are removed.

| Connected scene path | Scene updates | Maximum work | Frames above 20 ms | Views |
|---|---:|---:|---:|---|
| Qualification | 1,857 | 12.2790 ms | 0 | Cockpit, 0 |
| Mission three | 3,770 | 16.3617 ms | 0 | Cockpit, 0 |

The actual caller is `port/native/main.c`: ordinary host input and
`SDL_PollEvent`, `native_frontend_tick`, sound rendering/publication, palette
conversion and Direct3D submission/presentation. The existing frame pacer is
unchanged. Timing includes input and presentation cost; deliberate subsequent
wait is reported separately. CSV writes occur after the measured work interval.
No emulator/original capture or CPU-heavy reference oracle ran concurrently.
Lightweight source inspection and observer guard tests continued during replay.

The runner remains SHA256
`7ebadbd5cb2ac17df99751dda3dc3f804f57792d0762b0a32f7accac08a815a0`.
A fresh pilot is enlisted with ordinary keys; qualification and the mission
consume the accepted input and manage gear. No original RAM initializes native
gameplay. The runner closes automatically after the complete recorded sequence.
This batch changes evidence only, with no gameplay or host-loop implementation
change and no emulation dependency removal.

## Limits

This establishes one current visible route on this host, using `direct3d` and
the real audio device. The route uses cockpit view 0. Other missions, combat
and other camera views still need visible measurement. SDL presentation return
does not instrument compositor scanout, and the work threshold does not impose
a hard wall-clock presentation deadline on host scheduling or the frame wait.

Earlier visible failures are retained: their input-poll stalls remain included
in their failed budgets. The fresh successful run does not establish what
caused those stalls. SDL's pinned Windows implementation dispatches OS messages
inside event polling and handles move/resize/menu loops, but no corresponding
event trace proves those loops caused the historical failures.

The [checkpoint](figures/native_visible_complete_flight_checkpoint.json) retains
the complete phase summaries, worst rows, counts, executable/input/report/CSV
identities, final RAM/save and acceptance result. Raw CSV and logs remain in
bounded build storage. Independent complete mission-three drawing remains
accepted; recorded sound handoffs/timing and broader flight/performance coverage
remain open. Campaign continuity is waived and named-state cleanup is deferred.

## Reproduction

```powershell
python tools/native/measure_visible_performance.py --mission-report build/native-flight/frame-delta-trace-preservation/Release/report.json --mission-input build/native-flight/patrol-entry-review/update-consumed.fa18in --runner build/native-cmake/native/Release/fa18_native.exe --out build/native-flight/visible-complete-cockpit-current
```

Allow the window to remain visible through qualification, menu transitions and
mission three; it closes automatically after approximately 14.2 minutes.
