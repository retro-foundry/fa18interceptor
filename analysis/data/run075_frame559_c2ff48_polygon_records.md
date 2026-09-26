# run075 frame 559 `$C2FF48` polygon records

The full trace shows `$C2FF48` sorting the workspace at `$C4B390`, then
walking the resulting pairs through `$C3031C` and `$C305AA`. The main record
chain passed to `$C3031C` is:

```text
(0,319), (0,89), (71,89), (70,89), (90,89),
(105,89), (105,89), (122,89), (134,89), (110,89), (137,89), (200,89)
```

The first ten ordinary pairs are followed by two boundary records with
`D0=$FFFFFFFF`: `(83,65302)` and `(67,65362)`. `$C3031C` loads each pair into
`D0-D3` and calls `$C305AA`; the resulting display point operations are the
five `$C2F688` calls recorded in
`run075_frame559_c2f688_call_set.md`.

The leaf also prepares the display blit packet at `$C30678-$C306AE`, including
the fixed `$28` dimensions and the selected source and destination values.
This identifies the transition as a polygon and display packet construction
path, rather than an arbitrary 359 span image.

Authority: `build/run075_c2ff48_559_full/trace.jsonl`, captured from the
canonical run075 restore and playback with a breakpoint at `$C2FF48`, armed at
frame 558 and traced for 700 instructions.
