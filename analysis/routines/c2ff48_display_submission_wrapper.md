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

## Lower-run fixture excluded from the port evidence set

`build/port_run036_c2ff48_600_settle1/` repeats the first run036 frame-7,000
submission from the sealed initial state and configuration. It reaches
`$C2FF48` at replay frame 7,001, returns through `$C24D66` at trace index 540,
and contains three `$C304F4` direct-blitter triggers at indices 482, 515, and
536, with no `$C2FA7E` entry. The one-frame settled Chip-RAM result equals the
post-trace Chip-RAM result, so every submitted operation had completed.

The entry pair list is `(97,127)`, `(130,138)`, `(74,145)`, `(57,130)`. Only
110 bytes change, all in active plane 1 at `$012BC0`; the delta spans rows
129--144 and has these inclusive x ranges:

```text
129: 87..99     130: 67..102    131: 57..106    132: 58..109
133: 59..112    134: 61..116   135: 62..119    136: 63..122
137: 64..126    138: 65..129   139: 67..130    140: 68..125
141: 69..116    142: 70..106   143: 71..97     144: 73..88
```

This historical evidence is excluded from the current port contract. The
descending area-blit edge/mask contract requires a settled run060+ capture
before a native fill implementation is added.
