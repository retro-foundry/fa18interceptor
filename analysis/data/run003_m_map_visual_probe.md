# Sealed `M`-command visual probe

Classification: **scenario-backed display observation**. This records what the game visibly renders after the already traced raw `M` command. It is not an asset extraction or proof that the displayed pixels are the 3D terrain source.

## Reproduction

Restore the saved state immediately before the first sealed run003 `M` event, then replay the same input stream:

```powershell
python scripts/engine9000_bridge.py --restore build/run003_pre_m_2183/state.bin --playback captures/run003/playback.e9k --start-frame 2183 --frames 30 --output build/run003_m_visual_30
```

At frame 2188 (five replay frames), the screenshot retains the flight cockpit. At frame 2213 (30 replay frames), the image is a full-screen green/blue coastline-style map display with a grid and navigation readouts. Its recorded video hash is `1828e0c0717c86ddb8ee3d4afa1ce15b2f0c9b028b8a0201cbae65cdc462f9f3`. The same hash is present again at frames 2243 and 2363, so this is a stable post-transition display rather than a partial redraw.

[Open the reproducible frame-2,213 map screenshot](../visuals/run003_m_map_display.png).
The tracked PNG is copied byte-for-byte from
`build/run003_m_visual_30/screen.png` (SHA-256
`933d6071fbc3b56f7f01bf2f1bee824799647a510327fc22f79d98b7bb7b0200`).
It is a visual verification artifact, not an extracted 3D terrain dataset.
An independent run035 end-of-flight map view shows this coastline panning with
flight state; see the [two-position comparison](run003_run035_m_map_pan_comparison.md).

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

Both this snapshot and the independently reproduced run035 map view use the
same active RGB4 palette entries: in particular land green is
`COLOR04=$0151` and water blue is `COLOR06=$0036`. The complete 16-entry
sequence matches the shared `pix/frnt5`/`pix/inst5` disk palette catalogued in
`disk_graphics_assets.json`. This establishes shared mutable palette state;
it does not make either cockpit image file the coastline source. The disk
inventory contains no dedicated map/coastline asset.

The displayed planes are mutable: the first page's four plane digests differ
between the pre-`M` snapshot and frame 2213. Therefore the visible map is a
useful rendering oracle and an identified screen target, but neither its
current Chip-RAM bytes nor the Copper-list page is an immutable authoritative
terrain dataset.

## When the map becomes visible

A no-future-input continuation from the five-frame post-`M` checkpoint keeps
the cockpit image through continuation frame 12.  The coastline image first
becomes the displayed image at continuation frame 13 (equivalent to replay
frame 2201), and stays byte-identical through frame 25.  The transition is
therefore a prepared back-page becoming visible, rather than a gradual
coastline draw into the already displayed page.

The 13-frame Custom-chip trace records substantial CPU-originated display
work during frames 1--12 (between 100 and 792 writes per frame); the common
write sources include `$C30678-$C3069C`, and later `$C2FB72-$C2FD0A` and
`$C304E2-$C304F0`.  Frame 13 has only eight CPU-originated writes before the
Copper page setup.  This supports the page-presentation interpretation, but
these are generic renderer/blitter code addresses, not a recovered coastline
asset reader or an association with the 3D terrain-template directory.

The transition's reconstructed blitter jobs also rule out a tempting false
asset boundary.  During frames 1--8, `$C304F4` jobs read and write addresses
within `$006000-$007FFF`, while paired jobs write the same family of addresses
as `B`/`D` destinations.  Other jobs in those pairs use the pending map page
as `B`/`D`.  The `$006000-$007FFF` range is therefore mutable renderer scratch
in this scenario; matching bytes in separated snapshots do not establish an
immutable coastline resource there.  No enabled input channel has yet been
traced from a separately proven static map/terrain range to the completed map
planes.

## Renderer production of the prepared map page

The same trace and Custom-chip log now establish the immediate bitmap
producer. In frames 1--12, while the pending `$04DB30-$05582F` four-plane page
is prepared, 124 CPU `BLTSIZE` jobs have a pointer in that page. Forty-four
are direct span jobs at `$C304F4` and 76 are the four-plane line-job sites
`$C2FBE6/$C2FC4E/$C2FCB6/$C2FD1C`. The synchronized instruction trace also
reaches `$C2FF48` 42 times and `$C2FA7E` 19 times during the bounded
transition.

This is direct evidence that the page later presented as the map is produced
by the game's polygon/span and line renderer paths—not copied as a dedicated
coastline bitmap in this interval. It connects map-mode 3D renderer output to
the display target, but still does not assign an individual coastline pixel to
an immutable input record or extract the complete terrain model. The counts,
contexts, and exact reproduction inputs are in the [map display renderer
census](run003_m_map_display_renderer_census.md).

The census also supplies a bounded static-control-to-primitive handoff. At
trace frame 9, `$C1F6F8` enters with `A1=$C3B73E` in immutable scene-family
storage. Before the trace returns to another walker entry, it reaches four
`$C2FF48` wrappers with static `A5=$C3B6B0`: two complete through the line
route and two through the direct span route. This is the strongest currently
traced map-mode 3D control contribution to the prepared map page. The
immediately preceding map trace also transforms the known five-triple
immutable input `$C3B720-$C3B73D`; the complete bounded component chain is
recorded in the [static component boundary](c3b720_c3b6b0_static_component_boundary.md).
It is not a complete map export, and individual coastline pixels remain
unassigned.

The same bounded trace records the transform/control sequence for every one
of its six `$C1F6F8` entries: `$C35BDE -> $C35BF0`, `$C35BAA -> $C35BB8`,
`$C35BC2 -> $C35BD0`, `$C36220 -> $C36232` twice, and
`$C3B720 -> $C3B73E`. This is a small map-mode geometry-input index, not a
claim that the five source blocks or their repeated control entry are the
complete world terrain.

Their signed raw triples are retained in the renderer census. Several have a
nonzero middle component, including `$C3B720`'s fifth local triple
`(-640,1024,0)`. This is local component geometry and does not contradict the
separately observed flat X/0/Z *placement* layer; neither coordinate convention
may be promoted to a global elevation rule without a producer/consumer proof.

Authority: deterministic no-input renders
`build/run003_m5_noinput_1` through `build/run003_m5_noinput_25`, and
`build/run003_m_map_appearance_trace/{trace.jsonl,custom_writes.jsonl}` from
the sealed `build/run003_m_visual_5/state.bin` checkpoint; complete
transaction reconstruction is
`build/run003_m_map_appearance_blitter_jobs.json`.

The frame in which the prepared map page first becomes visible also runs the
established static terrain-template selector and reaches six static template
streams. This is shared transition-window activity, not a coastline-source
claim: the trace includes ordinary world update/render work and still lacks a
source-to-map-plane connection. See the [terrain-selector overlap](run003_m_map_terrain_selector_overlap.md).

## Boundary

The raw `M` entry at `$C1BF8C` is traced only through its request and capped helper boundary. Its static post-helper tail initializes transition state. The later pending-page renderer producer is now traced, but its immutable input source is not. Therefore this screenshot is a valuable visual oracle for future map-data work, not evidence that the flat template-placement cache, a particular static hunk, or the screen bitmap is the authoritative terrain model.
