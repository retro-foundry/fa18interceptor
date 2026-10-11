# Static recompilation in the Amiga framework

The default process is **recompile first, qualify an executable baseline, then
decompile into readable source**. The translator preserves instruction behavior,
guest registers/addresses and machine interfaces. Semantic reconstruction then
replaces connected routines against that baseline and the original emulator.

The shared implementation was imported from
[amiga-recomp](https://github.com/retro-foundry/amiga-recomp) commit
`0f71e52134ba64fa74975b6bbe75b19830b225c4`. Its MIT notice is retained in
[runtime/LICENSE](../../runtime/LICENSE); [UPSTREAM.json](../../runtime/UPSTREAM.json)
records the imported files and original hashes. Integration changes live here.
The source checkout remains intact. The imported specification is
[AMIGA_RECOMP.md](../../AMIGA_RECOMP.md); current procedures/limits are below.

## Build shared tools

From the framework root, using CMake, Ninja and a C++20 compiler:

```console
cmake -S . -B build/recomp -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build/recomp --parallel 4
ctest --test-dir build/recomp --output-on-failure
python -m unittest discover -s tests
```

`ARECOMP_BUILD_TESTS` defaults ON. `ARECOMP_ENABLE_INTERPRETER` defaults ON for
explicit development discovery/differential testing. Qualification builds of
generated ports can disable it; observed fallback counts are still part of the
evidence. `ARECOMP_WITH_SDL=ON` builds the imported SDL2 display/input backend.
It fetches its pinned SDL dependency. The default build is headless.
Both the headless toolchain and optional SDL backend compile in this integration;
the SDL build check does not qualify visible gameplay or input timing.

`ARECOMP_WITH_MUSASHI` is an inherited option without an implemented external
oracle target in this revision. The in-tree interpreter shares semantic helpers
with generated code, so differential agreement establishes translation/control
flow consistency, not independent proof of every CPU formula. Retained manual
expected-value tests and original-emulator comparisons provide other evidence.

## Layout and ownership

| Location | Responsibility |
| --- | --- |
| [tools/arecomp/](../../tools/arecomp/) | TOML input, image loaders, decoder discovery, C++ generator and project emitter. |
| [include/amiga_recomp/](../../include/amiga_recomp/) | Guest CPU/memory/runtime ABI shared with generated code. |
| [runtime/](../../runtime/) | CPU helpers, dispatcher, hooks, memory and chipset/platform implementations. |
| [tools/asm68k.py](../../tools/asm68k.py) | Limited assembler for synthetic fixtures; production exact-source checks still use vasm. |
| [tools/converge.py](../../tools/converge.py) | Bounded run-log targets to reviewed manifest candidates. |
| [tests/recomp/](../../tests/recomp/) | C++ tests, differential generation and original synthetic fixtures. |
| `projects/<id>/recomp/` | Game TOML metadata, observed entry/target ownership and game-specific hooks. |
| `projects/<id>/build/recomp/` | Ignored generated C++ and its build/output; original media stays local. |

The Python launcher wrapper uses `recomp_manifest` in `project.json` or an
explicit TOML argument. Its working directory remains the selected project.
Generated CMake receives the shared framework root explicitly, independent of
the current directory. Native `arecomp` also embeds its configured source root
and accepts `--framework-root` after relocation.

## Start with a synthetic end-to-end proof

The following runs only the included synthetic program. Append `.exe` to the
two executable paths on Windows:

```console
build/recomp/bin/arecomp recompile tests/recomp/ports/copper-demo.toml -o build/recomp-demo
cmake -S build/recomp-demo -B build/recomp-demo/build -G Ninja -DARECOMP_ENABLE_INTERPRETER=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build/recomp-demo/build --parallel 4
build/recomp-demo/build/copper-demo tests/recomp/testroms/synthetic/copper_demo.bin build/recomp-demo/frame.ppm 1000000
```

The imported program runs 9,503 compiled blocks, renders two frames and reports
zero interpreter fallbacks in this integration check. That proves the bounded
synthetic pipeline; it does not qualify another game's graphics, sound or OS.
The 13 C++ test executables exercise CPU, memory, chipset/disk, self-modification,
manifests and generated differential runs. Shared Python checks cover project
isolation, safe output, synthetic assembly bytes and convergence bank coordinates.

## Current limits

The SDL backend exists and handles keyboard, mouse and gamepad input. Paula DMA
state/interrupts exist, but there is no host PCM mixer/filter parity or audible
playback qualification. Display DMA and line blits contain approximations;
`BusAware`/`ChipExact` timing modes do not provide complete bus contention in this
revision. AGA/68020 and a general OS shim are not implemented.

Flat/snapshot generation, Hunk analysis and explicit ADF load mappings exist.
Direct Hunk executable generation is rejected: the upstream project emitter
does not implement the corresponding runtime loader/relocations. Use a verified
relocated flat image for that route. Boot-block analysis
recognizes a bounded set of loader patterns; it does not solve arbitrary disk
protection or decrunching. Runtime chipset snapshots use the imported native
format, not Engine9000/UAE savestate bytes; no general conversion tool is shipped.
Dynamic overlays and code invalidation require measured versions and further
convergence. Runtime containers/diagnostics have not been qualified for zero
gameplay allocations; that requires a separate instrumented audit.

See [the recomp-first workflow](porting_game.md), [manifest format](manifest.md),
[architecture](architecture.md), [hooks](hooks.md), [loaded code](decrunching.md),
[OS boundaries](os_interaction.md) and [widescreen](widescreen.md). Game-specific
Vroom metadata/evidence lives in [its project](../../projects/vroom/README.md).
