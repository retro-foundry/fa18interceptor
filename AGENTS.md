# Repository instructions

Read `CURRENT_PORT_HANDOFF.md`, `PORT.md` and `port/README.md` before work.
The user's latest instructions override historical plans and proof notes.

## Active reconstruction framework

- User direction (2026-10-10): the playable native port moved to the standalone
  `fa18-interceptor-decomp` repository. All new native port work belongs there.
  This supersedes the historical native runtime directions in this repository.
- Keep original CPU/machine comparison runners here: `fa18_recomp` and
  `fa18_romfree`, built by `port/recomp/CMakeLists.txt` and
  `scripts/build_recomp.py`.
- Shared `port/game/` routines remain here for reconstruction/comparison use;
  native entry/composition no longer belong in this repository. Do not recreate
  a native runner or parallel port here.
- The read-only flight trace reused by comparison runners now belongs to
  `port/recomp/flight_trace.c`; reusable host facilities remain in `port/amiga/`.
- `scripts/build_native.py` only forwards to the sibling standalone checkout;
  no standalone build requires this framework.

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

## Build artifact retention

- User direction (2026-10-07): keep workspace disk use bounded. Builds and
  native CTest runs invoke `scripts/prune_build_artifacts.py` with a 4 GiB
  build-cache budget. Do not disable those hooks or accumulate raw passing RAM.
- Native mode/postflight/window comparisons use temporary capture storage by
  default and retain reports plus the failing case. Use `--keep-captures` only
  for a concrete unresolved investigation; remove passing copies afterwards.
- Manual diagnostic captures default to 512 MiB. Use bounded ranges; increasing
  `--capture-budget-mib` is an explicit diagnostic choice, not a new default.
- Retain reusable original RAM compressed as `.dat.gz`, with hashes/reports.
  Run the pruner after direct emulator capture commands as well. Preserve
  canonical `captures/`, media, active build dependencies and user settings.
