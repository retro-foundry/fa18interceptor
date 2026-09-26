# run060 frame 7992 successor capture

Frame 7992 was captured immediately after frame 7991 from the same restored
checkpoint and input replay. The Chip RAM snapshots differ by 1,431 bytes.
The changed display ranges cover all four semantic planes and rows 37--144,
so frame 7992 is a new renderer output and cannot reuse the frame 7991 fill
operation.

The DMA records show four line mode submissions, at vertical positions 66,
74, 82, and 90. Each writes `$0FCE` to `BLTCON0`, uses the captured line
control state, and a common A pointer `$00A230`. The C/D pointer low words
are:

| vpos | C/D related pointer values |
|---:|---|
| 66 | `$019E4A`, `$019E4A` |
| 74 | `$00AE88`, `$017F0A` |
| 82 | `$00ACD8`, `$015FCA` |
| 90 | `$00AB28`, `$01408A` |

The frame-7992 Chip snapshots are reproduced by
`scripts/capture_chip_frames.py`. Their SHA-256 values are:

```text
7991  4fe8b1e796a44145415f666bc2c12e8adf23f5839ef51a881513ae16f33ac5fb
7992  7133ea3408cdfe27e45de2f8c9e775c809d6a13fe1edca405274a31b62216008
```

The next native task is to decode these four line packets and match frame
7992's four-plane delta. No later frame is being used as a port target yet.
