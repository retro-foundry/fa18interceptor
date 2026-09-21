# `$C24CFE`: polygon projection and renderer-submit tail

Classification: **runtime-backed common scene projection/output path**.

`source_amiga/observed/project_clamp_and_submit_polygon_tuple_list.asm` is
byte exact for `$C24CFE-$C24DA7`.

## Contract

The tail consumes `D7` finalized triples from `$C4B990`. For each positive
depth `z`, it computes:

```text
x = clamp(160 + 160*x/z, 0, 319)
y = clamp( 90 +  90*y/z, 0, 179)
stored_x = 319 - x
stored_y = 179 - y
```

It writes a count plus the projected pairs to `$C4B390`, calls `$C2FF48`, and
increments `$C46182` on success. A nonpositive tuple depth clears the output
count and returns zero. The adjacent common failure entry at `$C24D8C`
increments `$C458EC`, reports error `$A`, and returns zero for the earlier
clip-error paths.

## Renderer chain

For the run031 Golden Gate control packet the now-observed path is:

```text
scene control records → clipping / tuple cache → $C4B990 triples
→ $C24CFE projection → $C4B390 screen pairs → $C2FF48 renderer submission
```

This proves the common geometry-to-renderer handoff. It does not by itself map
a particular control record to a particular bridge pixel or model part.
