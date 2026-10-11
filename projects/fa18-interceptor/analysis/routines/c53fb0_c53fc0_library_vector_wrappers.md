# `$C53FB0` / `$C53FC0`: graphics-library blitter ownership wrappers

Classification: **behavioral OS-wrapper identification**.

The two contiguous, byte-exact 16-byte leaves preserve `A6`, load the library
base pointer at `$C182CA`, issue a library-vector call, restore `A6`, and
return. `$C53FB0` targets `-$1C8(A6)` and `$C53FC0` targets `-$1CE(A6)`.

The run060 frame-9,284 Slow-RAM checkpoint has `$C182CA=$00C028F6`. Its
Amiga `Library` node's `ln_Name` pointer at `+$0A` is `$00FC53FE`; the pinned
Kickstart 1.3 ROM contains the null-terminated string `graphics.library` at
that address. The pinned target's `inline/graphics_protos.h` maps the two
LVOs to `OwnBlitter()` (`-456`, `$1C8`) and `DisownBlitter()` (`-462`,
`$1CE`) respectively. Therefore the wrappers are identified as:

```text
$C53FB0  graphics.library OwnBlitter
$C53FC0  graphics.library DisownBlitter
```

Both are observed from the outer update loop; `$C53FB0` also occurs in the
run024 crash-result execution sample. The outer-loop order is `$C2F558`,
`OwnBlitter`, parent update, `DisownBlitter`, `$C1612C`, then the
`$C15DB2 -> $C15D96` back-edge. This establishes an OS blitter-ownership
bracket around the parent update. It does not establish which child submits
each blit, whether `$C1612C` waits for a frame, or the outer scheduling source.
