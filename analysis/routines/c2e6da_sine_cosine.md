# `lookup_sine_cosine` at `$C2E6DA` (Hunk 32 +`$BE`)

Classification: **behavioural**. This is a byte-exact native-angle
trigonometric lookup helper.

## Proven contract

Input `D4.w` is doubled internally and reduced through four table-symmetry
cases. The routine returns a signed pair in `D4.w` and `D5.w`, using the word
table at `$C3E5E8` (verified Hunk 63). The table begins `0, 29, 57` and reaches
`$4000`; the observed zero-angle invocation returns `D4 = 0`, `D5 = $4000`.
The four code paths reflect and negate those values in the standard sine/cosine
quadrants. The angle unit is deliberately left unnamed.

The full `$C2E6DA-$C2E74F` routine is reconstructed in
[`source_amiga/observed/lookup_sine_cosine.asm`](../../source_amiga/observed/lookup_sine_cosine.asm).
It assembles to 118 bytes identical to the restored runtime image.

## Runtime evidence

From the no-input attract replay, `$C2DD6C` calls this helper and returns to
`$C2DD70` after 11 instructions. With input `D4 = 0`, the measured result is
`D4 = 0`, `D5 = $4000`. The packet is
`build/no_key_c2e6da_trace/`; its raw P-code at `pcode/raw/no_key_c2e6da/`
contains 11 observed instructions, 66 P-code operations, all mapped to Hunk
32.

The helper appears ten times in the bounded `$C1C63E` update-stage packet,
from several Hunk-32 callers. That establishes reuse inside the observed stage
but does not establish whether each call transforms world, view, terrain, or
other coordinates.
