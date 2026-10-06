# Native flight sound resource initialization

The playable caller is `fa18_native` -> `native_frontend_open` ->
`native_audio_load_resources` -> C17510/C1756A, then
`load_setup_text_resources` (C1787A). The existing C1787A owner now consumes
native ADF loading, C5046C descriptor creation, C50614 duplication and the
original C50A58/C50B36 square-wave/noise generators. CPU, glue and chipset
objects remain absent from this runner. This removes missing engine, alert,
noise and programmable sound availability and the skipped startup random
sequence; it does not implement audible output.

C1787A retains its original sample ranges, repetition counts, programmed
callback identifiers and availability decisions. Missing textegn tries the
original textegn2 alternate; other missing samples return zero so the original
owner controls availability. Host allocation exhaustion reports an error.
No reference RAM supplies native game state or samples.

Validation:

- All 15 sample buffers match the existing original initial checkpoint byte
  for byte, including the generated square wave and 2,048 noise bytes.
  Availability bytes are exactly $07/$F7. RANDOM_SEED is exactly $6DD578DA.
- 23 original instruction cases cover the previous intro/menu/descriptor
  cases plus C1787A success, alternate engine loading, programmable and noise
  allocation failures, and missing alert/scripted/cockpit/airflow files.
  Source square-wave/noise instructions execute in the fixture; their bodies
  are not replaced by the native implementation. All non-stack RAM matches
  apart from the explicitly corresponding two-byte host local.
- All 4,892 native demo updates complete. The 31 source startup/launch cases
  and record parents at update 2,400 and the end pass. Startup lead remains
  37 ticks versus the reused original checkpoint, down from 97 before menu
  resources: about 62% of that measured gap is removed.
- The native carrier recording completes all 8,038 updates and 554 consumed
  keys, lands, saves qualification success and restarts. Fifteen source result
  parent cases and save/reload pass.
- The crash recording consumes all 168 keys and returns to the menu. Re-entry
  now reaches original C1072E's code prompt with the correct availability
  flags. Return and Space continue into active qualification at C10DAE. Four
  dirty-bank reset comparisons match original non-stack RAM.
- Native frontend/menu/save/SDL/link omission checks pass. Both MSVC reference
  runners build and their twelve focused CTests pass. The ADF is unchanged.

Startup sound initialization is **3/3 source parents complete**, with all
15 sample buffers checked. This percentage describes initialization only.
Per-tick voice programs, playback/output, full menu setup and remaining
message/viewport/frame/result cadence remain open. Functional scenario wiring
is 3/3; full native frame parity remains **0/3 accepted**. Copper fade is
excluded. Existing reference captures were reused; no full original replay
suite was repeated.

Run `python tools/native/check_sound_resources.py`. Its optional
`--source-initial build/native-flight/reference-demo-initial.dat` and
`--source-checkpoint build/native-flight/reference-demo2401.dat` arguments
reuse the original artifacts. Carrier and crash/re-entry regressions are
`check_qualification_result.py` and `check_sequence_return.py`.
