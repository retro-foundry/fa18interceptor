# Run031 landmark-window M-map command rejection

Classification: **state-gate candidate; not causal proof**.

An isolated raw `M` event does not reach the visible map display from three
run031 landmark-window states:

| source state | visible scene before command | result after 100 frames |
| --- | --- | --- |
| frame 12,000 checkpoint | Golden Gate cockpit pass | cockpit/world view remains |
| frame 13,200 ordinary replay checkpoint | Alcatraz-window cockpit pass | cockpit/world view remains |
| frame 14,500 checkpoint | later bridge cockpit pass | cockpit/world view remains |

These must not be used as M-map coastline or landmark-position captures. The
frame-13,200 checkpoint was built by a normal replay of sealed
`captures/run031/playback.e9k`; all three follow-up tests delivered exactly
one raw key-109 event using the same `E9K_INPUT_V1` form that succeeds for the
run001--run004 and run024 M-map captures.

The byte-exact map-command prelude first tests `$C458AE`. It is zero in the
successful Base-1, Base-3, Base-4, and run024 checkpoints **and** in the
run031 frame-12,000/frame-14,500 snapshots, so that documented early gate
does not explain the rejection. A correlated distinction is `$C4599C`: it is
`$00` in the successful pre-map snapshots and `$FA` in both inspected run031
landmark snapshots. `$C1BF8C` sets bit 0 of this byte, but this observation
does not establish whether `$FA`, another shared state field, demo-mode
control, or later transition logic prevents the display change.

The safe conclusion is operational: map capture must begin from a verified
free-flight state whose isolated M event visibly reaches the green/blue map.
It is not valid to infer a landmark's M-map coordinate solely from an
otherwise labelled flight-window snapshot.

Authority: byte-exact
[`dispatch_map_command_prelude.asm`](../../source_amiga/observed/dispatch_map_command_prelude.asm),
the run031 frame-12,000/frame-14,500 snapshots, and ignored isolated replay
artifacts produced during this check.
