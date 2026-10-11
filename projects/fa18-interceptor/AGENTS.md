# F/A-18 project instructions

Read [the framework instructions](../../AGENTS.md), [project README](README.md)
and [framework process manual](../../REVERSE_ENGINEERING.md) before analysis.
The user's latest directions override archived plans and proof notes.

- This project owns F/A-18 addresses, records, scenarios, interpretations,
  original media/recordings and game-specific observers. Shared Amiga methods,
  hardware support and analysis tools belong at framework level.
- The new shared recomp-first workflow applies to future game bring-up. F/A-18
  already has its native baseline; continue implementation in its submodule.
  The shared `runtime/` and `tools/arecomp` are general framework components,
  not restoration of this project's retired runtime/comparison runners.
- All new playable native implementation and gameplay qualification belongs in
  `native/`, the `fa18-interceptor-decomp` submodule. Commit/push it first, then
  update the framework pointer. `scripts/build_native.py` forwards to its
  standalone build script.
- The original CPU/machine comparison runners, port runtime and Musashi
  dependency were removed at the user's request on 2026-10-11. Do not recreate
  a parallel native runtime or comparison runner here. `port/` contains
  historical metadata only.
- Original bytes and observed behavior remain authoritative. Routine counts,
  structural P-code and historical milestones do not establish whole-game
  completeness or runtime independence. Keep the original evidence scope.
- Historical port reports require their recorded checkout, often `b04eaa35`.
  `PORT.md`, the port handoffs, `STATUS.md` and the `.history.md` files are
  archives; their commands are provenance rather than current build directions.
- The user waived the uninterrupted campaign requirement in favor of repeated
  individual mission tests. Residual internal-state cleanup is separately
  deferred outside the playable-completion goal.
- Do not modify `scripts/check_native_build.py`, `scripts/native_frame_count.py`
  or `port/native_data_allowlist.txt`. Preserve sealed recordings and unrelated
  user settings, including `.vscode/`.
- Follow the shared 4 GiB build-cache and 512 MiB diagnostic capture defaults;
  select this project explicitly when pruning. Retain reports, compressed
  reusable authority and failing cases; do not rewrite original configs.
