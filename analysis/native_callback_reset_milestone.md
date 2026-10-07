# Native callback reset and fresh eligible-pilot fixtures

2026-10-07. Active build: `port/recomp/CMakeLists.txt` includes
`port/native/CMakeLists.txt`. Full gameplay/audio/performance acceptance remains
unfinished.

## Runtime connection

The host Delete mapping was absent. Raw $46 reached an unimplemented
COMMAND_RESET_CONTEXT child. The authoritative
`source_amiga/observed/run_input_callback_transition.asm` and existing domain
owners define C1C2B8 -> C06BF0 -> C1748C / C06C02 / C17456. C06C02 is empty in
the release executable; the other two remove/reinstall the input PAL callback.

Actual flight caller: SDL/replay event -> `native_frontend_event` -> keyboard
queue -> `native_input_process` -> `dispatch_keyboard_command` -> callback
reset. Menu/mission key dispatch uses the same native services.
`native_viewport_remove_callback` and `native_viewport_install_callback` call
the existing domain owners. Their OS boundary updates the host callback state.
Reinstallation executes source descriptor writes without resetting mouse
counters/bounds. No CPU or guest OS callback supplies game behavior.

## Original comparisons and runtime evidence

`check_input.py` passes **976** complete input-parent cases, including **64**
new Delete press/release/gate variants. A further **16** menu/mission Delete
command parents match original non-stack RAM and host installation state.
Original C06BF0/C1748C/C17456 instructions execute completely; only actual
Exec Add/RemIntServer is a host service boundary.

The shared playable runtime's Free Flight probe consumes both Delete events,
keeps the callback installed and continues through Escape/restart. **57**
actual input/stage intervals and **37** sampled bodies match compared original
RAM and every display byte, with unchanged exclusions. Input/stage comparisons
cover the reset; body comparisons cover subsequent rendering. These are
connected sampled checks, not an independent whole-flight proof.

```powershell
python tools/native/check_mode_two.py --mode 125 --callback --out build/native-flight/fresh-config-fixtures/callback
```

The CMake integration gate is `fa18_native_callback_reset`.

## Corrected eligible-pilot fixture

After the preceding file-policy batch, a fresh mode-eight probe failed with
zero scene frames. The test attempted C1643A before C162E4 established
MENU_FILE_READY, so source policy correctly skipped its save. Retained eligible
pilot files concealed this fixture mistake in the previous 28-test run. That
run was evidence on existing files, not proof of fresh fixture creation.

The validation-only fixture now enters enlistment before changing its one
availability byte, checks source readiness/status, saves, reopens normally and
requires the availability byte to persist. Runtime source gates and the disk's
locked-pilot behavior remain unchanged. Six fresh pilot checks pass:

| Scenario | Input/stage intervals | Sampled bodies |
| --- | ---: | ---: |
| Mode seven | 42 | 29 |
| Mode eight | 38 | 27 |
| Mode-eight ejection | 46 | 43 |
| Mode-eight weapon one | 44 | 38 |
| Mode-eight weapon two | 46 | 38 |
| Mode-eight gun | 48 | 36 |
| Total | 264 | 211 |

Each comparison uses the actual native startup/input path and original game
instructions on its exported starting state. No full original replay was
repeated. Successful player hits and whole mission outcomes remain separate.

Artifacts: `build/native-flight/fresh-config-fixtures/`. The initial fresh
fixture failure is retained in
`build/native-flight/config-owner/fresh-mode-eight/probe.log`.

## Build and regression checks

The full native Release build passes. Six selected CTest gates pass: callback
reset, audio programs, frontend/link omission, menu, viewport and menu startup.
The audio check covers voice programs, not complete audio fidelity. Protected
scripts/allowlist, sealed recordings and `.vscode/` remain unchanged.

The public native Release executable has SHA256
`AF38FF739033F272F5E5C639FA10EE784E27A6618B44AA89EECB28EDF2B7A56F`.
The complete-port goal stays active. Remaining recorder variants, independent
complete sequences, successful missions/combat, readable typed state, audio
fidelity and the 20 ms performance target remain unfinished.
