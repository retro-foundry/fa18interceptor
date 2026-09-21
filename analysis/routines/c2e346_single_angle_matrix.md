# `build_single_angle_matrix` at `$C2E346` (Hunk 32 +`$F3E`)

Classification: **behavioural**. This helper writes a nine-word matrix derived
from one native angle word.

It arithmetic-shifts `D4.w` by three, calls `lookup_sine_cosine`, shifts the
returned pair by six, and writes a fixed `$0100` axis term plus zero terms and
signed sine/cosine terms through `A1`. The complete 42-byte source is
[`source_amiga/observed/build_single_angle_matrix.asm`](../../source_amiga/observed/build_single_angle_matrix.asm)
and matches the restored runtime image exactly.

The no-input matrix pipeline calls it at `$C2DADC`; it returns to `$C2DAE0`
after 30 instructions and writes at `$C45BFC`. Its P-code packet,
`pcode/raw/no_key_c2e346/`, contains 30 instructions / 289 operations, all
mapped to Hunk 32. This matrix representation uses `$0100` in the observed
fixed axis, so it must not be conflated with the `$4000` rotation matrix at
`$C45B90`. The game-level meaning of its angle is unknown.
