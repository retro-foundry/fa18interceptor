# `$C1F99A`: descriptor-resolved transform lane exclusion

Meaning level: **dataflow**, frame-1966 trace-backed.

At frame 1966, `$C1F99A` enters with `A3=$C48390`, but its setup adds the signed low word of `D7` before transforming. The captured lane resolves `$C45A32` to descriptor `$C399CE`, source `$C39C2A`, and destination `$C485E2`.

That destination is outside the `$C483xx-$C484xx` records used by the observed `$C34A9A` pre-clip faces in the composite pass. Therefore this lane must not be promoted as the static source transform for the C34A face family. It is an explicit rejected candidate, not evidence against some other `$C1F99A` invocation or another transform routine supplying those records.

`scripts/collect_c1f99a_transform_entries.py` reproduces the descriptor/source/destination capture. The frame-1965-to-1972 report also records later distinct lanes, including `$C35154` descriptor data, but none establishes C34A source ownership.
