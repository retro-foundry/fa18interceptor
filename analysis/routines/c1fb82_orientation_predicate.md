# `$C1FB82`: three-point orientation / face-side predicate

Classification: **runtime-backed, memory-read-only geometry predicate**. The
observed `$C1FBD4` route tests a triangle's oriented plane against the origin
of its current coordinate space. Its caller is a face-record renderer, making
face culling a well-supported interpretation; the coordinate-space identity
and which winding is front-facing remain unproved.

## Evidence and exact arithmetic

The return-bounded `pcode/raw/no_key_c1fb82_helper/` packet contains 41
instructions / 357 P-code operations with no memory stores. It was captured
at frame 601 from `$C200B8` through return to `$C200BE`; byte-exact source is
`source_amiga/observed/branch_on_d7_bit_pair.asm`,
`test_c1fb8c_relative_record_components.asm`, and
`test_c1fbd4_workspace_component_product.asm`. The caller copies selected
triples from `$C48390` to `$C4BF94` before the call (see
`c2005c_dispatch_target.md`).

When `(D7.w & $0C00) == 0` and `(D7.w & $3000) == 0`, the helper reads signed
word triples `p`, `q`, `r` at `$C4BF94`, `+$06`, `+$0C`. With 68000 arithmetic
widths, its calculation is:

```text
u = wrap16(q - p)                 # componentwise
v = wrap16(r - p)
s = (sign16(A3.w) >> 7) & 7
if s != 0: u = wrap16(u << s); v = wrap16(v << s)
n = asr32(wrap32(u cross v), 8)  # each component independently
t[i] = sign16(n[i].w) * p[i]     # MULS.W reads only low 16 bits of n[i]
a = wrap32(t[2] + t[0])
b = wrap32(a + t[1])
```

The final `BLT` uses the **N xor V flags of the second `ADD.L`**, not a fresh
`TST.L b`. With no overflow these agree with the sign of `b`; the distinction
matters at the arithmetic limits. The scalar triple product is therefore an
interpretation of the ordinary-range computation, not a substitute for the
width-limited machine contract.

The captured triple is `p=(-818,0,478)`, `u=(-539,-143,-471)`, and
`v=(85,-927,-114)`. The three shifted cross-product components are
`(-1642,-397,1999)`; the terms are `(1343156,0,955522)`. Their accumulated
`2298678` exactly matches `build/no_key_c1fb82_helper/trace.jsonl`; this call
returns `D7=1`, `Z=0`.

## Return and alternate routes

On the non-`BLT` route, `MOVEQ #1,D7` returns `D7=1`, `Z=0`. On the `BLT`
route, `CLR.W D7` sets `Z=1` but **preserves the upper word of `D7`**. Thus
`D7` need not be zero on rejection. Both `$C200BE` and the record-walker
caller consume `BEQ`, so the returned **Z flag** is the reliable predicate.

The `(D7.w & $3000) != 0` route at `$C1FB92` is statically reconstructed but
not exercised in this P-code packet. It computes a signed dot product of a
three-component relative position and three record words, then uses the same
`BLT`/`MOVEQ`/`CLR.W` return convention. It also consumes one stream word via
`(A2)+`. The `(D7.w & $0C00) != 0` branch targets `$C1FC3A`; its contract is
not established by this packet. Neither alternate route should be treated as
runtime-validated from this capture.
