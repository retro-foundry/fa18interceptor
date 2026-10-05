# Current playable port handoff

Updated 2026-10-05 following the user's source-ownership correction, cleanup and
explicit instruction to prevent another costly detour. Read this handoff and
`AGENTS.md` before continuing. This file supersedes earlier resume instructions;
previous handoffs and removed source are preserved in git history.

## What went wrong and must not recur

About fourteen hours of elapsed work on 2026-10-05 expanded a separate gameplay
implementation in the abandoned top-level `port/` tree. Its standalone proofs
were reported as progress toward emulation independence without establishing
that the playable runner called it. This wasted substantial user time and money.
Some shared host components were integrated and remain useful; this does not
excuse the disconnected gameplay work or its misleading progress reports.

The user clarified that `port/game/` is the active port and requested deletion
of the abandoned sources. Commit `0855f5d6` removed 759 tracked source/header
files and the old build definition. Commit `e70a7014` moved the 48 retained
dependencies into `port/game/` and updated builds, includes and active tools.
Do not recover the deleted implementation as a parallel port or resume its
bootstrap/pose/control-owner plan. Git history is reference evidence only.

The unsupported three-to-six-week estimate was withdrawn. The later "0%" answer described
the absence of a verified complete emulation-free gameplay path, not the amount
of work completed; it was misleading as an overall progress percentage. Neither
number is an accepted project estimate. The measured 614/699 (87.84%) figure is
only readable routine reconstruction within the known inventory. It is neither
whole-game discovery coverage nor emulation-independence completion.

## Required checks before substantial implementation

1. Verify the candidate implementation is compiled by
   `port/recomp/CMakeLists.txt` or `scripts/build_recomp.py`, and trace its caller
   from the actual runner entry. A library link alone does not establish use.
2. Identify the specific CPU, guest-bus, generated-code or chipset dependency
   the batch will remove, and the real startup/menu/flight scenario exercising
   that change. Record the active caller and planned integration in this handoff.
3. Make the first batch a small connected change exercised in the playable
   runner. Do not accumulate more disconnected owner modules and expensive
   component proofs while startup/frame integration remains absent.
4. Validate the changed runner path and the relevant original behavior. Report
   separately what is compiled, what is actually exercised, which dependency
   was removed, and which dependencies remain. Standalone proofs alone do not
   complete a runtime milestone.
5. Reuse existing source and evidence. Run checks appropriate to the affected
   behavior; repeat or broaden them only for new changes, failures or unresolved
   concerns. The retired 249-test suite is not the active runner's acceptance
   suite. Do not spend another long sequence validating the wrong target.

Report useful progress regularly during execution. If evidence shows a batch
is disconnected or targets the wrong build, correct the implementation direction
before continuing it. Do not substitute routine counts, passing test counts,
commit counts or elapsed time for a measured runtime result. Give any future
time/effort estimate with its scope, assumptions and measured basis; do not
invent an overall percentage or extrapolate a deadline from function counts.

## Work scope

Work on `port/game/`, `port/game/glue/` and the actual `port/recomp/` runtime.
The goal is the playable game without Kickstart or emulation. Do not build a
parallel gameplay implementation in the top-level `port/` directory.
A component proof counts as runtime progress only when the active runner uses it.

The abandoned `fa18_port` source/build and disconnected native replacement
components were removed. The 48 retained shared source/header files have now
been moved into `port/game/`: disk/Hunk loaders, map-packet code, its projection
type header and three command type headers. No top-level `port/*.c` or
`port/*.h` files remain. Build paths and active includes use the new locations.
The CMake runtime now compiles the map core with the other game sources; the
Amiga loader library compiles disk/Hunk once. The GNU source glob includes all
of these without duplicate explicit source lists. See `port/README.md`.

Historical `analysis/routines/native_*.md` notes without the `native_c_` prefix
and standalone `tools/recomp/check_native_*` component tools may reference the
removed implementation. They are historical evidence, not a resume plan or
runnable acceptance gates for the active game. Inspect their source dependencies
before using them. The disconnected uncommitted shared-origin batch was dropped.

Removal validation: both active runners build with MSVC Release and GNU;
all eleven active CTests pass. The GNU and MSVC ADF-only launcher checks pass
185 Hunk/8,441 relocation construction, both CPU modes, splash, credits and
keyboard-to-demo, with zero ROM accesses or unsupported services. Hash checks
before relocation confirmed all 697 tracked active-tree files, all 48 retained
sources and the three user-owned guard/allowlist files were unchanged.

Relocation validation also passes both MSVC/GNU runner builds, all eleven active
CTests and both ADF-only launcher checks. All 48 moved implementations are
unchanged apart from four loader include paths; the GNU manifest has 450 unique
existing source files. Loader/OFS checks match all 185 hunks, 8,441 relocations
and 22 resource hashes. The retained RGB4 and viewport comparison tools now use
the moved loaders and exclude checkpoint dependencies on deleted components:
16,384 RGB4 and 32,768 viewport construction/merge calls match their frozen host
implementations. These shared host components are used by the active runner.

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

Before sustained implementation, document the real startup/frame call graph
from `port/recomp/recomp_main.c` through machine execution and the registered
`port/game/glue/ports.c` owners. Inspect `port/game/memory.h`, hardware access
and child hooks for the remaining CPU/bus/chipset dependencies. Distinguish the
44 deferred original ADF entries from the 41 reference wrappers; only the actual
required game paths define the cutover work. The known inventory does not prove
the complete original callback/call graph has been found.

Use that evidence to choose the first small dependency-removal batch in the
active runner. The intended milestone is clean launch into active flight with
Musashi absent from that executable; it is not delivered yet. Chipset removal
and complete game-mode acceptance remain additional requirements. Keep each
change connected to the actual game/runtime call graph and record its measured
result before extending the batch. Reuse retained shared code where called.

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
