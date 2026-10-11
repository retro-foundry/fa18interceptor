# `$C3985A`: observed projected-edge list candidate

Classification: **scenario-backed data layout**. This identifies a stable
edge-list instance, not yet the owning model or artwork.

At the frame-602 `$C212B0` breakpoint, `A2=$C3985A`. The first word is the
routine's selector `$000A`; it is followed by ten endpoint-offset pairs:

| Segment | First offset | Second offset |
| ---: | ---: | ---: |
| 0 | `$0156` | `$015C` |
| 1 | `$015C` | `$0168` |
| 2 | `$0168` | `$016E` |
| 3 | `$016E` | `$0174` |
| 4 | `$0174` | `$0162` |
| 5 | `$017A` | `$0180` |
| 6 | `$0180` | `$0186` |
| 7 | `$0186` | `$0198` |
| 8 | `$0198` | `$0192` |
| 9 | `$0192` | `$818C` |

The high bit on the final second offset is the `$C212B0` end-of-list marker;
its usable offset is `$018C`. These offsets select 3-word endpoint records at
`$C48390`, making this a ten-segment projected-edge list under selector `$A`.

The exact 42-byte sequence is identical in the baseline menu snapshot, the
frame-602 display trace, and run029 frame 1,093. That stability makes it a
static data candidate, but it is not sufficient to name the model: `$C48390`
itself is runtime data and the caller that chooses this list remains to be
traced.
