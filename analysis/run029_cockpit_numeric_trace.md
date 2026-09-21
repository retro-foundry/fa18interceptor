# run029 final-cockpit numeric-renderer trace

## Authority

`build/run029_cockpit_numeric_trace/` replays sealed run029 normally through
frame 14,298, then instruction-steps chipset frame 14,299 with no future
recorded input.  It contains 7,252 instruction rows and 138 Custom-register
writes.  The final video hash is unchanged from the pre-trace frame:
`0198e648472461a1ca47615e439275d64a075a03bd32cbbfa9327a0ce3a133f6`.

## Accepted facts

- `$C0D730 -> $C2FD8C` executes once.  This is the already established active
  four-plane cockpit packet.
- Its four `BLTSIZE` triggers occur at `$C2FDF0`, `$C2FE3A`, `$C2FE90`, and
  `$C2FEDA`, with `A2 = $C4567E`, the live active-plane table.
- `$C30668-$C306AE` executes twice.  Its two jobs consume successive prepared
  records at `$C4B39A` and `$C4B3A0`.
- The frame does not execute `$C2EE4A`, `$C2FA7E`, or `$C2FB7A`; therefore it
  is not evidence for the separate observed projected-segment path.

## Result for cockpit numbers

This stable redraw cannot identify the writer/record for the visible KTS or FT
glyphs.  It is evidence that those glyph regions reside in the same mutable
four-plane display system, but the unchanged output makes every blit in this
frame compatible with copying pre-existing content.  No RAM field is named.

The next useful oracle must deliberately bracket a frame where a readable
numeric glyph changes, then compare the before/after Chip-RAM rectangle and
trace the corresponding blit source.  That experiment should retain normal
full-frame playback until the changed frame; only a no-future-input interval
may be instruction-stepped.

## Frame-latency and watchpoint boundary

The trace beginning after normal frame 992, stepping chipset frame 993,
does **not** repeat the frame-994 packet.  Its eight `BLTSIZE` triggers are
from `$C30D1C`, `$C30E40`, `$C2FBE6`, `$C2FC4E`, `$C2FCB6`, and `$C2FD1C`.
Their C/D pointers are in the alternate/working ranges beginning `$014600`
and `$0147FE`, rather than the Copper-visible run029 planes beginning
`$04DB30`.  Conversely, the frame-994 trace has the four self-addressed
active-plane maintenance submissions and two line jobs described above.

This establishes a buffered renderer cadence: the normal screenshot boundary
is not a safe assumption about which instruction-stepped chipset frame
produced its visible pixels.  In particular, neither trace may be promoted to
a speed/altitude formatter attribution.

Three ordinary-replay write watchpoints were placed at bytes proven different
between the normal frame-993 and frame-994 active-plane snapshots:
`$04DB58`, `$051FAB`, and `$053EEB`.  Each missed with `--any-source`.
The same helper also misses a trace-proven CPU store at `$C45968`; it therefore
does not reproduce instruction-stepped execution under ordinary full-frame
replay.  Exact CPU watches have separately been validated at `$C02D06`, but a
broad source-filtered blitter watch misses over frames 1--1000.  The relevant
blitter writes are therefore absent from this runtime's watchbreak stream.
These misses are an instrumentation boundary, not evidence that the
active-plane bytes did not change.  The normal snapshot comparison remains the
authority for the visible change; see
`analysis/engine9000_watchpoint_abi.md` for the ABI and validation details.

The next renderer experiment needs a blitter-completion/write log (or a
capture that exposes the pending blitter state and destination before the
frontend snapshot), rather than further blitter watchpoint probes.

## Normal line-submission buffer evidence

A breakpoint trace of `$C2FB7A` during ordinary replay (frame 990) reaches the
routine from `$C312D4`, not through the stepped `$C0D730` packet.  Its four
line blits target `$0142C9`, `$016209`, `$018149`, and `$01A089`, an off-screen
four-plane working family.  This independently confirms the buffered cadence:
the normal renderer is drawing a phase ahead of the Copper-visible five-plane
cockpit buffers.  See `analysis/routines/c2fb7a_blitter_line_plane_submission.md`.

## First changed-number boundary

Adjacent normal-playback screenshots establish a real output transition:

| Frame | HUD speed | HUD altitude |
| ---: | ---: | ---: |
| 993 | `161 KTS` | `145 FT` |
| 994 | `171 KTS` | `145 FT` |

The initial comparison was against the older attract-style plane set at
`$012BC0` onward.  That set is inactive in run029.  The Copper-source rows in
the trace identify the active run029 list at `$057858`; it points at a separate
five-plane display set.  Those active planes change by 2,760, 3,870, 438, and
352 bytes respectively across the 993-to-994 boundary.  See
`analysis/run029_active_cockpit_bitplanes.md`.

The stepped trace still programs the earlier four-plane packet and ends before
the active five-plane presentation is reflected in its exported RAM.  In the
frame-994 interval, however, its live destination registers resolve the packet
to the active plane-1 through plane-4 bases at `$04DB30-$05582F`, each plus
`$28`; see `analysis/run029_active_cockpit_bitplanes.md`.  It must still not
be used to attribute `$C4597C` through `$C45984`, or any other simultaneously
changing state word, to KTS.  The next trace must follow the prepared source
records consumed by this active-plane pipeline.
