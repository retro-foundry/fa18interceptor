# Native postflight schedule and result connections

2026-10-07; active build `port/recomp/CMakeLists.txt`, native runtime
`port/native/CMakeLists.txt`. This is progress toward the complete-port goal,
not whole-game acceptance.

## Connected behavior

Actual caller: `fa18_native` -> `native_frontend_tick` -> `native_flight_tick`
-> `native_records_update` -> C22C80 -> C09E06. Three controlled terminal
conditions initially stopped at unavailable native children 10 (mode four),
12 (mode five) and 9 (player readiness). The connected implementations are:

- C0A3EA: expose the existing source readiness logic as a typed result. Preserve
  its gear/contact/takeoff flags, carrier branch and zero-speed condition.
- C09EC4: prepare mode four's result view through existing C1BEE8 composition,
  retaining record four's +6 OR +12 value and STREAM_MODE.
- C0A002 -> C0A12E: restore both mode-five records' five-word view entries,
  including sign extension of the fifth word.
- Reached root control child C083A6: request the selected aircraft control
  through existing context publication, preserving its grounded gate.
- C110A4 -> C24FA4: queue the source indexed result message, mode/position/row
  arguments. C110A4 -> C11350 -> C08ED0: record the source mission outcome,
  grade, prior attempt, completion count and signed/unsigned level limits.

C11350 now has an ordinary-local-pointer entry used by native composition.
Its original frame entry and observer behavior remain available to CPU glue.
No game constants, terminal conditions or AI behavior are invented.

## Runtime and original comparisons

`tools/native/native_postflight_schedule_test.c` starts through normal menu
inputs, then seeds source terminal flags/positions **only in validation**.
It uses the actual runner's shared runtime object library for every subsequent
body and callback. Each case executes 16,000 host ticks:

| Controlled case | Input/stage intervals | Sampled bodies | Asserted result |
| --- | ---: | ---: | --- |
| Mode four | 5 | 9 | phase FF, event 4; C11078/C110A4 messages |
| Mode five | 1 | 8 | both C0A12E view-table restores |
| Ready player | 3 | 8 | phase FC; C11350 result and 78-byte persisted log |

All nine input/stage intervals and 25 bodies match compared original RAM and
display bytes, with existing RAM/display masks unchanged. The ready
fixture verifies the saved file equals its native result table.

The result-parent oracle additionally passes **63** derived original-parent
cases. Its 24 new FC cases cover modes four through seven, normal and overflow
level bytes, unsigned attempt clamping, and word completion-count wrapping.

**Persistence scope:** C1643A is the existing shared native config-write
boundary in result comparisons. Its game-side disk/status decisions are
excluded, explicitly selected with `--shared-config-write` in the new actual
interval checker. Exactly one ready-player interval reaches this boundary.
The original wrapper C539F4 calls DOS offset 48, **Write**, despite the older
readable helper's `load_menu_mode_file`/`MF_FILE_READ` naming. Its status and
readiness gates remain to be integrated and compared; these checks do not
accept the complete C1643A owner. Four/five intervals use the default original
stage execution without this boundary option.

Repeat the affected controlled comparison with:

```powershell
python tools/native/check_postflight_schedule.py
```

Evidence: `build/native-flight/postflight-schedule/original-check/`, including
each case's `captures.json`, before/after exports and original comparison logs.

## Regression and remaining work

Normal-input mode-four flight still passes **56** actual input/stage intervals
and **47** sampled bodies, including takeoff, regions, NPC missiles and player
hit/restart. This is separate from the controlled terminal fixtures.
Evidence: `build/native-flight/postflight-schedule/normal-flight/`.

All **26** affected native CTests pass; all seven native-only host/loading
checks pass. Release reference targets `fa18_recomp` and `fa18_romfree` rebuild.
The native Release link map has no CPU/glue/bus/recomp/machine symbols.
`scripts/check_native_build.py` was left unchanged: it targets the retired
`port/CMakeLists.txt` and cannot check this active build. The active CMake build
and Release link map provide the current structural evidence.

The validated executable is copied to `build/native/fa18_native.exe`, SHA256
`E7C9200E439A06225140D55F281132116BF4C190D439DCE53AE8858A0C12888F`.

These fixtures do not prove normally played successful missions, mode-five
completion, whole independent flight sequences, complete audio fidelity or
the 20 ms presentation target. C1643A status/readiness integration, remaining
combat and other mission/result variants remain open. The complete-port goal
stays active.
