# `$C30EAA` segment-37 table renderer

Classification: **partially runtime-backed structural/dataflow**. The
`attract_1800` P-code export calls this entry from `$C309DC` and records 136
body bytes. Its two local helper entries at `$C30F46` and `$C30F56` are also
observed separately; unexecuted branch portions are static byte evidence.

`source_amiga/observed/run_segment37_table_renderer.asm` is byte-exact for
`$C30EAA-$C30F73` (202 bytes). The routine obtains a pointer block from
`$C456B6`, adjusts `d1` with `$C45918`, calls `$C310E2`, bounds/wraps the
resulting inputs, then consumes entries through `a1/a2`, calls `$C53F44`, and
writes a derived parameter set to the Custom block. It has an explicit reject
return of `d0 = -1`.

The pointer-table content and screen-level effect remain unproven. Labels
describe only directly observed control flow and register/memory dataflow.
