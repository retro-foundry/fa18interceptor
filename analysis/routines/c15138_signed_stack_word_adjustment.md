# `$C15138`: bounded signed stack-word adjustment

Classification: **observed arithmetic/dataflow with byte-exact source**.
The complete `$C15138-$C1518B` entry is reconstructed in
`source_amiga/observed/adjust_signed_stack_word_pair.asm`.

## Contract

The helper reads signed words at `10(A6)` and `14(A6)`. With `x = word(10,A6)`
and `y = word(14,A6)`, it computes `s = x + y`, then derives:

```text
if abs(s) > 4:  s = arithmetic_shift_right(s, 2)
else if abs(s) > 2: s = arithmetic_shift_right(s, 1)
else: s is unchanged

word(10,A6) = s - y
D0 = sign_extend(word(10,A6))
```

The exact comparisons are signed magnitude tests only after a negative sum is
negated in a local word; the division steps are 68000 arithmetic shifts and
therefore preserve negative signed rounding behavior.

## Evidence

The entry is present in six independent P-code exports. Observed callers at
`$C14ACE`, `$C14AE8`, and `$C14B26` invoke it from update-stage paths in the
attract and recorded-flight packets. Its caller-side argument ownership and
gameplay interpretation remain unassigned.
