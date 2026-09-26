# run060 `$C3201A` changed-value Chip RAM delta

Authority: `build/run060_frame0976_c3201a_chip_trace/`, replay arm at
`$C3201A`, 1,700 instruction steps, and one settled frame.

This is the changed-value companion to the frame-8244 cache trace. The
formatter reaches the redraw path at replay frame 978. Relative to the Chip
RAM snapshot at the breakpoint, the bounded trace changes four bytes:

| Chip offset | Before | Trace result | Settled result |
| ---: | ---: | ---: | ---: |
| `$018339` | `$84` | `$04` | `$04` |
| `$018360` | `$3A` | `$3B` | `$3B` |
| `$018388` | `$0A` | `$20` | `$20` |
| `$0185B3` | `$40` | `$00` | `$40` |

The first three bytes are in the active plane range `$016A40-$01897F`, so
they are plane 3 in the captured display page. Their plane-relative offsets
are `$18EF9`, `$1920`, and `$1948`. The final byte is transient asynchronous
blitter state because it returns to `$40` after one settled frame.

This establishes a run060+ pixel oracle for a changed numeric submission and
confirms that this invocation writes one active plane. It does not establish
that every numeric field uses one plane, nor does it identify the final
coordinate table interpretation by itself.

## Compositor join

The same changed trace enters `$C32806` with the normal `$C3201A` layout. The
first glyph destination is `$01830E`, which is active plane 3 base `$016A40`
plus native offset `$18CE`. `$C32806` reconstructs shift `0` from the packed
mask word and derives five rows from the captured `D7` value. The row writes
therefore use offsets `$18CE`, `$18F6`, `$191E`, `$1946`, and `$196E` within
the selected plane, matching the observed 40-byte stride. The three
persistent changed bytes fall inside those row writes; `$0185B3` is outside
the settled result and remains asynchronous state.
