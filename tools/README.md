# Shared tools

Run tools through `python -m amiga_decomp --project <id> run <tool>`. Relative
inputs and output paths are interpreted inside the selected project. Shared
Python import paths also reach child processes; project modules supply only
game-specific observers and analysis policy.

| Task | Framework tools |
| --- | --- |
| Reference execution | `engine9000_bridge`, `profile_window`, `capture_run_checkpoint` |
| Sealed recording/replay | `record_run`, `finalize_run`, `playback_run`, `probe_replay_state`, `render_run` |
| Replay editing | `append_replay_events`, `clip_replay`, `replace_replay_key`, `slice_playback` |
| RAM and breakpoints | `capture_chip_frames`, `sample_replay_memory`, `sample_runtime_memory_region`, `find_memory_transition`, `check_replay_breakpoint`, `check_replay_breakpoints`, `check_replay_watchpoint`, `diff_slow_ram --region <name>` |
| Instruction traces | `trace_from_breakpoint`, `trace_replay_breakpoint`, `trace_instruction_memory_writes`, `reverse_trace_call_stacks`, `generate_trace_call_graph` |
| Hunk mapping | `parse_hunk`, `map_loaded_segment`, `map_runtime_window`, `resolve_hunk_runtime`, `disasm_game` |
| Source evidence | `reconstruction_coverage`, `verify_reconstructions`, `generate_ghidra_function_coverage`, `list_unrepresented_functions`, `list_unreconstructed_observed`, `plan_captures` |
| Ghidra/P-code | `prepare_ghidra`, `import_ghidra`, `annotate_pcode_segments`, `summarize_trace_edges` |
| Chipset activity | `capture_dma_frame`, `inventory_blitter_jobs`, `inventory_dma_blits`, `inventory_dma_families`, `inventory_dma_writes` |
| Image inspection | `capture_keyframes --direct-core`, `render_keyframe_contact_sheet`, `measure_grayscale_viewport_regions` |
| Frame work/presentation | `report_frame_times` (documented phase CSV schema) |
| Capture retention | `prune_build_artifacts`, `tools/amiga/audio/capture_workspace.py` |
| OS contracts | `tools/amiga/capture_cold_entry.py`, `tools/amiga/capture_service_contract.py`, `tools/amiga/inspect_segment_list.py` |
| Audio | `tools/amiga/audio/build_original_audio_probe.py`, `tools/amiga/audio/check_original_audio_capture.py`, `tools/amiga/audio/check_original_audio_clock.py`, `tools/amiga/audio/check_original_audio_prefix.py`, `tools/amiga/audio/extract_original_pcm_averaging.py` |

Use `--help` for argument-based tools. A few small offline tools take positional
arguments and print their usage when invoked without them. `reconstruction_coverage`
is an importable helper used by the two source-evidence commands.

The shared Engine9000 submodule remains pinned to the observer-compatible fork
revision; its branch name records provenance. Initialize it with
`git submodule update --init tools/engine9000-src`. Installed binaries belong in
ignored `tools/engine9000/` or an explicit manifest installation path. The replay
host patch lives in `patches/engine9000-v062-replay-snapshot.patch` and is tied to
its recorded Engine9000 version. Replay builds and diagnostics stay in a project's
ignored `build/` directory.

The audio probe exports and stream magic retain their historical `fa18`/`FA18`
names so existing sealed recordings and observer manifests remain readable.
They describe Paula/UAE records, not game voice ownership. Legacy decoder defaults
use the original 512 KiB Chip RAM profile; pass another explicit Chip RAM size
when reading captures for a different machine. DMA grids and sample layouts must
still match the pinned observer ABI. Game-specific acceptance checkpoints,
voice-table readers and native gameplay fixtures stay in the game project.

Run shared checks with `python -m unittest discover -s tests`. The retained game
capture integration test runs with `python -m amiga_decomp --project <id> run
tools/native/test_original_audio_capture.py` in the F/A-18 project; its scenario
is game-specific. Source comparison also requires `vasmm68k_mot` on PATH and an
explicit `verify_reconstructions --snapshot <directory>`.

Optional manifest `dma_capture_ranges` contains half-open address pairs for narrow
DMA capture. `candidate_data_segments` associates Hunk indices with project
evidence paths; candidates never reduce the strict code denominator. Hunk
disassembly uses `executable`, `hunk_runtime_map` and
`disassembly_synthetic_base`. GUI recording additionally uses `toolchain`.
