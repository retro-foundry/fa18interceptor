# run029 ordinary-frame Custom-register log

## Authority

`build/run029_normal_custom_994/` replays sealed run029 through frame 994
without instruction stepping, with `--normal-custom-log`.  The log contains
218,348 Custom-register writes.  Its final video hash is
`805e2099a9177cc01a57067d41aafbd053d62f17ae0f8d8a2222d042f85fdb3e`, equal
to the separately captured normal frame-994 snapshot.

This is distinct from `trace.jsonl`: it records authentic full-frame timing,
but not an instruction row for every CPU operation.

## Numeric-boundary blit records

At hardware frame 993, the log records a working-family submission from
`$C30E40`:

| PC | BLTSIZE | A | B | C/D |
| --- | --- | --- | --- | --- |
| `$C30E40` | `$01C2` | `$0129FC` | `$012A6C` | `$01A6E0` |

The frame also has three `$C30D1C` submissions and four line submissions into
the `$014...-$01A...` family.  These are not Copper-visible run029 planes.

At frame 994, the packet triggers at `$C2FDF0`, `$C2FE3A`, `$C2FE90`, and
`$C2FEDA`, with `BLTSIZE=$2414` and C/D respectively `$053918`, `$0519D8`,
`$04FA98`, and `$04DB58`.  Those are the visible plane-4 through plane-1
subset.  The following two `$C306AE` line jobs use `$04DB7F` and `$04DB58`.

The log therefore confirms the working/visible cadence without the altered
scheduling of instruction stepping.  It still does not identify the formatter
which prepared a KTS or FT glyph: blitter source tags do not expose the
asynchronous Chip-RAM write stream, and the broad visible packet is compatible
with buffered presentation work.
