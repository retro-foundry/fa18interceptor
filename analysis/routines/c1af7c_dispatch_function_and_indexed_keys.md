# `$C1AF7C` function and indexed-key dispatch

Classification: **structural with byte-exact source**. This 148-byte fragment
routes raw `$45` to the signed-input-state bridge, applies context gates, and
passes raw `$01-$0A` and `$50-$59` to their respective indexed and
function-key-level routes.

For other permitted contexts it maps raw `$1D`, `$1E`, `$1F`, `$2D`, `$2E`,
`$2F`, `$3D`, and `$3E` to indexes 0–7 in `D4`, then branches to `$C1BC78`.
The exact post-F10 trace proves the `$50-$59` route; the indexed keys are a
static routing observation pending bounded per-key traces.
