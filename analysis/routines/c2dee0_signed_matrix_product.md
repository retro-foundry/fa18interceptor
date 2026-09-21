# Signed matrix-product stage at `$C2DEE0`

Classification: **behavioural renderer/update primitive**.  The no-key replay
at frame 607 enters `$C2DEE0` from `$C2D700` and returns to `$C2D704` after 288
instructions (`build/no_key_c2dee0_trace/`).

The exact observed regions in `guard_signed_matrix_product_d0.asm`,
`guard_signed_matrix_product_d2.asm`, `guard_signed_matrix_product_d4.asm`,
and `compute_signed_matrix_product.asm` first guard signed `D0`, `D2`, and
`D4`, then call `$C2E47A`.  They multiply the 3×3 signed-word
matrix rooted at caller-supplied `A4` by the word matrix at `$C45B90`.  Every
output is a signed 32-bit sum of three `muls.w` terms, written in sequence to
the nine-long workspace `$C45BA2`.

The negative-input handlers and the rest of the consumer stage remain raw. The
evidence proves matrix arithmetic and workspace dataflow, not a particular
world/camera coordinate interpretation.
