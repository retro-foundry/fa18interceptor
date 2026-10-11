# `build_rotation_matrix` at `$C2E47A` (Hunk 32 +`$5E`)

Classification: **behavioural**. This routine constructs a 3×3 signed
fixed-point rotation matrix from three native-angle words.

It right-shifts `D0.w`, `D2.w`, and `D4.w` by three before calling
`lookup_two_sine_cosine` and `lookup_sine_cosine`. It combines the resulting
word values with signed multiplication and 14-bit arithmetic shifts, then
writes nine words at `A1` through `A1+$10` (with `A1` advanced to the final
word). The source reconstruction is
[`source_amiga/observed/build_rotation_matrix.asm`](../../source_amiga/observed/build_rotation_matrix.asm);
all 154 bytes match the restored runtime image.

## Runtime packet

The no-input attract packet starts at `$C2E47A` and returns to `$C2DF06` after
106 instructions. It begins with low-word inputs `D0 = 0`, `D2 = 0`, `D4 = 0`
and `A1 = $C45B90`. The nine observed stores form the identity matrix:

```text
$C45B90:  $4000, $0000, $0000
$C45B96:  $0000, $4000, $0000
$C45B9C:  $0000, $0000, $4000
```

This establishes `$4000` as the matrix’s observed unity representation for
this invocation. It does not establish which game object or coordinate frame
owns `$C45B90`.

The P-code packet `pcode/raw/no_key_c2e47a/` has 106 instructions and 956
operations, all mapped to verified Hunk 32. In the bounded `$C1C63E` stage,
the routine is called four times from `$C2DF02` and `$C2D968`.
