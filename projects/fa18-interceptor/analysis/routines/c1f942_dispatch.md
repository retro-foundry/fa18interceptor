# Observed indirect dispatch at `$C1F942` (Hunk 10 +`$C0A`)

This instruction sequence is byte-verified in original Hunk segment 10. It:

1. masks `D0` with `$3FFF`;
2. loads `A0` from `$C1FCE8 + D0.W`;
3. executes `JSR (A0)`.

At the live `$C212B0` breakpoint, `D0 = $0034`, so table slot
`$C1FCE8 + $34 = $C1FD1C` contains `$C212B0`. The first 16 runtime table
entries are preserved as raw pointers in the trace evidence; their individual
discriminator meanings remain unknown.

Observed targets from this single dispatch site:

| Target | Hunk identity | Frame-600 calls | Frame-1800 calls | Evidence |
| --- | --- | ---: | ---: | --- |
| `$C2005C` | 13 + `$374` | 15 | 22 | structural only |
| `$C207FE` | 13 + `$E86` | 1 | 1 | structural only |
| `$C212B0` | 14 + `$250` | 2 | 1 | reaches the observed line-emission path |
| `$C1FEF2` | 13 + `$1BA` | 0 | 1 | counted `$34`-byte stream skip; see `c1fef2_record_stream_stride_skip.md` |
| `$C20002` | 13 + `$2CA` | 0 | 1 | structural only |

This is an observed dispatch mechanism, not yet a proven object, primitive,
event, or main-loop table. Only the `$C212B0` target currently has a bounded
hardware-level contract.
