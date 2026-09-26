# run060 frame 7992 line jobs

The four `$C30F5A` entries captured in frame 7992 build the same typed line
job shape:

```text
BLTCON0 = $0FCE   BLTCON1 = $0000
BLTAFWM = $FFFF   BLTALWM = $FFFF
A       = $00A230 BLTSIZE = $0312
```

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

`FA18LineBlitJob` and `fa18_build_run060_frame7992_line_jobs` preserve these
values as a semantic packet. The packet builder is the current native boundary
for this frame. The conversion from these source-table pointers to screen
segments and the resulting four-plane pixels remains the next frame-7992 task.
