# Run031 landmark-window M-map command rejection

Classification: **upstream keyboard-context gate identified**.

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
does not explain the rejection. The actual divergence is earlier: the
keyboard command gate sees `$C4584B=$03` in the run031 landmark states and
routes raw input to `$C1B030` rather than the direct-key table at `$C1AE28`;
the accepted run033 checkpoint has `$C4584B=$00` and reaches `$C1BF8C`.
Writing only `$C4584B=$00` in the Alcatraz-window diagnostic is sufficient for
the same raw M event to enter the original M-map renderer. A remaining
correlated distinction is `$C4599C`: it is
`$00` in successful pre-map snapshots and `$FA` in all three inspected run031
landmark snapshots. `$C1BF8C` sets bit 0 of this byte. A debugger-only
Alcatraz control that writes this byte to zero before the same event produces
the identical final video digest as the unmodified control. It is therefore
not sufficient to enable M-map mode. It is downstream of the actual observed
keyboard-context gate.

The safe conclusion is operational: an unmodified map capture must begin from
a state whose command-mode latch permits the direct-key table. A latch-mutated
capture can yield authentic renderer vectors but must remain diagnostic and
cannot, by itself, identify a landmark's map primitive.

Authority: byte-exact
[`dispatch_map_command_prelude.asm`](../../source_amiga/observed/dispatch_map_command_prelude.asm),
the sealed frame-12,000/frame-14,500 snapshots, rebuilt frame-13,200
checkpoint, and ignored `build/run031_frame{12000,13200,14500}_m_map_control/`
artifacts. The isolated mutated Alcatraz control is retained separately in
`build/run031_frame13200_alcatraz_m_map_request_flags_zero/`; the successful
mode-latch diagnostic is documented in
[`run031_frame13200_alcatraz_m_map_diagnostic.md`](run031_frame13200_alcatraz_m_map_diagnostic.md).
