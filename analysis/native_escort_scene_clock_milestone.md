# Escort initialization: actual clock dependency — 2026-10-10

The first escort scene difference is now attributed to **different stored
microsecond samples**. Native's complete actual C0FECE input/setup interval
matches original instructions under its own inputs. Changing only that timer
fraction in an external original-instruction probe reproduces **all 16 complete
original record cores**. The playable runtime receives neither reference RAM
nor fitted clocks. Complete escort flight parity still fails.

The connected caller is `port/native/main.c -> native_frontend_tick ->
native_flight_tick -> advance_delayed_menu -> MENU_DELAY_ROOT ->
initialize_scene_from_mode`. The original corresponding call is
`C0FF7C JSR C28722`, returning to C0FF82. The initialization rule uses the
stored C16D04 timer fraction at C45AF6; its low word C45AF8 selects a variant
and signed coordinate shifts. This is a gameplay input dependency, beyond
matching a constructor's output for one fixed input.

The original probe replays normally through observation 36,448, preserving
**every preceding observation**, the real bus and controls. It exports actual
C28722/C0FF82 RAM/register boundaries at observation 36,447 and checks the
actual stack return. Source inputs, complete snapshots and probe identities
are retained compressed. The current playable native executable starts a new
pilot independently, earns qualification and the complete preceding mission,
and captures actual update 36,445 without changing any recorded controls.
The full subsequent replay preserves its final RAM, earned save, event report
and every ordinary counter. Single-frame timing comes from the runner's
actual summary fields.

| Initialization input/result | Original | Native |
| --- | ---: | ---: |
| Stored microseconds | 976,083 | 600,000 |
| Low word C45AF8 | 58,579 | 10,176 |
| Retained variant | 2 | 0 |
| Signed X shift | -1 | 0 |
| Signed Z shift | 0 | 0 |
| Selected record stream | C2996E | C2996E |

The original instructions explicitly use `bits & 3` at C28782, and signed
`(bits >> 1) & 3` / `(bits >> 2) & 3` at C28CAE. The source sample therefore
selects variant two and X shift minus one. Native's sample selects zero for
each. Slots 8 and 14 differ initially; their positions and slot 8's resulting
orientation/matrices follow this source dependency. The stream happens to be
the same in this escort case despite the different retained variant.

Validation keeps the two questions separate:

- The external original C0FECE oracle produces **zero compared RAM
  differences** for the actual native entry/setup output. Its existing named
  frame-local/ABI exclusions remain unchanged; no new exclusions are added.
- Supplying the recorded original fraction to **only the external oracle**
  matches every byte of all 16 actual original 164-byte cores: **2,624 bytes**.
  It still fails comparison against unchanged native output with **33 RAM
  differences**. These differences are preserved, rather than relabelled as
  a passing independently recorded flight.
- Flipping original fraction bits 1, 2 and 4 gives only 14 matching original
  cores in each case. These three dependency controls reject their altered
  inputs, and the strict native-output comparisons also fail as expected.
- Native memory reports show zero project gameplay heap violations, zero
  SDL arena failures and zero gameplay pool requests for this headless replay.

The timer acquisition limitation is now concrete. `native_clock_request`
publishes `(pal_ticks % 50) * 20000` microseconds. Every value is divisible by
32, so the five low bits used by these original placement/variant rules are
always zero. The 50-value cycle is retained explicitly. Thus the existing
host timer contract suppresses source variation when SEQUENCE_FLAG is clear.
Different rendering cadence alone does not justify eliminating that input
variation. Timer resolution needs implementation assessment; this batch does
not inject a seed, replace a clock, insert a delay or claim that later escort
resets are explained by initialization alone.

Reproduce using the retained original owner captures:

```powershell
python tools/native/check_escort_scene_clock.py --runner build/native-cmake/native/Release/fa18_native.exe --reference build/native-flight/independent-escort-event-input --source-evidence build/native-flight/original-escort-failure-boundary --source-updates build/native-flight/escort-failure-update-entries --source-body build/native-flight/escort-source-scene-initializer --out build/native-flight/escort-scene-clock
```

The new optional `FA18_MODE_SOURCE_OUTPUT` exports reference output from
`native_mode_entry_oracle.c`; its default comparison stays strict. Both this
component check and the complete actual playable replay run in the helper.
The [checkpoint](figures/native_escort_scene_clock_checkpoint.json) binds
source captures, native snapshots, reference output, logs, actual source and
executable identities. Default diagnostic bounds and the build pruner remain
enabled. Earlier single-frame capture scripting incorrectly requested a range
timing sidecar; the reusable helper uses the actual summary and completes.

Original timer-resolution fidelity, subsequent escort flight rules/outcomes,
successful escort coverage, broader full-flight comparisons and recorded sound
timing remain open. No playable behavior changes in this evidence batch.
