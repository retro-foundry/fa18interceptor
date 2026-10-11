# F/A-18 Interceptor reconstruction project

This project holds original-game evidence and source reconstruction for
*F/A-18 Interceptor* (Intellisoft / Electronic Arts, Amiga, 1988).
The reusable decompilation process lives in the
[framework manual](../../REVERSE_ENGINEERING.md), with shared tools in the
[tool catalog](../../tools/README.md). Start there for capture, mapping,
tracing, Ghidra, reconstruction, validation and evidence retention.

## Original authority and local setup

The recorded authority is `FA-18 Interceptor (1988)(Electronic Arts)[cr A-Ha].adf`
on a pinned Engine9000/UAE build with Kickstart 1.3, A500 PAL OCS and
512 KiB Chip plus 512 KiB Slow RAM. Confirm the actual capture's hashes and
configuration when reproducing a result; this label alone is not its identity.

[project.json](project.json) selects local configuration/media paths, the shared
Engine9000 installation, memory regions, loaded-Hunk mapping and game audio
observers. Supply the ignored `local/` assets and compatible tools yourself.
Preserved configs can contain former workspace paths; the launcher derives
runtime configs without rewriting sealed originals.

Run these commands from the framework root:

```console
python -m amiga_decomp --project fa18-interceptor info
python -m amiga_decomp --project fa18-interceptor run disasm_game C32D24 5
python -m amiga_decomp --project fa18-interceptor run engine9000_bridge --frames 5 --output build/startup-smoke
```

Relative tool arguments then resolve inside this project. The short startup
capture is a host diagnostic; it does not qualify a full flight or sealed GUI
presentation. Use the manual's recording/replay method for that evidence.

## Case evidence

| Location | F/A-18-specific material |
| --- | --- |
| [analysis/memory_map.md](analysis/memory_map.md) | Globals, records and observed ownership. |
| [analysis/hunk_inventory.json](analysis/hunk_inventory.json), [loaded mapping](analysis/hunk_runtime_resolved.json) | Executable segments and resolved placements. |
| [analysis/routines/](analysis/routines/) | Routine contracts, callers and interpretations. |
| [analysis/](analysis/) | Scenario reports, hardware/game findings and historical milestones. |
| [source_amiga/](source_amiga/README.md) | Exact observed slices and separate full-bank scaffolds. |
| [pcode/raw/](pcode/raw/) | Structural exports from bounded original captures. |
| `captures/` | Ignored original recordings, restore states and sealed input/config evidence. |
| [scripts/](scripts/), [tools/native/](tools/native/) | Game-aware observers and retained evidence checks. |

Coverage files are dated measurements, not a whole-game completion percentage.
Regenerate a result from its declared inputs before using it as a current count.
See [the case analysis plan](RE_COMPLETION_PLAN.md) for evidence entry points and
[source instructions](source_amiga/README.md) for this project's byte gate.

## Playable native checkout

The playable C game is the [native submodule](native/README.md), maintained in
[fa18-interceptor-decomp](https://github.com/retro-foundry/fa18-interceptor-decomp).
Continue native implementation and gameplay qualification in that checkout:

```console
git submodule update --init projects/fa18-interceptor/native
python -m amiga_decomp --project fa18-interceptor run build_native
```

The project wrapper forwards to the native checkout's standalone build script.
Its source layout and build dependencies are described in its own README.
Commit and push native changes there before updating the framework pointer.
The CPU/machine comparison runners and old port sources were retired from this
framework on 2026-10-11; do not recreate them here.

## Historical results

The [previous project README](README.history.md),
[previous analysis plan](RE_COMPLETION_PLAN.history.md),
[reconstruction history](analysis/reconstruction_history.md),
[PORT.md](PORT.md), [CURRENT_PORT_HANDOFF.md](CURRENT_PORT_HANDOFF.md),
[NEW_PORT_HANDOFF.md](NEW_PORT_HANDOFF.md) and [STATUS.md](STATUS.md) retain
historical findings and qualification scope. Their retired runtime commands
require the checkout named in the report (commonly `b04eaa35`); they are not
current framework setup instructions. Original relative paths in the two
history files still refer to this project root.

Current reusable methods and hardware findings live at framework level in the
[process manual](../../REVERSE_ENGINEERING.md), [Amiga reference](../../AMIGA.md)
and [Amiga findings](../../AMIGA_FINDINGS.md). Keep new game addresses, measured
results and scenario acceptance in this project; promote reusable methods there.
