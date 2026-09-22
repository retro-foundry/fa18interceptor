# Sealed `M`-command visual probe

Classification: **scenario-backed display observation**. This records what the game visibly renders after the already traced raw `M` command. It is not an asset extraction or proof that the displayed pixels are the 3D terrain source.

## Reproduction

Restore the saved state immediately before the first sealed run003 `M` event, then replay the same input stream:

```powershell
python scripts/engine9000_bridge.py --restore build/run003_pre_m_2183/state.bin --playback captures/run003/playback.e9k --start-frame 2183 --frames 30 --output build/run003_m_visual_30
```

At frame 2188 (five replay frames), the screenshot retains the flight cockpit. At frame 2213 (30 replay frames), the image is a full-screen green/blue coastline-style map display with a grid and navigation readouts. Its recorded video hash is `1828e0c0717c86ddb8ee3d4afa1ce15b2f0c9b028b8a0201cbae65cdc462f9f3`. The same hash is present again at frames 2243 and 2363, so this is a stable post-transition display rather than a partial redraw.

The local visual captures are:

- `build/run003_m_visual_5/screen.png` — cockpit before the transition.
- `build/run003_m_visual_30/screen.png` — stable map display.
- `build/run003_m_visual_180/screen.png` — same stable map display after a longer wait.

## Display-memory verification

A second deterministic replay collected normal Custom-chip writes at frames
2213 and 2363. The Copper programs four consecutive bitplane pointers at the
start of the display. Each plane is `$1F40` (8,000) bytes, giving an observed
320 by 200, four-plane screen target:

| Replay frame | Copper-programmed plane starts | Copper-list source |
| ---: | --- | --- |
| 2213 | `$04DB30`, `$04FA70`, `$0519B0`, `$0538F0` | `$057858` |
| 2363 | `$012BC0`, `$014B00`, `$016A40`, `$018980` | `$00A468` |

The complete SHA-256 digest of every 8,000-byte plane at frame 2363 equals the
digest of its respective frame-2213 plane. The stable screenshot hash thus
comes from equivalent display pages at distinct Chip-RAM addresses. This is
direct evidence of display-page swapping/double buffering for the `M` view.

The displayed planes are mutable: the first page's four plane digests differ
between the pre-`M` snapshot and frame 2213. Therefore the visible map is a
useful rendering oracle and an identified screen target, but neither its
current Chip-RAM bytes nor the Copper-list page is an immutable authoritative
terrain dataset.

## Boundary

The raw `M` entry at `$C1BF8C` is traced only through its request and capped helper boundary. Its static post-helper tail initializes transition state, but the producing pixel/asset path has not been traced. Therefore this screenshot is a valuable visual oracle for future map-data work, not evidence that the flat template-placement cache, a particular static hunk, or the screen bitmap is the authoritative terrain model.
