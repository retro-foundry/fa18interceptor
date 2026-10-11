# run060 `$C304F4` isolated fill

The `$C304F4` breakpoint was captured from the sealed run060 restore at frame
7991 with 220 post breakpoint instructions, Chip snapshots, and one settled
frame. The before and settled Chip images differ in exactly 1,790 bytes, all
in active plane 1. No other active plane changes in this interval.

The deplanarized changed spans are:

```text
94: 145..208     95: 141..218     96: 134..236     97: 127..255
98: 120..273     99: 113..292    100: 107..310    101: 100..319
102: 93..319    103: 86..319     104: 79..319     105: 72..319
106: 65..319    107: 58..319     108: 51..319     109: 44..319
110: 37..319    111: 31..319     112: 24..319     113: 17..319
114: 10..319    115: 3..319
116..144: 0..319
```

The input pair list at `$C4B390` is
`(319,100),(208,93),(145,93),(0,115),(0,179),(319,179)`.
This is the first isolated run060 area-fill output in the current run policy.
The spans provide the exact integer edge and inclusion contract required by
the native fill primitive.

Authority: `build/run060_c304f4_fill_probe/trace.jsonl`, `chip.bin`, and
`settled_chip.bin`.
