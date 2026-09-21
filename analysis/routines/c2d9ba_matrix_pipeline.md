# `$C2D9BA` matrix pipeline (Hunk 32 +`$5B2`)

Classification: **structural**. This bounded routine owns the observed
two-angle matrix, row-scaling, and single-angle matrix sequence. It is not yet
identified as camera, player, AI, or renderer state.

## Evidence packet

- No-input `start_demo` replay from `captures/baseline_menu/state.bin`.
- Breakpoint `$C2D9BA`, hit at frame 608; observed return `$C2D9A8`.
- 433 instructions with no input after the breakpoint.
- P-code at `pcode/raw/no_key_c2d9ba/`: 389 observed RAM starts, 3,088
  operations, all mapped to verified Hunk segments.

The observed route calls `$C091E0`, `$C123FA`, `$C25980` twice, and `$C2564E`
before the local Hunk-32 matrix sequence. It then:

1. builds the `$C45BD8` two-angle matrix through `$C2E38E`;
2. scales its rows through `$C2E5AC` using `$C45A3E`;
3. builds the `$C45BFC` single-angle matrix through `$C2E346`;
4. copies three words from `$C461EA` to `$C45A88`.

The surrounding code selects inputs and branches on several state bytes,
including `$C457B4`, `$C457AE`, and `$C457B5`. Their meanings, the origin of
the three copied words, and the consumer of the matrices remain unknown.

`$C2D99C` calls this routine in the observed parent update sequence, returning
to `$C2D9A8` before its own `RTS`.

## Verified upward edge

A separate no-input trace starts at `$C2D99C`, hits at frame 608, and returns
to `$C0F030` after 437 instructions. Its four extra instructions are the
wrapper's test/call/return around this pipeline. The annotated raw P-code is
`pcode/raw/no_key_c2d99c/` (393 observed starts, 3,099 operations, all mapped).
This confirms the observed call edge:

```text
$C0F02A  jsr $C2D99C
  $C2D9A4  bsr $C2D9BA
    matrix pipeline
  $C2D9A8  rts
$C0F030  continuation
```

## Reconstructed source

The complete `$C2D9BA-$C2DADF` body is now byte-exact source in
`source_amiga/observed/update_matrix_pipeline.asm` (294 bytes). Field and
callee names in that file remain structural where the bounded packet does not
prove a game-level role.
