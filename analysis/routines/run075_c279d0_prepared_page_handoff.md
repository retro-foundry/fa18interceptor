# run075 prepared-page `$C279D0` native handoff

Classification: **scenario-backed renderer input boundary**.

The first `$C279D0` invocation in the prepared-page interval occurs at local
trace frame 184 (global run075 frame 384) and returns after 4,593 instructions.
It is the invocation that targets the four-plane render-page pointer family;
the later visible frame selects the already prepared five-plane page.

At the entry packet's normal route, the source stores the three projection
inputs in its stack locals before selecting the original Hunk-25 `$C28124`
table:

| Source value | Frame-384 observed value | Native owner boundary |
| --- | ---: | --- |
| `$C45A72.w` | `$E7C1` | first grid component |
| `$C45A76.w` | `$E64E` | second grid component |
| `$C45A78.l` | `$FFFFFF83` | projection/depth component |
| `$C45BD8` | read by each projected record | nine-word renderer matrix |

The `$C27A54` trace row reads the published depth low word `$FF83`, proving
that the renderer packet consumes the projection publisher's output rather
than a frame counter. It then selects table `$C28124` because that signed
value is at least `-$80`; the exact 96-record Hunk-25 table is already loaded
by `FA18ProjectionGrid`.

`$0004` and `$F800` occur as `$C279D0`'s own later control values. They are
not the `$C45A72/$C45A76` packet. The saved slow-RAM bytes and the trace's
post-read register values agree on the signed packet above; the same tuple is
also present at the global-frame-392 first renderer invocation. This keeps
the fade-stage evidence separate from renderer-local setup values.

Thus the missing native composition is:

```text
scene/root transform -> projection packet ($C45A72/$76/$78)
                         + matrix ($C45BD8)
                       -> FA18ProjectionGrid pass
                       -> five-plane render page
                       -> existing Copper page presentation
```

The trace also proves that this must be data-driven: the top-level call arrives
with unrelated register values, then the packet forms its own inputs from the
published workspace. The frame-384 values above are an oracle for integration
tests, not constants permitted in the runtime path.

The native port now exposes this exact bounded composition through
`fa18_render_flight_scene_pipeline`: caller-owned selected record bytes and
live matrix decode/publish the packet, then the existing Hunk-25 traversal
submits through a caller-initialized five-plane renderer. It deliberately does
not choose the record, matrix, render page, display page, or call cadence.

## Normal-replay cadence sample

Single-frame ordinary-replay PC profiles for global frames 370--392 sample
`$C279D0` on global frames 373, 384, and 392, and sample `$C1612C` on frames
380 and 389; `$C1718E` is sampled in every profiled frame. PC sampling cannot
prove that an unsampled routine did not execute, so these are entry witnesses,
not a complete call cadence. The renderer and display publication paths remain
separate source paths.

A return-bounded renderer trace armed at local frame 171 reaches `$C279D0` on
local frame 173 (global frame 373) and returns to `$C0F0C8` after 11,352
instructions. Its entry Slow-RAM state has `$C456B6=$C4567E`,
`$C4566C=0`, and the same `$E7C1/$E64E/$FFFFFF83` packet recorded for the
known frame-384 and frame-392 calls. The known render-page trace has instead
`$C456B6=$C4566E`; frame 392 returns to `$C4567E`. Thus the source-backed
page selection is:

| Global frame | sampled `$C279D0` entry | `$C456B6` page family |
| ---: | :---: | --- |
| 373 | yes | `$C4567E` |
| 384 | yes | `$C4566E` (prepared display-page lower lanes) |
| 392 | yes | `$C4567E` |

The trace artifacts are `build/run075_frame373_c279d0_full/`,
`build/run075_frame382_c279d0_render_page/`, and
`build/run075_frame392_c279d0_first/`. This is evidence for a future
state-owned selector/scheduler, not authority for frame-number logic or an
inferred call cadence in the native replay.

## Counted outer-loop boundary

`scripts/sample_run075_outer_loop_hits.py` uses a live `$C1612C` breakpoint,
steps one instruction only to escape each entry, and resumes ordinary replay.
Over frames 201--392 it records 392 entries. A fresh full-window replay
confirms that this aggregate is variable; it is not one child per display
frame. In frames 360--392, each replay frame has six child entries and their
pre-tail indices alternate `1,0,1,0,1,0`. The adjacent `$C0EFD4` parent
sampler sees the same six-entry late window. Earlier frames contain zero, one,
four, five, or six entries, so neither count authorizes a fixed native
per-frame schedule.

The child toggles `$C4566C` in its tail, making each measured index the page
the preceding `$C2F558` selected for rendering and the page the child is
about to publish for display. The known frame-384 renderer sees `$C4566E` and
writes the prepared display page; a later index-zero child publishes that
page before toggling back. This is an observed page handoff, not a replay-frame
counter. The generated reports are
`build/run075_parent_update_hits_201_392.json` and
`build/run075_outer_loop_hits_201_392_recheck.json`.
