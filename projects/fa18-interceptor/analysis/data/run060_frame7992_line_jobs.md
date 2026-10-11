# run060 frame 7992 area blit jobs

The four `$C30F5A` entries captured in frame 7992 build the same typed area
blit shape. `BLTCON1=$0000`, so this is an area operation; the earlier line
classification was incorrect.

```text
BLTCON0 = $0FCE   BLTCON1 = $0000
BLTAFWM = $FFFF   BLTALWM = $FFFF
A       = $00A230 BLTSIZE = $0312
BLTAMOD = 1       BLTBMOD = 1
BLTCMOD = 5       BLTDMOD = 5
```

The live custom image confirms that the first packet has the same setup even
though the breakpoint is before its register writes. The three subsequent
images read directly at the repeated preparation entries all contain this
same control, mask, modulo, and size tuple.

The per job B and C/D pointers are:

```text
B=$00B038 C/D=$019E4A
B=$00AE88 C/D=$017F0A
B=$00ACD8 C/D=$015FCA
B=$00AB28 C/D=$01408A
```

Using the captured active page table
`[$018980,$016A40,$014B00,$012BC0]`, the C/D destinations decode as:

| job | semantic plane | page byte offset | word x | row |
|---:|---:|---:|---:|---:|
| 0 | 3 | `$0FCA` | 1 | 101 |
| 1 | 2 | `$14CA` | 1 | 133 |
| 2 | 1 | `$14CA` | 1 | 133 |
| 3 | 0 | `$14CA` | 1 | 133 |

The word x values are obtained from the byte offset by dividing by two; the
remaining byte offset is zero for all four jobs. The source pointers are
temporary table data and remain address-free in the native representation.

At the four packet entries, the caller tables advance together:

```text
A1: $C309AA, $C309AE, $C309B2, $C309B6
A2: $C45682, $C45686, $C4568A, $C4568E
A3: $C1AB5C, $C1AB58, $C1AB54, $C1AB50
```

The first source record at A3 contains pointer words
`$00B038/$00A210/$008280/$008B18`; subsequent entries shift through the
same object slot chain. These are source asset/table references, not screen
coordinates. `$C53F44` is the observed `WaitBlit()` library wrapper, so the
native implementation should consume the resolved asset words and submit a
synchronous line operation at the semantic page boundary.

`FA18AreaBlitJob` and `fa18_build_run060_frame7992_area_jobs` preserve these
values as a semantic packet. The packet builder is the current native boundary
for this frame. The conversion from the source asset words and row strides to
the resulting four-plane pixels remains the next frame-7992 task.
