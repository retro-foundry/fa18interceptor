# `$C3B4F8` debugger-injected control-parser probe

Classification: **debugger-only parser compatibility experiment**. This is not
an original scenario transition and cannot prove map ownership, a mountain
instance, or that ordinary execution reaches this target.

From the sealed run037 map checkpoint, execution paused at `$C1F6F8` and the
debugger wrote `$C3B4F8` to `$C45A36` before the byte-exact walker executed.
At trace index 5, `$C1F716` reads the injected pointer through `A5=$C3B4F8`.
Its relative-control chain subsequently enters `$C31BE4`, `$C3A490`, and
`$C31BDC`; it then reaches the already observed `$C35C4C/$C35BD2` stream
region. Within the retained 12,000-instruction cap it also reaches five
`$C1F4AC` entries, four `$C2FF48` polygon submissions, and eight `$C2FA7E`
line-emitter entries.

The probe never executes `$C3B4F8`, `$C3B4FE`, or `$C3B588` as code: those are
data/control addresses, and the walker reads them through pointers. The
original, unmodified map samples instead overwrite `$C45A36` before their next
walker entry, as recorded in [the transition audit](c3b4f8_map_store_walker_audit.md).

Authority: ignored reproducible
`build/run037_c3b4f8_debugger_parser_probe/{trace.jsonl,trace_summary.json}`;
the preserved restore was
`build/run037_m_map_template_to_placement_3f_trace/state.bin`. The sole
debugger write is recorded in that probe's snapshot metadata.
