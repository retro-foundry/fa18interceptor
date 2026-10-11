# Recompile first, then decompile

[Recompiler index](README.md) · [Full process manual](../../REVERSE_ENGINEERING.md)

Start future game projects by obtaining executable behavior through literal
static recompilation. Qualify that baseline against original execution before
replacing it with readable routines. Initial image/caller investigations still
need evidence; comprehensive naming and semantic recovery can follow a running
baseline rather than blocking the first executable.

## 1. Establish authority and a usable image

Follow the process chapters on [authority](../decompilation/01-project-and-authority.md),
[recordings](../decompilation/02-capture-and-replay.md) and
[loaded bytes](../decompilation/03-memory-and-hunks.md). Preserve hashes,
machine configuration, code versions and initial CPU state.

Choose a flat image, explicit disk-load mapping or captured resident image.
Hunk analysis can establish placements, but direct Hunk generation is currently
rejected because the generated runtime loader is incomplete; first obtain a
verified relocated flat view. Bound executable ranges; do not decode compressed
payloads or guessed table entries as instructions. See [decrunching](decrunching.md).
Snapshot bring-up requires CPU, RAM and machine state. A native `ARCS` chipset
state is not a UAE savestate; converting between them is unfinished framework
work. Captured RAM without matching hardware state is not a qualified resume.

## 2. Write the game's recompilation metadata

Place TOML in `projects/<id>/recomp/game.toml`. Add
`"recomp_manifest": "recomp/game.toml"` to the existing `project.json`.
The JSON selects framework capture/project policy; the TOML selects translated
input/CPU/code/machine metadata. They must describe the same intended machine,
but one is not automatically generated from the other.

See [manifest.md](manifest.md). Record entry/register/stack state, explicit
regions/ranges, known interrupt entries and measured indirect targets. TOML
asset paths resolve beside that TOML. Hunk analysis has its own placement rules;
verify them rather than assuming they equal the original loader's addresses.

From the framework root:

```console
python -m amiga_decomp --project your-game run recompile analyze
python -m amiga_decomp --project your-game run recompile disasm --from ENTRY_PC --count 40
python -m amiga_decomp --project your-game run recompile recompile --output build/recomp
```

An explicit TOML positional argument overrides `recomp_manifest`. The wrapper
uses `build/recomp/bin/arecomp` from the framework (with `.exe` on Windows), or
`--tool PATH`. It rejects output outside this project's build roots. Native
`arecomp` can also run directly with explicit input/output/framework paths.

`build_report.txt` describes translated blocks/instructions, unresolved sites
and unsupported instructions. Successful generation is an executable candidate,
not semantic understanding or original-game acceptance. Inspect diagnostics.

## 3. Build and run a bounded development baseline

From the game project directory:

```console
cmake -S build/recomp -B build/recomp/build -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build/recomp/build --parallel 4
build/recomp/build/GAME_ID local/image.bin build/recomp/frame.ppm GUEST_CYCLE_BOUND
```

Replace `GAME_ID` with the TOML ID and append `.exe` on Windows. The first
argument is the memory/game image. Attached disk media comes from the manifest,
not that image argument. The third argument is a guest-cycle budget; retain
the reported stop PC, cycles, blocks, interrupts and fallbacks. A process can
exit successfully after a bounded guest fault/stop, so inspect its logs/state.

Development interpretation is explicit discovery support. Record its usage;
do not call a mixed run fully recompiled. A snapshot-start run also has the
snapshot's startup boundary: it does not establish independent cold launch.

## 4. Converge from observed execution

Run real input/scenarios and retain stderr, missing block PCs, loaded bytes and
code-overwrite events. Before adding a target, prove the code version and its
guest-to-bank offset. Recover newly loaded overlays at their actual loaded
time; translating zeros or stale bytes cannot close that path.

```console
python -m amiga_decomp --project your-game run tools/converge.py recomp/game.toml build/recomp/run.log --snapshot local/image.bin --snapshot-base BANK_BASE --dry-run
python -m amiga_decomp --project your-game run tools/converge.py recomp/game.toml build/recomp/run.log --snapshot local/image.bin --snapshot-base BANK_BASE --output recomp/candidates.toml
```

The helper collects unresolved-target log entries and rejects windows that are
empty or outside the supplied bank. Nonzero bytes remain candidates, not proof
of valid code or runtime ownership. Review target ranges and original evidence
before applying them. A derived manifest stays beside its input to preserve
relative asset paths; originals need not be edited. `--snapshot-base` defaults
to zero and must reflect the actual dumped bank.

Regenerate from reviewed metadata, rebuild and repeat the scenario. Self-modifying
code retires overlapping compiled blocks; subsequent interpretation does not
establish static completion. Keep versions and dynamic dependencies visible.
No missing entries in one log only establishes that log's observation scope.

## 5. Qualify the recompilation baseline

Compare compiled and forced-interpreted runs using retained digest boundaries
(`ARECOMP_DIGEST=N`, `ARECOMP_INTERPRET=1` in development builds). They share
semantic helpers, so agreement narrows translator/control-flow bugs; compare
original-emulator state/output as well. Preserve the first differing boundary,
input/time coordinates and complete scenario scope.

Build the generated executable with interpretation removed:

```console
cmake -S build/recomp -B build/recomp/qualified -G Ninja -DARECOMP_ENABLE_INTERPRETER=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build/recomp/qualified --parallel 4
```

Exercise the agreed scenarios, inspect faults/unknown targets and verify zero
fallbacks. A synthetic pass does not qualify a real title. Record separate
startup, menus, gameplay, transitions, saving and audio coverage as applicable.
Use the [validation policy](../decompilation/06-validation-and-divergence.md).
Qualified static CPU execution still depends on the machine/chipset runtime.

SDL window/input builds use `-DARECOMP_WITH_SDL=ON`. The imported backend
supports keyboard, mouse and gamepad. Its timing/audio correctness and visible
presentation need their own original comparisons; opening a window is not parity.

## 6. Decompile against the working baseline

Keep the generated baseline and reports reproducible. Select a related subsystem
and trace its original producer/state/consumer behavior. Use the existing
[tracing/Ghidra](../decompilation/04-tracing-and-ghidra.md) and
[source reconstruction](../decompilation/05-source-reconstruction.md) chapters
for names, contracts, exact assembly and arithmetic/calling-context proof.

Declare [hook](hooks.md) boundaries in TOML, implement a native replacement and
show that the actual caller reaches it. Hooks can qualify an individual replacement
while the remainder stays translated. Preserve caller-live registers, flags,
stack/memory effects, timing and ordered machine accesses. Compare original,
translated and replaced paths under the same acceptance policy.

Move proven behavior into readable game modules and eventually replace guest
state/adapters where callers permit it. Generated C++ is a behavioral baseline;
readable reconstruction and removal of CPU/chipset dependencies are separate
deliverables. Track both, plus semantic meaning, without a combined percentage.

Game implementation belongs to its project/native checkout. For F/A-18 that is
the existing submodule; integrating this shared recompiler does not revive its
retired comparison runtime. Enhancements such as [widescreen](widescreen.md)
come after the agreed original-behavior baseline.
