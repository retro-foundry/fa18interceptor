# Native sustained flight and guidance limits

2026-10-07. The active `fa18_native` build uses the shared runtime from
`port/native/CMakeLists.txt`, included by `port/recomp/CMakeLists.txt`.
Complete gameplay, readable typed state, audio fidelity and performance
acceptance remain unfinished.

## Connected callers and fixes

Normal throttle/stick/target/fire input in modes six and eight reached an
unhandled guidance continuation. The actual caller is native frontend tick ->
flight stage -> frame body -> `native_records_update` -> dynamics ->
`update_dynamics_record_action`. C2C348 calls the release's empty C06C02, then
returns at C2C34E to the existing readable countdown/level owner. Native
composition now consumes AP_AFTER_FAULT with `fault_hook` and resumes that
owner. Other unresolved transfers still fail explicitly. No CPU supplies this
continuation in the playable build.

The extended original-body comparisons exposed three additional defects:

- C1F1D0 uses C2EC90's returned X/Y, including rejection. The published
  PROJECTED_PAIR can be -1 while the returned X is zero and Y is retained.
  The native target marker now uses the complete existing projection owner's
  typed return, and skips that call in the source's alternate vertex branch.
  Existing void entry points and CPU observers retain their contracts.
- C2DB9E's MOVEM.W sign-extends attitude words before C2DBA4 publishes longs.
  The native matrix route now publishes signed values.
- C2DEE0 scales a small divisor in D3 itself, retaining it for subsequent
  ratios. The port previously recomputed each axis with the initial divisor.
  The late mode-six NPC trace starts with $008F, retains $23C0 after C2E0EA,
  and returns roll $3840; the previous native result was $3720. The shared
  matrix calculation now retains this divisor, including its final return.

A focused check of the adjacent settling tail exposed another source-input
error: C2D70C loads the record's previous roll when flag bit four requests
settling. It replaces the extracted angle. The native tail now uses that same
previous value; its unrequested path keeps the newly extracted angle.

The projection and matrix owners remain in `port/game/`; native composition
remains in `port/game/native/`. Loading and entry ownership are unchanged.
No copied geometry, captured state or additional pixel/RAM exclusion supplies
runtime behavior.

## Runtime comparisons

The validation entry shares the actual playable runtime. It starts through
intro/menu and supplies normal keyboard events, running up to 60,000 host
ticks. The eligible mode-eight pilot uses the existing validation-only
enlist/save/reload fixture. No flight actor, motion, guidance or timer is
seeded. The sampler records actual first-seen action/countdown/altitude/angle
combinations and retains the body that changes mode or exits its owner.

| Normal-input scenario | Input/stage intervals | Sampled bodies | Scene/HUD frames |
| --- | ---: | ---: | ---: |
| Mode six sustained flight | 59 | 214 | 5,765 |
| Mode eight sustained flight | 55 | 251 | 14,859 |
| Total | 114 | 465 | 20,624 |

All compared gameplay RAM and display bytes match. The mode-eight original
frame bodies execute three C06C02 returns to C2C34E; the checker requires a
positive count. Mode six does not claim a sampled original guidance-fault
return. These checks restart original instructions from actual native
boundaries; they prove connected sampled behavior, not independent complete
flights or successful missions. Scene-frame counts are diagnostic, not exact
Amiga timing acceptance.

```powershell
python tools/native/check_mode_two.py --mode 6 --combat --out build/native-flight/combat-probe/validated-6
python tools/native/check_mode_two.py --mode 8 --combat --out build/native-flight/combat-probe/validated-8
```

The shared-runtime CTest gates are `fa18_native_combat_6` and
`fa18_native_combat_8`. The retained failures and traces are under
`build/native-flight/combat-probe/`, including `target-trace.log`,
`attitude-trace.log` and `matrix-angle-trace.txt`. Failed intermediate flight
counts are superseded by the validated runs above.

## Component comparisons

The native model oracle passes 512 complete C2EC90 return-coordinate cases,
including signed boundaries, rejection, invalid depth and last-row limits.
It compares full returned X/Y and non-stack RAM. The existing projection
family also passes 3,584 affected CPU/PC/full-SR/all-RAM cases: five callable
entries plus two internal clamp segments, 256 cases each with real and
controlled children. Internal segments are not additional callable owners.

The native record oracle passes 128 complete C2DB18 signed-attitude parents,
512 complete C2DEE0 product/extraction cases comparing returned angles/final
divisor/non-stack RAM, and 256 complete C2D704 settling tails. Its existing
256 publication parents and C12098/C1C63E checkpoint comparisons also pass.
These component checks are separate from the connected runtime evidence.

Normal mode-four region flight also passes 56 input/stage intervals and 49
sampled bodies after the matrix fixes. Its earlier 47-body count belongs to
the preceding implementation; this run retains the existing sampler.

## Builds and regressions

The full native Release build and both reference runner builds pass. All 36
selected native CTest gates pass: the two new sustained-flight gates plus 34
affected existing flight, postflight, renderer, input, frontend, qualification,
cockpit and audio/resource gates. The record checker passes all seven actual
startup/motion/stick checkpoints. Default setup-model comparisons pass. No
full original recording was rerun.

After the final build, the frontend/link check passes again: settled original
intro/credits/menu pixels, exact pilot save/reload and SDL presentation pass;
the native link contains no CPU, translation, glue, bus or chipset objects.
The ADF and protected scripts/allowlist remain unchanged. Audio program and
resource checks do not establish complete audio fidelity.

The public `build/native/fa18_native.exe` is refreshed. SHA256:
`658D6D1D46ADA0BB05DB6ED6A49DFBFD34CDB1D1E8432CE46796DE7C8671A252`.
Build/regression logs are under `build/native-flight/combat-probe/`.

The complete-port goal remains active. Normal-input mission successes,
independent full sequences, other combat/input variants, typed-state
migration, exact audio fidelity and the intended 20 ms frame budget remain
unaccepted. Copper fade remains the sole visual exclusion.
