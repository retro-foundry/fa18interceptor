# Reverse-engineering handoff

Updated: 2026-09-24. Read `README.md`, `STATUS.md`, then this file.

## Authority and working state

- Branch: `coverage-accounting`; remote: `github-retrofoundry`.
- All material analysis commits are pushed. Check `git status --short` before
  editing; build outputs are ignored.
- Original disk and sealed captures are authorities. Do not alter a sealed
  capture. Runtime authority is the pinned Engine9000 v0.62-alpha Amiga core.
- Run the byte gate after source changes:

  ```powershell
  python scripts/verify_reconstructions.py
  ```

  Last checked result: 589 source slices / 46,488 bytes match the authority
  snapshot.

## Current map result

The world path has two separate layers:

```text
immutable terrain template pair -> mutable placement (X, 0, Z) -> descriptor/control
                                                       -> local 3D component triples -> renderer
```

- The sampled placement/cache layer is flat (X/0/Z), but terrain/landmark
  component geometry is genuinely 3D. Do not collapse these claims.
- `$C3B720 -> $C3B6B0` is a traced reusable five-local-triple component with
  polygon topology. Run037 directly joins a flat template placement through
  `$C3B6A6/$C3B6AE` to an observed batch of four `$C3B6B0` filled faces and
  one line. See `analysis/data/run037_c3b6a6_placement_to_face_context.md`.
- `$C3B588 -> $C3B4FE/$C3B50A` is the user-identified green mountain behind
  the Golden Gate: five local triples and observed pyramid/wedge topology.
  `$C3B4F8` is only a six-byte adjacent control prefix, not a mountain-instance
  pointer. Run037 proves it is read as descriptor `$C22708`'s live `+8` field,
  but eleven later stores overwrite `$C45A36` before the next walker enters at
  `$C3B73E`; neither `$C3B4F8` nor `$C3B4FE` executes in that continuation.
  See `analysis/data/c3b4f8_c3b4fe_control_boundary.md`.
- `$C363EC` has an immutable four-triple input and one source-bounded observed
  five-vertex filled face. Its static face-controller boundary remains unknown.
  Inspect `analysis/plots/run034_c363ec_interval_face_sheet.png` and
  `analysis/data/c363ec_c3628a_close_range_line_component.md`.
- The `M` map is renderer-produced coastline geometry, not a stored bitmap.
  Its raw source pairs are planar renderer inputs; do not call them global
  coordinates or terrain elevation.

## Run042 (latest user capture)

`captures/run042` is sealed: 215 events, last input frame 10,443; `M` is
pressed at frame 9,385. It supplies a visibly distinct map location.

Reproducible artifacts:

```powershell
python scripts/engine9000_bridge.py --restore captures/run042/initial_state.bin `
  --config captures/run042/config.uae --playback captures/run042/playback.e9k `
  --frames 9392 --output build/run042_post_m_checkpoint

python scripts/engine9000_bridge.py --restore build/run042_post_m_checkpoint/state.bin `
  --config captures/run042/config.uae --frames 24 --trace-frames 24 `
  --output build/run042_m_map_transition_trace

python scripts/inventory_map_polygon_static_packets.py `
  --trace build/run042_m_map_transition_trace/trace.jsonl `
  --slow build/run042_m_map_transition_trace/slow.bin `
  --output analysis/data/run042_m_map_polygon_static_packets.json
```

Result: 57 direct `$C2AF00` packet entries, 110 completed `$C2AFE2` batches,
749 exact segment-68 pairs, and 25 distinct headers. Compared with the run037
stable map sample, run042 newly observes `$C43D1A`, `$C43FD0`, `$C4404C`,
`$C440BC`, `$C44160`, `$C4417A`, `$C4420C`, and `$C4424A`.

This proves location-sensitive static M-map packet selection. It does **not**
prove absolute map coordinates, physical extent, terrain mesh ownership, or
physical-distance LOD. Full report:
`analysis/data/run042_m_map_packet_comparison.md`.

The five-run cumulative source-pair coverage is 519 exact pairs:
`analysis/plots/m_map_packet_source_coverage.png`.

The eight run042-only direct headers now join to 45 nonnegative cells in the
byte-decoded 32x32 wide directory. Each has a complete inline/alternate
count-threshold stream decode; the bounded run042 entries all take inline.
This is selector/grammar provenance only, not a world-position or terrain
mapping result. Report and reproducible join:

```powershell
python scripts/compare_run042_map_packet_directory.py `
  --run042 analysis/data/run042_m_map_polygon_static_packets.json `
  --run037 analysis/data/run037_m_map_stable_polygon_static_packets.json `
  --wide-directory analysis/data/wide_m_map_packet_directory.json `
  --static-streams analysis/data/static_m_map_packet_streams.json `
  --output analysis/data/run042_m_map_header_directory_grammar.json
```

## LOD status

- M-map renderer has a real projection-depth-driven inline/alternate packet
  variant mechanism. It is LOD-style renderer detail, not established
  physical-flight-distance terrain LOD.
- Run041 proves landmark-associated visual-detail selection at stable Golden
  Gate bearing. Its four checkpoints now bracket the mixed source-family change
  from 72 to 674 red pixels, but no selector threshold is tied to measured
  physical distance; see `analysis/data/run041_stable_bearing_transition_bracket.md`.
- Do not call source-count changes, face counts, culling, or line/polygon
  coexistence an LOD scheme without a controlled selector/distance trace.

## Best next work

1. Capture or isolate a map continuation where `$C3B4F8` remains in `$C45A36`
   through a `$C1F6F8` entry. The existing run037 no-input continuation proves
   the target's store but shows it overwritten before the walker, so it is not
   a `$C3B4FE` parser or mountain-component trace. The repeated run003/run035/
   run037 audit at `analysis/data/c3b4f8_map_store_walker_audit.md` rules out
   treating any current bounded sample as that transition. The debugger-only
   parser probe confirms `$C1F6F8` can consume the prefix, but must never be
   cited as an original scenario transition.
2. For physical terrain LOD, keep one landmark/camera bearing stable while
   varying range and trace the selector plus immutable family. Existing
   `analysis/data/golden_gate_lod_capture_protocol.md` is the protocol.

## Recent commits

- `1c835de Join run042 map headers to directory grammar`
- `de05086 Add run042 map packet source coverage`
- `3648c51 Compare run042 map packet selection`
- `d888b05 Separate mountain face family from control prefix`
- `24ea7f7 Bound C3B4F8 placement control prefix`
- `15c8df0 Complete observed placement face batch`
- `0f895cd Join flat map placement to 3D face context`
- `1dd3249 Visualize source-bounded terrain face`

Keep the goal active: full reverse engineering, a complete 3D world-map
extraction, and a physical-distance LOD proof are not complete.
