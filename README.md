# Amiga decompilation framework

Tools for static recompilation, original-execution capture, instruction-byte
validation, Ghidra analysis and readable source reconstruction.
Game-specific work lives in `projects/<id>/`; shared tools do not select a game
implicitly. The existing case study is [F/A-18 Interceptor](projects/fa18-interceptor/README.md).

The default workflow is **recompile first, then decompile**: get an executable
baseline, qualify its observed behavior, then replace translated routines with
meaning-backed source. Start with the [recomp-first guide](docs/recomp/porting_game.md)
and [build/integration instructions](docs/recomp/README.md).

The [decompilation process manual](REVERSE_ENGINEERING.md) documents the full
workflow in eight chapters: authority/setup, recording/replay, loaded memory
and Hunks, tracing/Ghidra, source reconstruction, validation, hardware/OS/audio,
and evidence maintenance. Each stage specifies inputs, commands, expected
artifacts, acceptance evidence and the limits of the current tools.

Python 3.11 or newer is required. Install `capstone` for 68000 instruction tracing
and disassembly, `numpy` for DMA/raster analysis, and `Pillow` for image tools.
The Engine9000 ctypes host currently uses the Windows DLL ABI;
offline analysis and the Ghidra launcher also work on Linux with matching tools.
Supply your own game media, Kickstart ROM and compatible Engine9000 installation.
Original media, recordings and local build caches are ignored by Git.

```console
python -m amiga_decomp projects
python -m amiga_decomp --project fa18-interceptor info
python -m amiga_decomp --project fa18-interceptor run disasm_game C32D24 5
python -m amiga_decomp --project fa18-interceptor run engine9000_bridge --frames 5 --output build/startup
python -m amiga_decomp --project fa18-interceptor run prepare_ghidra build/trace
python -m amiga_decomp --project fa18-interceptor run import_ghidra build/trace --name ObservedTrace --headless /path/to/ghidra/support/analyzeHeadless
python -m amiga_decomp --project fa18-interceptor run prune_build_artifacts --dry-run
python -m unittest discover -s tests
```

`run` accepts a shared script name, a project script name or a relative Python
tool path such as `tools/amiga/capture_service_contract.py`. Its working directory
and relative arguments are the selected project root. Project selection and
Python import paths propagate to child processes. Alternatively set
`AMIGA_DECOMP_PROJECT` to a project ID or a manifest's absolute path.
Use `AMIGA_DECOMP_ENGINE_ROOT` to explicitly select an observer core.

Create another game directory with a `project.json` containing the following
fields. Paths are relative to that directory; absolute paths can refer to an
externally installed emulator or ROM directory.

```json
{
  "schema": 1,
  "id": "your-game",
  "title": "Your game",
  "config": "local/game.uae",
  "engine_root": "local/engine",
  "replay_engine": "local/engine/e9k-debugger.exe",
  "system_dir": "local/system",
  "memory": [{"name": "chip", "start": 0, "size": 524288}]
}
```

Configure memory regions from the actual machine setup. Keep its analyses,
source reconstruction and project scripts in that project. Shared Engine9000
source and patches live under `tools/` and `patches/`; select installations
through the manifest.
Optional `python_paths` exposes project analysis modules. `former_roots` lets
the host derive relocated UAE path values without changing sealed configs.
Game audio observers and artifact retention exceptions are optional project
policy, as demonstrated by the existing manifest.

Sealed GUI replay requires an Engine9000 executable with the recording's boot
restore protocol, selected by `replay_engine` or the tool's `--engine` option.
When relocating a recorded config, the tools derive matching snapshot identity
headers in copied saves and retain all serialized core bytes. The original
recordings are never rewritten. The framework retains its replay-host patch in `patches/`;
its ignored `build/engine9000-replay/` executable must be supplied locally.

The shared layer includes a 68000-to-C++ translator, guest machine runtime,
SDL display/input support and synthetic/differential tests imported from
`amiga-recomp`. [Current capabilities and limits](docs/recomp/README.md) distinguish
implemented features from the inherited [design specification](AMIGA_RECOMP.md).
Game TOML metadata stays under `projects/<id>/recomp/`; generated translations
stay in ignored project build storage. Optional `recomp_manifest` selects the
TOML used by `python -m amiga_decomp --project <id> run recompile`.

The shared layer also includes bounded capture, replay/checkpoint rendering, input
editing, instruction-write tracing, snapshot validation, DOS segment inspection,
Ghidra import/P-code export and cache pruning. Ghidra imports only validated
observed instruction starts; structural functions do not establish behavior.
See [REVERSE_ENGINEERING.md](REVERSE_ENGINEERING.md) for the reusable workflow,
[AMIGA.md](AMIGA.md) and [AMIGA_FINDINGS.md](AMIGA_FINDINGS.md) for hardware guidance,
and [the shared tool catalog](tools/README.md) for analysis tools. Historical game commands are
relative to their project directory; their archived runtime proofs require the
commit recorded in those notes. The playable native port is a separate repository.

The F/A-18 project's playable port is checked out as the
[`projects/fa18-interceptor/native`](projects/fa18-interceptor/native/README.md)
submodule. Initialize and build it from the framework root:

```console
git submodule update --init projects/fa18-interceptor/native
python -m amiga_decomp --project fa18-interceptor run build_native
```

Commit and push changes within the submodule first, then commit its updated
pointer in this framework. The native repository also builds on its own.
