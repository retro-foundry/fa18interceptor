# `$C309B6` segment-37 table-renderer wrapper

Classification: **runtime-backed structural/dataflow**. The `attract_1800`
P-code export reaches `$C309B6` from `$C0F182` once and records a 42-byte
body; its call to `$C30EAA` is also observed. The trailing `RTS` is retained
from static byte evidence.

`source_amiga/observed/invoke_segment37_table_renderer.asm` reproduces the
44-byte wrapper at `$C309B6-$C309E1`. It passes the first pointer from the
five-long table at `$C309A2` in `a3`, passes the remaining four entries via
`a1 = $C309A6`, initializes register constants, and calls `$C30EAA`.

The table values and the callee's visual or gameplay role are not yet proven;
the entry name records only its observed table-to-callee dataflow.
