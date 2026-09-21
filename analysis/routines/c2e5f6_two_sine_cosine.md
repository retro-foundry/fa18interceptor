# `lookup_two_sine_cosine` at `$C2E5F6` (Hunk 32 +`$1DA`)

Classification: **behavioural**. This is a paired native-angle sine/cosine
lookup using the same verified Hunk-63 table at `$C3E5E8` as
[`lookup_sine_cosine`](c2e6da_sine_cosine.md).

It doubles `D0.w` and `D2.w`, independently applies four quadrant-reflection
cases, and returns sine/cosine pairs in `(D0.w, D1.w)` and `(D2.w, D3.w)`.
The upper words of those data registers are not part of this routine's proven
word-result contract. The native angle unit remains unknown.

[`source_amiga/observed/lookup_two_sine_cosine.asm`](../../source_amiga/observed/lookup_two_sine_cosine.asm)
reconstructs `$C2E5F6-$C2E6D9` as 228 byte-exact bytes.

From the no-input update-stage packet, `$C2E480` calls the helper and returns
to `$C2E484` after 20 instructions. The observed low words of both inputs are
zero and both output pairs are `(0, $4000)`. Its P-code packet at
`pcode/raw/no_key_c2e5f6/` contains 20 instructions / 128 operations, all
mapped to verified Hunk 32.

This helper has six calls in the bounded `$C1C63E` stage. The callers’ angle
sources and vector meanings are not yet established.
