# Run031 landmark-window M-map command rejection

Classification: **state-gate evidence; no causal gate identified**.

An isolated raw `M` event does not reach the visible map display from three
run031 landmark-window states. Each current probe restores its sealed state,
delivers the press/release at relative frames 1 and 6, then runs 160 normal
frames:

| source state | visible scene before command | result |
| --- | --- | --- |
| frame 12,000 checkpoint | Golden Gate cockpit pass | cockpit/world view remains |
| frame 13,200 rebuilt checkpoint | Alcatraz-window cockpit pass | cockpit/world view remains |
| frame 14,500 checkpoint | later bridge cockpit pass | cockpit/world view remains |

These must not be used as M-map coastline or landmark-position captures. The
frame-13,200 checkpoint was rebuilt by a normal replay of sealed
`captures/run031/playback.e9k`; all three tests use the same relative
`E9K_INPUT_V1` key-109 press/release that opens the map from run033's Golden
Gate checkpoint.

The byte-exact map-command prelude first tests `$C458AE`. It is zero in the
successful Base-1, Base-3, Base-4, and run024 checkpoints **and** in the
run031 frame-12,000/frame-14,500 snapshots, so that documented early gate
does not explain the rejection. A correlated distinction is `$C4599C`: it is
`$00` in successful pre-map snapshots and `$FA` in all three inspected run031
landmark snapshots. `$C1BF8C` sets bit 0 of this byte. A debugger-only
Alcatraz control that writes this byte to zero before the same event produces
the identical final video digest as the unmodified control. It is therefore
not sufficient to enable M-map mode; another shared state field, demo-mode
control, or later transition logic remains responsible.

The safe conclusion is operational: map capture must begin from a verified
free-flight state whose isolated M event visibly reaches the green/blue map.
It is not valid to infer a landmark's M-map coordinate solely from an
otherwise labelled flight-window snapshot.

Authority: byte-exact
[`dispatch_map_command_prelude.asm`](../../source_amiga/observed/dispatch_map_command_prelude.asm),
the sealed frame-12,000/frame-14,500 snapshots, rebuilt frame-13,200
checkpoint, and ignored `build/run031_frame{12000,13200,14500}_m_map_control/`
artifacts. The isolated mutated Alcatraz control is retained separately in
`build/run031_frame13200_alcatraz_m_map_request_flags_zero/`.
