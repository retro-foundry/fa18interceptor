# Native digit-4 / mode-125 connection

The normal digit-4 menu route now reaches mode 125's prompts and sustained
native flight. A disk/input run presents 2,211 scene/HUD frames over 18,000
host ticks, including Escape, menu return and a second entry into flight.
Complete mode outcomes and whole-game acceptance remain open.

The real caller is `native_frontend_tick` -> `native_flight_tick` ->
`run_post_input_tick` -> C0FECE. Its restore branch now connects C10B90's
aircraft refresh, C2949A's origin preset, C09148's position preset and
C28722's scene setup using existing game C. The native mode gate admits 125.
During actual flight this exposed two reached missing children:

- Model command `$120` maps through the original relocated command directory
  at C1FCE8 to C1FEE4. Its DBRA loop skips 18-byte entries according to the
  low four bits of STREAM_SKIP, with zero advancing nothing. Native model
  composition now implements this source operation directly.
- `update_control_records` -> C09E06 selects C0A334 for mode 125. Native
  record composition now calls `schedule_postflight(POSTFLIGHT_MODE_125)`.
  That existing source reconstruction owns the phase/outcome behavior.

No emulator, translated opcode or CPU bridge is added to the native link.
Original addresses identify source data and behavior; capture files remain
validation inputs to the separate oracle, never native runtime state.

The shared-runtime menu integration test now accepts mode 2 or 125. The
mode-125 run presses Space at 1800, digit 4 at 3000, Return at 5000 and 6500,
Escape at 11000, and Return at 15000 and 16500; releases follow two ticks
later. It samples the first body at each stage on both entries plus gameplay
after 128, 512 and 1000 scene frames. It requires menu return and resumed
C10DAE flight with over 2000 scene frames.

```powershell
python tools/native/check_mode_two.py --mode 125 --out build/native-flight/restore-check/original-check
ctest --test-dir build/native-cmake -C Release -R '^fa18_native_mode_125$' --output-on-failure
```

55 actual C0F3C4/C0F5F8 input/stage intervals and 35 C0EFEA/C0F3C0 bodies match
all compared original RAM and display bytes. The checks include expiring
prompts, active flight, C0F946/C0F974/C0F992 return callbacks and C0FCB4's menu
body. The existing oracle exclusions are unchanged: original stack, native
record locals and documented scratch/asynchronous voice/blitter polling.
No HUD pixels or additional rendering differences are masked. This proves
component behavior and runtime integration for these samples, not full
sequence parity, complete outcomes, audio fidelity or the 20 ms frame target.

Artifacts are in `build/native-flight/restore-check/original-check/`:
`captures.json`, `mode_entry-check.log`, `frame_body-check.log` and
before/after/source exports. No full original replay was repeated.

Regression validation passes mode 2's 32 source input/stage intervals and 21
bodies, seven affected native integration tests, and the current runner's
intro/credits/menu pixel, save/reload, SDL presentation and link-omission
checks. The delivered `build/native/fa18_native.exe` includes this batch.
