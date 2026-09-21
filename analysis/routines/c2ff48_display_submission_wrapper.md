# Display submission wrapper at `$C2FF48` (Hunk 36 +`$AF8`)

Classification: **behavioural display wrapper**. The complete run001 packet
enables a custom-chip DMA bit, walks a caller-provided list through a local
helper, and reaches the verified blitter line emitter. It does not establish
the list's primitive, object, or camera ownership.

## Runtime packet

- Direct edge `$C24D60 -> $C2FF48 -> $C24D66`, hit at replay frame 9.
- 969 instructions, complete at the observed return boundary.
- P-code: `pcode/raw/run001_c2ff48_renderer_stage/`, 214 observed RAM starts /
  1,184 operations. The imported trace has two observed RAM call targets.

At entry `$C2FF4A`, the wrapper writes `$8400` to custom register `DMACON`
(`$DFF096`). It then calls `$C301F6`, which iterates state rooted at
`$C4B390`; its observed path calls `$C302B6 -> $C2FA7E`, the known blitter
line emitter. `$C2FF56` branches to `$C2FF46` for the observed return.

This proves a hardware display-submission path and DMA enable operation for
this invocation. The individual records and rendered geometry remain unknown.
