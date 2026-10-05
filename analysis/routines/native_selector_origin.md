# Native active-origin owner

`port/native_selector_origin.c/.h` implements the complete hot
`$C29042-$C295D0` active-origin update on ordinary caller-owned state. Its
behavioral authority is the complete readable reconstruction in
`port/game/selector_origin.c`, the original slices listed in
`c29042_active_origin_update.md`, and the sealed original-instruction proof in
`native_c_selector_origin.md`.

The native owner calls the matrix preparation child before testing the source
gates. The direct record route replaces only the first and third components,
preserving the existing middle component. The matrix route retains signed
angle history, byte index rotation, six-byte table rows, word-width scaling,
matrix-child choice, candidate publication before the floor clamp, and the
wrapped signed floor calculation. Static rows are supplied as immutable
`PortFieldWindow` values, so signed row offsets have explicit bounds without
creating an emulated address space.

Adjustment modes zero through eight preserve their original signed threshold
tests, route-selected shift, candidate blend and preset behavior, small matrix
inputs, signed countdown expiry, regeneration exits, normalization, smoothing
and masked/negated companion publication. Arithmetic helpers retain 68000
word/long wrapping and arithmetic right shifts. Invalid mode or unavailable
state fails explicitly after any source-ordered writes already completed.

Four real lower routine families remain explicit: current-matrix preparation,
matrix transforms A/B and candidate regeneration. Their
ordinary triples are separate from completion status. They can mutate the same
live state before the parent resumes. The native component contains no CPU
registers, guest addresses, bus, interpreter, or machine dependency.

`$C1C63E` now calls this owner directly. The former
`FA18NativeRecordUpdateOps.update_origin` boundary has been removed. Focused
contracts cover gate exit, direct publication, matrix selection and floor
clamping, normalization/smoothing and regeneration. The existing sealed proof
establishes the behavior of the readable authority; a direct differential
harness for the full ordinary-state adapter remains useful before final parity
signoff. Its `$C29548` adjustment tail now shares the actual native vector-math
owner with record view, removing the normalization child callback entirely.
The tail's direct sealed-byte proof passes 32,768 calls across four entries at
all 143 boundaries with no child contracts; all shared math/origin outputs and
game RAM match. See `native_vector_math.md` for the arithmetic and exact scope.
