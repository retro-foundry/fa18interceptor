# 1. Establish a project and original authority

[Process manual](../../REVERSE_ENGINEERING.md) · [Next: capture and replay](02-capture-and-replay.md)

The first deliverable is an identity for the bytes and machine being studied.
Without it, similar captures can be different games, patched disks, loaders,
ROMs or emulator configurations.

## Preserve inputs and their derivation

Keep supplied disk images, executable containers and ROMs unchanged in ignored
project-local storage. Record filename, byte count, SHA-256, provenance and the
command/tool revision used for each extraction. Ignore rules cover `local/`,
`captures/`, ADFs and ROMs; inspect Git status before committing.

An extracted file is a new authority with a derivation from its parent. A
decrunched or relocated image is another view, not a replacement for the disk.
Record input/output hashes and what transformation occurred. If an extractor
is absent, state that dependency; this framework does not provide a general
disk extraction or decrunching command.

```powershell
Get-FileHash -Algorithm SHA256 projects/your-game/local/media/game.adf
Get-FileHash -Algorithm SHA256 projects/your-game/local/system/kickstart.rom
```

Record the original media variant, including visible crack, trainer or version
markers. A deliberate change belongs to a separately identified variant;
comparisons against it cannot silently stand for the unchanged original.

## Describe the actual machine

Record CPU model, chipset, PAL/NTSC selection, RAM banks, ROM identity, mounted
volumes, disk order, write protection, floppy timing, cycle-exact CPU options,
blitter options and audio settings. Preserve actual UAE config bytes. State
observer core and replay host binary hashes as well as source commits; a source
revision alone does not identify a DLL.

Another game's config is an example, not a recommendation for this game.
A500 OCS observations do not apply automatically to every Amiga model. See
[AMIGA.md](../../AMIGA.md) and [Amiga findings](../../AMIGA_FINDINGS.md).

Record Python/Capstone versions, Ghidra release/language, assembler version and
host OS in a project toolchain report. GUI recording additionally expects JSON
at the manifest's `toolchain` path; `record_run` copies it and adds the recording
engine identity. The installation must supply the relevant binary and ABI.

## Create the project manifest

Create `projects/your-game/project.json`. The example region below demonstrates
the schema; replace it with your measured machine setup:

```json
{
  "schema": 1,
  "id": "your-game",
  "title": "Your game",
  "config": "local/game.uae",
  "engine_root": "../../tools/engine9000/e9k-debugger",
  "replay_engine": "build/engine9000-replay/e9k-debugger.exe",
  "system_dir": "local/system",
  "memory": [{"name": "chip", "start": 0, "size": 524288}]
}
```

JSON addresses/sizes are decimal numbers. IDs use lowercase letters, digits
and hyphens. Memory names use lowercase letters, digits and underscores;
regions require distinct names, positive sizes, no overlaps and 24-bit guest
address bounds. Host pointers are unrelated to these addresses.

Paths are relative to the manifest directory or explicit absolute paths.
`engine_root` contains `system/ami9000.dll`; `replay_engine` names an executable,
not a directory. Manifest loading checks schema and memory intervals, not the
existence of every media file or optional tool.

| Additional setting | Consumer and meaning |
| --- | --- |
| `engine_source` | Source for isolated audio observers; shared checkout is `../../tools/engine9000-src`. |
| `toolchain` | Existing JSON report copied by GUI recording. |
| `executable` | Extracted Hunk executable for `disasm_game`. |
| `hunk_runtime_map` | Resolved placement report for `disasm_game`. |
| `disassembly_synthetic_base` | Starting coordinate for labelled unresolved-Hunk listings. |
| `python_paths` | Project import directories; defaults to `["scripts"]`. Shared paths are added separately. |
| `former_roots` | Old root prefixes eligible for derived UAE path relocation. |
| `dma_capture_ranges` | Half-open address pairs for narrow DMA capture. |
| `candidate_data_segments` | Hunk-index strings mapped to evidence paths; working classifications only. |
| `audio_observers` | Modules for `events`, `dma`, `samples`; voice ownership is a project hook. |
| `artifact_policy` | Project build roots and retention exceptions; [chapter 8](08-evidence-and-maintenance.md). |
| `native_checkout` | Used by a project's build forwarding script; not needed for generic analysis. |

```console
python -m amiga_decomp --project your-game info
python -m amiga_decomp --project your-game run engine9000_bridge --help
```

There is no implicit default game. `AMIGA_DECOMP_PROJECT` can also name an ID
or absolute manifest path. Selection from a project working directory can use
an ancestor `project.json`; published commands should prefer `--project`.
`AMIGA_DECOMP_ENGINE_ROOT` selects an alternate observer installation, not a
project. Record it when using an instrumented core.

## Place material by ownership

```text
amiga_decomp/                    shared selection and format/library helpers
scripts/                        shared analysis/capture commands
tools/amiga/                    OS and hardware observer tools
tools/engine9000-src/            pinned reusable source submodule
patches/                        version-specific reusable host patches
docs/decompilation/             this process
projects/your-game/
  project.json                  machine/tools and game-specific policy
  README.md                     authority, scope, evidence and next work
  scripts/                      addresses and game-only analysis
  analysis/                     contracts, inventories and reports
  pcode/raw/                    retained structural exports
  source_amiga/                  exact and symbolic source
  local/                        ignored media, ROM and private installations
  captures/                     ignored immutable recordings
  build/                        ignored bounded diagnostics
  runtime_project/              ignored local Ghidra projects
  native/                       optional implementation submodule
```

Directories beyond the manifest are conventions, not generated scaffolding.
Some coverage tools expect conventional `analysis/`, `pcode/raw/` and
`source_amiga/observed/` paths. Document departures and use explicit arguments
where supported.

## Prove setup before analysis

```console
python -m amiga_decomp --project your-game run engine9000_bridge --frames 5 --output build/startup-smoke
python -m amiga_decomp --project your-game run prune_build_artifacts --dry-run
```

Inspect `snapshot.json`, `screen.png`, configured memory dumps and `state.bin`.
The report contains core/config/state identities, memory sizes/hashes,
registers, cycles, core options and video/audio hashes. This proves the host
executed that interval; it does not prove loading or entry to gameplay. Its
screenshot is diagnostic rather than a sealed GUI replay image.

Acceptance: original inputs, derived views, machine and tools are identifiable;
bounded capture produces interpretable evidence without another game's files.
