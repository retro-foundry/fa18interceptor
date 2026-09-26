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

## Complete bounded entry set

The repeated-breakpoint collector reaches 32 `$C2F688` entries before the
postflight path leaves this call family. The screen coordinates and `D7`
selection values are:

```text
156,156,00000001  158,167,00000018  157,168,00000002  159,168,00000004
158,168,00000001  158,167,00000002  156,156,00000002  158,167,00000018
156,156,00000001  158,167,00000018  160,129,00000002  159,129,00008000
160,130,00000001  159,130,00008000  160,131,00000001  159,131,00008000
159,132,00000001  161,132,00000003  159,071,0000C000  159,072,00000001
159,074,00000001  159,075,00000001  293,156,0005403D  295,156,00050C00
298,156,00050300  300,156,00050060  298,159,00050018  300,159,00050060
293,159,00050018  295,159,00050C00  303,159,00050300  305,159,00050003
```

The final entries split into the same three spatial families identified in
the earlier inventory. This is sufficient to define a bounded native
`FA18PostflightRecord` fixture for this call family, but the complete frame
still includes other rendering work; this record set alone must not unlock
frame 395.

## Shared renderer bridge

The first shared-renderer entry `$C2F5F4` has `(D0.w,D1.w)=(158,167)` and
reads the renderer mode as `D2.w=5` from `$C45954`. It selects the primary
mask table `$C2F766` and handler table `$C2F786`. The entry carries
`D4=$00041C00`, `D5=32`, `D6=2`, and `D7=24`; the table lookup selects handler
index 2 before the shared body at `$C2F688`. This is the first frame-395
record with a complete native renderer-state bridge. The remaining lane
fields and all adjacent entries still need the same bounded capture before
the record fixture can drive pixels.

The adjacent renderer entry `$C2F60A` is also reached in frame 395. Its first
captured invocation is clipped at `(-25,-25)`, but the next valid invocation
has `(D0.w,D1.w)=(156,156)`, mode `D2.w=10`, alternate mask table `$C2F7C6`,
handler table `$C2F786`, and `D4=$40002`, `D5=2`, `D6=2`, `D7=2`. This is a
two-row renderer input and matches the native `FA18_PIXEL_TWO_ROWS` contract.

## Enclosing walker entry

The frame-395 replay reaches `$C31392` at the normal boundary. Its entry
state includes `D0=-1`, `D1=21`, `D2=30`, `D3=60`, `D4=263170`,
`D5=6922`, `D6=6912`, and `D7=348198`. The prefix selects the 40-byte table
region at `$C4E71C`, observes `$C4566C=0`, clears the renderer mode at
`$C45954`, and starts with a ten-entry limit. It then loads the scene record
stream from `$C4E2BC` and applies the fixed-point normalization before the
record guards.

The bounded trace later reaches the adjacent renderer call at instruction
144. Before that branch it writes renderer mode `13` and normalizes the first
record's source values into the screen coordinates captured above. This
establishes the semantic boundary for a future `FA18PostflightScene` struct:
table selection, record cursor/limit, normalization state, and renderer mode.
The original table addresses remain implementation evidence only.

The 1,200-instruction continuation confirms the walker order. After the
initial clipped call, it submits shared records `(158,167)`, `(157,168)`,
`(159,168)`, and `(158,168)`, then submits adjacent `(156,156)`, followed by
shared `(158,167)`. The shared calls use modes `5`, `0`, `0`, `0`, and `9`;
the adjacent call uses mode `10`. The loop calls `$C31312` between record
groups. A later pass repeats the first scene group and reaches `$C332BC` and
`$C332FE`, which are the next continuation helpers after this postflight
record family.

## Continuation component `$C332BC`

Frame 395 reaches `$C332BC` with `D4=$40002`, `D5=2`, `D6=3`, and `D7=2`.
The helper writes `$FFFFF` to the renderer lane mask, calls `$C332FE`, then
computes a second component from the stored scene offsets: `x=160` plus
`$C45988`, and `y=129` plus `$C458D8`. It sets renderer mode `8` and submits
the component through `$C2F60A` followed by `$C2F5F4`. The bounded trace then
reaches the same primary/alternate pixel tables with the component's lane
state. This should become a separate semantic component in the native scene
model; it must not be folded into the 32-record postflight fixture.

The `$C332FE` helper itself is a bounded screen component constructor. It
accepts the stored horizontal and vertical offsets, clamps the horizontal
coordinate to `1..$13D`, adds the fixed vertical base `$81` plus
`$C458D8`, sets mode `8`, and submits `(160,129)` through `$C2F60A` and
`$C2F5F4`. Its later table work converts the resulting handler word into
lane masks, including the `$C456E7` per-plane suppression checks. The native
component struct therefore needs horizontal/vertical offsets, clamp bounds,
renderer mode, and lane suppression state.

`$C31312` is the corresponding group constructor. In frame 395 it computes
`x=157` from base `$9D` plus `$C45988`, computes `y=168` from base `$A8` plus
`$C458D8`, sets renderer mode `12`, and submits through `$C2F5F4`. It then
continues to the next shared records. The native scene model should represent
this as a parameterized record group rather than another raw memory region.

## Dense line raster path

The same frame-395 walker reaches `$C2FA7E`, which accounts for the dense
scene pixels that the 32 `$C2F688` entries cannot explain. The first captured
line submission has endpoints `(175,93)` and `(163,93)` with source state
`D4=$FD18005D`, `D5=$7B6`, `D6=$60`, and `D7=$D14`. The routine copies the
current plane mode, advances both rows, and enters its established line-mode
stepper. This is an evidence-backed `FA18LineSegment` candidate; plane
selection and the remaining line list still require collection before frame
395 can use the native line renderer.

The first eight line entries are:

```text
(175,93)->(163,93)  (163,93)->(164,94)
(164,94)->(177,94)  (177,94)->(176,93)
(176,93)->(163,93)  (194,93)->(182,93)
(182,93)->(183,93)  (183,93)->(196,93)
```

The bounded entries alternate source states `$FD18005D`, `$FFD8005E`,
`$FB1C005D`, and repeat line-mode values `D5=1720/1847/1974`, `D6=96`.
The observed list is enough to start a native line submission fixture, but
the complete line count and plane mapping remain open.

The frame-395 saved Slow-RAM snapshot resolves part of that state:
`$C456E7=$FF` enables all four line planes, `$C45954=$000A` supplies the
line mode, and `$C45984=$0090` supplies the row limit. The native line style
can therefore use `active_plane_mask=0x0F` for this call family. The source
plane bit value and control-plane value still need to be recovered from the
line setup writes before assigning the remaining `FA18LineStyle` fields.

The frame-395 submission tail `$C2FB7A` supplies the remaining computed line
packet for the first segment: `D0=-1`, `D1=85`, `D2=$FFF8`, `D3=0`,
`D4=$FD18005D`, `D5=$FFD0`, `D6=$FB00`, and `D7=$0EC5`. It writes the same
line control and destination address for each enabled bit of `$C456E7`, with
`BLTADAT=$8000`; the subsequent per-plane paths adjust the modulo word and
destination pointer. These are hardware packet values, not native palette
indices, so they belong in the line adapter's derived state rather than in
`FA18LineStyle.plane_bits` until the chunky color mapping is proven.
