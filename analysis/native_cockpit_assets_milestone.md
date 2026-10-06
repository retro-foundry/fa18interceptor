# Native cockpit artwork correction

The playable caller is `fa18_native` -> `native_frontend_open` ->
`native_cockpit_load`, then `native_flight_tick` -> `native_hud_draw` ->
`draw_panel_frame` / `draw_panel_image`. The native runner now loads original
`pix/inst5` (320 x 55, five planes) and `pix/frnt5` (288 x 12, five planes)
through the existing read-only OFS and ILBM facilities. C0E57E/C0E59E are the
original loader callers; C16982 publishes the plane caches and creates the
C16AD4/C16B2A/C16B58 four-plane union mask. The renderer retains the original
plane reversal, view clipping, image placement and instrument redraw cadence.

The user's missing-cockpit observation was correct. Earlier HUD milestones
connected drawing calls but omitted these two disk resources. Their source
comparisons used empty image pointers and did not prove loaded cockpit artwork.
No substitute artwork was created. Runtime pixels now come from the ADF; no
captured planes are imported.

The first display page also overlapped the original immutable image Hunk 62
at $012988, which contains mode/mark/compass images. It now resides at $034000,
separate from that source data, recording storage, circle workspace and render
buffers. The second page remains at $040000. Cockpit asset storage occupies
$064000..$067FFF, after the render banks. The existing ILBM decoder exposes
the original BMHD fields for the source bitmap consumers.

Validation:

- All eight rendering planes, both 20-byte headers and the 432-byte panel mask
  match the existing original startup checkpoint byte for byte. This reuses
  the prior one-frame reference artifact; no new original replay was run.
- Four dirty-cache/dirty-mask cases execute original C16982 against the actual
  native cache/mask function. Every non-stack RAM byte matches, with the two
  external allocation results shared between validation paths.
- At native demo updates 2,400 and 3,000, 115 populated HUD/panel drawing cases
  per checkpoint match original instructions and plane bytes. These include
  centered, panned and clipped views. The game executes 335 and 935 HUD passes.
- The immutable embedded image package and complete loaded cockpit asset bank
  remain unchanged across both native flight checkpoints. A fresh PPM at
  update 3,000 visibly contains the instrument panel and cockpit front frame.
- Native frontend/menu/save/SDL/link-omission checks pass. Postflight display
  waits, page preservation and reset tests pass, including 128 original display
  cases. Its page-preservation check now follows actual page pointers. Its
  fixture's heading enum alias is repaired after the preceding demo integration.
- Both MSVC reference runners build and their twelve focused CTests pass.
  The native MSVC build and GNU validation builds pass; the ADF remains sealed.

Cockpit disk artwork is **2/2 assets connected and verified (100% of that
scope)**. Functional scenario wiring remains 3/3; full native frame parity
remains **0/3 accepted**. This does not establish whole-game completion.
Copper fade is excluded. Startup sound availability, remaining frame/result
cadence, other missions and actual audio output remain open.

Run `python tools/native/check_cockpit_assets.py`; optionally pass
`--source-initial build/native-flight/reference-demo-initial.dat` to reuse the
existing original checkpoint for exact asset comparisons.
