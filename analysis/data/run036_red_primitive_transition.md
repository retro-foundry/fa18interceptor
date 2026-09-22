# run036 Golden Gate primitive-transition probe

Classification: **scenario-backed line correlation; no filled-face association**.

`run036` is the sealed, no-flight-control Golden Gate pass.  The exact
outside-viewport RGB `#880000` raster grows through the following measured
spans.  These coordinates are converted to the renderer's 320-wide bitmap
space with `x=(screen_x-40)/2`, `y=screen_y-16`.

| replay frame | red raster bitmap bounds | C2FF48 finalized-polygon result |
| ---: | --- | --- |
| 5,000 | `x=114..152, y=101` | Three observed submissions in `$C3B504` project at `x=269..319, y=108..116`; they are not a landmark match. |
| 6,000 | `x=97..146, y=104..105` | Eleven submissions occur while debugger stepping advances subsequent host frames.  No source/control ownership connects any one to the red landmark. |
| 7,000 | `x=59..145, y=107..115` | Eight submissions are observed after arming at frame 7,000, but their mutable workspaces alone do not establish bridge or raster ownership. |

The large sample does produce a direct continuation of the *line* evidence.
The replay-preserved `$C2FA7E` collector records these two Golden Gate line
groups, all with the normal projection return `$C2F08E` and mask `$FF`:

| static `A5` context | line endpoints | combined bounds |
| --- | --- | --- |
| `$C35596` | `(125,112)->(125,106)`, `(145,111)->(106,111)`, `(106,111)->(125,106)`, `(125,106)->(133,110)` | `x=106..145, y=106..112` |
| `$C355CE` | `(86,114)->(86,108)`, `(58,115)->(107,111)`, `(107,111)->(86,108)`, `(86,108)->(76,113)` | `x=58..107, y=108..115` |

Together they span `x=58..145, y=106..115`, matching the red landmark's
full horizontal extent and vertical extent except the expected one-pixel
raster boundary.  This is the same type of line-to-raster correlation as the
earlier run035 result, now at the largest controlled red span.

The new polygon collector is replay-preserved: it now accepts `--playback`
and `--arm-frame`, delivers recorded events before the breakpoint is armed,
and records the mutable finalized triples/pairs at `$C2FF48`.  It makes a
filled-face test reproducible, but the present captures do **not** associate a
filled polygon with the red Golden Gate pixels.  In particular, they cannot
show that nearby polygons are absent: instruction stepping advances the host
frame and `$C4B990/$C4B390` have no immutable owner without upstream tracing.

Result: the proposed ``distant lines, then nearer filled polygons`` scheme is
**not established**.  At the largest red span sampled here, the Golden Gate
still has a direct line-renderer correlation.  A filled primitive may coexist,
but needs a face-to-raster match before it can be called a detail transition or
LOD.

A completed trace of the first frame-7,000 `$C2FF48` submission now separates
one direct-blitter polygon route from `$C2FA7E` line emission.  Its reflected
projected bounds are entirely outside the red Golden Gate raster, so it is
positive evidence that this particular polygon is unrelated—not evidence for
or against co-visible filled bridge detail.  See the [polygon span-path
probe](run036_polygon_span_path_probe.md).

The exact one-frame replay census strengthens the separation: it observes
three direct-span `$C2FF48` submissions with non-bridge `A5=$FFFFF2`, while
the eight red-raster-correlated line calls retain `$C35596/$C355CE` contexts.
It supports coexistence of both primitive routes without supporting a
line-to-polygon bridge transition.

Authority: sealed `captures/run036`; keyframes in ignored
`build/run036_keyframes`; replay-preserved ignored collector outputs
`build/run036_{4999,5999,6999,7000}_polygon_submissions` and
`build/run036_7000_line_entries`.

Reproduce the decisive large-span line sample:

```text
python scripts/collect_blitter_line_entries.py \
  --restore captures/run036/initial_state.bin \
  --config captures/run036/config.uae \
  --playback captures/run036/playback.e9k \
  --arm-frame 7000 --frames 7002 --max-lines 128 \
  --output build/run036_7000_line_entries
```
