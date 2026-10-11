# run075 frame559 `$C30678` workspace handoff

The extended breakpoint trace at `$C30678` starts at engine frame 559 and
executes 500 instructions. It confirms that the routine has two phases:

1. It writes the recovered packet fields to the display blitter setup area.
2. It walks the sorted pair records, computes a row offset using the value at
   `$C456E2`, and submits the operation for each accepted record.

The recovered packets set BLTCPT and BLTDPT from the same `D7` value. This is
an in place operation on a temporary plane workspace. The first changed Chip
regions from the bounded trace are outside the Copper active page, while the
later renderer path performs the copy into the visible plane family.

This separates the native model into a semantic temporary planar workspace and
a visible `FA18PlanarPage`. The `0x6e..` values are retained as packet fields
for trace comparison, but are not treated as display coordinates. The next
required evidence is the later active-page copy and its source region.

Authority: `build/run075_frame559_c30678_500/trace.jsonl` and its Chip
snapshot pair, captured from the sealed run075 restore and playback.
