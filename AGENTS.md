# Repository instructions

Read `CURRENT_PORT_HANDOFF.md`, `PORT.md` and `port/README.md` before work.
The user's latest instructions override historical plans and proof notes.

## Active port

- Latest user direction (2026-10-06): `fa18_native` starts a native runtime
  using the existing `port/game/` sources. First milestone is intro -> menu.
  Native game composition belongs in `port/game/native/`; the entry/build in
  `port/native/`; reusable asset loading stays in `port/amiga/`. This explicitly
  supersedes the earlier prohibition against a new native runner below.
  Emulator runners remain comparison references. See `port/native/README.md`.

- Implement game behavior in `port/game/`; temporary CPU adapters belong in
  `port/game/glue/`. The actual runners are `fa18_recomp` and `fa18_romfree`,
  built by `port/recomp/CMakeLists.txt` and `scripts/build_recomp.py`.
- Reusable host/loading code belongs in the existing `port/amiga/` facilities,
  with game-specific launch configuration in `port/romfree/` and adapters in
  `port/os/`. Preserve the existing ownership boundaries.
- The abandoned top-level `fa18_port` and disconnected native gameplay modules
  have been deleted. Do not recreate a parallel port, restore its work plan or
  add C source/header files directly under `port/`.

## Integration and evidence

- Before substantial implementation, identify the active build, trace the real
  caller from the runner entry and name the dependency the change removes.
  Being linked into a library is insufficient evidence of runtime use.
- Implement connected changes in the playable runner. Exercise the changed path
  before accumulating further modules or extensive standalone proof work.
- Keep original source/disk behavior authoritative. Emulator/ROM runs, captures
  and historical proofs are reference evidence, not replacement game behavior.
- Report component correctness and runtime integration separately. Claim
  emulation independence only for the scope actually demonstrated; do not label
  ROM independence or static opcode helpers as CPU/chipset independence.
- The 614/699 readable-entry count measures known routine reconstruction only.
  It does not measure emulation-removal effort or whole-game completeness.
  Give estimates only with their scope, assumptions and supporting evidence.
- Use the active build's tests and affected gameplay comparisons. Avoid repeated
  expensive checks without a new change, failure or unresolved concern. Retired
  component tools can reference removed sources; inspect dependencies first.

## Preserve

- Do not modify `scripts/check_native_build.py`, `scripts/native_frame_count.py`
  or `port/native_data_allowlist.txt`.
- Preserve unrelated user changes, including `.vscode/`, and sealed recordings.
- Commit completed validated batches as the user previously requested.
