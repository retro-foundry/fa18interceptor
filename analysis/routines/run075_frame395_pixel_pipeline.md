# run075 frame 395 pixel pipeline boundary

Frame 395 is the next visual boundary after the exact frame-392/394 scene
fixture. Relative to frame 394 it changes 29,453 of 64,000 native pixels,
covering `x=0..319, y=1..199`. The oracle contains the expected dark-blue,
gray, and red RGB444 scene colors.

The event-faithful replay reaches the shared `$C2F688` pixel pipeline at the
next Engine execution boundary. The saved entry registers are:

```
D0=$FFFFF1FC  D1=$FFFFF1FC  D2=12  D3=68112
D4=267264     D5=16         D6=1   D7=1
A0=12903112   A1=12867182   A2=12904224
A3=12777414   A4=12777350   A5=90300
```

The signed low words of `D0` and `D1` are both `-25`. `$C2F688` therefore
takes its established `D1.w <= 0` return path for this invocation and does
not write the native visual buffer. The bounded trace then returns through
the selected handler jump. This proves that the first observed call is a
clipped or rejected pixel operation; it does not account for frame 395's
visible output.

The surrounding renderer loop must be traced through all calls at the normal
frame boundary. The existing native `$C2F688-$C2FA6F` primitive is suitable
for those calls once their signed coordinates, table, mode, and plane state
are captured. Do not replace frame 395 with a full image fixture until the
call sequence and state inputs have been checked.

## Subsequent calls

Skipping the first breakpoint hit and replaying from the same canonical
restore captures the following entries. The debugger reports the next Engine
frame for each entry, so these are one boundary later than the corresponding
normal replay execution point.

| entry | `D0.w` | `D1.w` | `D2` | `D3` | `D4` | `D5` | `D6` | `D7` |
| ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| 1 | 158 | 167 | 5 | `$FFFF0FF0` | `$41C00` | 32 | 2 | 24 |
| 2 | 157 | 168 | 0 | 0 | 262146 | 2 | 3 | 2 |
| 3 | 159 | 168 | `$FFFB` | `$FFFB` | 262148 | 4 | 4 | 4 |
| 4 | 158 | 168 | `$FFFE` | `$FFFE` | 262145 | 1 | 1 | 1 |
| 5 | 158 | 167 | `$FFFD` | `$FFFD` | 262146 | 2 | 2 | 2 |

The first coordinate pair `(158,167)` is the established run060 primary
table sample. The later calls are nearby edge pixels with distinct lane
masks. The complete numeric records are retained in
`build/run075_frame395_c2f688_hit0` through `hit5`; the next step is to
collect the full call count and identify the caller's scene record before
encoding these as a native draw list.

The inventory was extended through hit 30. It shows three distinct screen
regions in the same frame construction:

* cockpit or horizon edge calls around `(156,156)` and `(158,167)`, with
  small lane masks;
* vertical calls around `(159..161,129..132)` and `(159,71..75)`;
* outer scene calls around `(293..303,156..159)`, with wider masks and larger
  packed plane words.

Representative later entries are `(159,132, mode inputs 1/3)`, `(159,71,
0x3FFF)`, and `(293,156, 0x00000003/0x00000000)`. Each individual hit is
  preserved in `build/run075_frame395_c2f688_hit16` through `hit30`. These
records establish that frame 395 is assembled by several draw families;
they do not yet identify the source scene structs or the complete caller
loop.

The first captured call has stack pointer `$C55034`; the saved return address
at that stack location is `$C31708`. The shared body uses the established
renderer tables `$C2F766`, `$C2F786`, and `$C2F7C6`. `$C31708` is therefore the
next caller boundary for static disassembly and source reconstruction of the
frame-395 scene loop.

The address falls inside the existing byte-exact `$C316C0-$C31721` submission
tail documented in
[`c316c0_postflight_renderer_submission.md`](c316c0_postflight_renderer_submission.md).
That routine adds the vertical offset to `D1`, appends the `(D0,D1)` pair,
selects `$C2F5F4` or `$C2F60A` from `D7` bit 0, and loops to `$C31410`. This
connects the frame-395 pixel calls to the reconstructed postflight record
walker. Its table contents and gameplay meaning remain unproven, so the
native port still needs a semantic postflight record struct and a run075
entry fixture before this path can replace the frame fixture.
