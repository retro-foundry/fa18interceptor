# Repository instructions

Read the framework `../../AGENTS.md`, `CURRENT_PORT_HANDOFF.md`, `PORT.md` and `port/README.md` before work.
The user's latest instructions override historical plans and proof notes.

## Active reverse-engineering framework

- User direction (2026-10-10): the playable native port moved to the standalone
  `fa18-interceptor-decomp` repository. All new native port work belongs there.
  This supersedes the historical native runtime directions in this repository.
- User direction (2026-10-11): remove the original CPU/machine comparison
  runners and remaining port code here. This supersedes the earlier direction
  to keep `fa18_recomp`, `fa18_romfree` and shared `port/game/` routines.
- Keep the original disk, sealed recordings, analysis, P-code/Ghidra work and
  Engine9000 capture/instrumentation tools. The retired port runtime, its
  Musashi dependency and its runtime test harness do not belong here.
- `port/` retains historical metadata only. Do not recreate a native runner,
  comparison runner or parallel port in this repository.
- `scripts/build_native.py` only forwards to the sibling standalone checkout;
  no standalone build requires this framework.

## Analysis and evidence

- Native implementation and gameplay validation belong in the standalone
  checkout. This repository supports original-byte analysis and recordings.
- Keep original source/disk behavior authoritative. Emulator/ROM runs, captures
  and historical proofs are reference evidence, not replacement game behavior.
- Report component correctness and runtime integration separately. Claim
  emulation independence only for the scope actually demonstrated; do not label
  ROM independence or static opcode helpers as CPU/chipset independence.
- The 614/699 readable-entry count measures known routine reconstruction only.
  It does not measure emulation-removal effort or whole-game completeness.
  Give estimates only with their scope, assumptions and supporting evidence.
- Use the retained analysis/capture tools and their affected tests. Historical
  port proofs require the pre-retirement checkout (`b04eaa35`); references in
  archived reports are provenance, not instructions to restore the runtime.

## Preserve

- Do not modify `scripts/check_native_build.py`, `scripts/native_frame_count.py`
  or `port/native_data_allowlist.txt`.
- Preserve unrelated user changes, including `.vscode/`, and sealed recordings.
- Commit completed validated batches as the user previously requested.

## Build artifact retention

- User direction (2026-10-07): keep workspace disk use bounded with
  `python -m amiga_decomp run prune_build_artifacts` and its 4 GiB build-cache budget.
  Do not disable retained cleanup hooks or accumulate raw passing RAM.
- Native mode/postflight/window comparisons use temporary capture storage by
  default and retain reports plus the failing case. Use `--keep-captures` only
  for a concrete unresolved investigation; remove passing copies afterwards.
- Manual diagnostic captures default to 512 MiB. Use bounded ranges; increasing
  `--capture-budget-mib` is an explicit diagnostic choice, not a new default.
- Retain reusable original RAM compressed as `.dat.gz`, with hashes/reports.
  Run the pruner after direct emulator capture commands as well. Preserve
  canonical `captures/`, media, active build dependencies and user settings.
