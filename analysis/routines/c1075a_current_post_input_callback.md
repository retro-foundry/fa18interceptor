# `$C1075A`: current post-input callback

## Evidence

The bounded `build/training_600_c0f5f8/trace.jsonl` packet enters `$C1075A`
from the `$C0F7D2` indirect callback call. It executes:

```text
$C1075A  tst.b $C458AC
$C10760  beq $C10788
$C10788  rts
```

and returns to `$C0F808`. Byte-exact static source is
`source_amiga/observed/run_current_post_input_callback.asm`.

## Observed contract

With `$C458AC == 0`, this callback has no state write and returns. Its static
nonzero path optionally sets `$C458AD` to two, calls `$C11312`, reloads
`$C45AD6` with three, and changes `$C1820C` to `$C1078A`. That nonzero path is
not yet dynamically observed.
