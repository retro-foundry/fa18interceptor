# Current playable port handoff

Updated 2026-10-05 following the user's source-ownership correction and request
to remove the abandoned top-level port. This file supersedes earlier resume
instructions; previous handoffs and removed source are preserved in git history.

## Work scope

Work on `port/game/`, `port/game/glue/` and the actual `port/recomp/` runtime.
The goal is the playable game without Kickstart or emulation. Do not build a
parallel gameplay implementation in the top-level `port/` directory.
A component proof counts as runtime progress only when the active runner uses it.

The abandoned `fa18_port` source/build and disconnected native replacement
components were removed. Forty-eight top-level C source/header files remain:
disk/Hunk loaders, shared map-packet code, its projection type header and three
command type headers. They are dependencies of the active runners. See
`port/README.md` for ownership and build commands.

Historical `analysis/routines/native_*.md` notes without the `native_c_` prefix
and standalone `tools/recomp/check_native_*` component tools may reference the
removed implementation. They are historical evidence, not a resume plan or
runnable acceptance gates for the active game. Inspect their source dependencies
before using them. The disconnected uncommitted shared-origin batch was dropped.

Removal validation: both active runners build with MSVC Release and GNU;
all eleven active CTests pass. The GNU and MSVC ADF-only launcher checks pass
185 Hunk/8,441 relocation construction, both CPU modes, splash, credits and
keyboard-to-demo, with zero ROM accesses or unsupported services. Hash checks
confirm all 697 tracked active-tree files, all 48 retained top-level sources
and the three user-owned guard/allowlist files are unchanged.

## Active runner state

`fa18_romfree` loads the original game/resources from an ADF without Kickstart
or a savestate, using reusable host compatibility/loading in `port/amiga/`,
Interceptor configuration in `port/romfree/`, and guest adapters in `port/os/`.
It still uses Musashi, guest RAM and the chipset model. Removing the abandoned
sources does not establish emulation independence.

The last active function milestone (`05fa7453`) statically recompiles 85 deferred
entries in `port/recomp/generated/recomp_static_deferred.c`. They retain explicit
`STATIC_RECOMP`/`TODO(decompile)` markers and per-entry debt in
`recomp_deferred.json`. The inventory includes 539 readable translated entries
and 75 additional readable source-only callable entries. Static compilation
still depends on shared CPU/machine state and is not readable decompilation.
See `analysis/routines/static_recomp_deferred.md` and its checkpoint for the
existing validation scope and original call-graph limitations.

Existing ROM-free verification covers clean startup/demo, menu and active
mission entry/restart, and original configuration save/load. Full mission
outcomes/progression, carrier success and active-flight teardown remain open.
See `analysis/routines/romfree_machine_startup.md`,
`analysis/routines/romfree_exact_followup.md` and
`analysis/routines/romfree_performance_followup.md` for evidence and limits.

## Next work and validation

Trace the real startup/frame graph through `port/game/` and the active runtime;
remove CPU/guest-bus/chipset dependencies there in connected, source-backed
changes. Reuse the retained shared code where it is actually called. Do not
resume the removed standalone bootstrap/pose/control-owner plan.

Build the MSVC runners with `cmake -S port/recomp -B build/recomp-cmake` and
`cmake --build build/recomp-cmake --config Release`. Headless GNU builds use
`python scripts/build_recomp.py` and `python scripts/build_recomp.py --romfree`.
Run `ctest --test-dir build/recomp-cmake -C Release --output-on-failure`.
For changed gameplay use `scripts/recomp_ports_check.sh` and affected live
RGB/RAM comparisons with `scripts/recomp_live_check.sh`, plus targeted original
routine proofs. Keep known combined readable-C timing debt explicit.

Original disk bytes and source behavior remain the authority; emulator/ROM
runs and sealed captures are validation evidence. Do not alter
`scripts/check_native_build.py`, `scripts/native_frame_count.py` or
`port/native_data_allowlist.txt`. The former `fa18_port` guard is retained as
user-owned historical tooling; its removed target is not an active build.
Preserve unrelated `.vscode/` content. Commit completed validated batches as
previously requested. The emulation-independence goal remains unfinished.
