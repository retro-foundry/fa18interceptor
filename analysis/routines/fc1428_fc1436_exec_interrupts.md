# Kickstart 1.3 Exec Disable and Enable at `$FC1428` and `$FC1436`

The local Kickstart 1.3 `LVO.OFFS` identifies Exec `Disable()` at `-$78(A6)`
and `Enable()` at `-$7E(A6)`. The observed RAM vectors at `$C001FE` and
`$C001F8` are byte-exact jumps to `$FC1428` and `$FC1436`, respectively.
The pinned ROM contains these instructions:

```text
$FC1428  MOVE.W #$4000,$DFF09A
$FC1430  ADDQ.B #1,$126(A6)
$FC1434  RTS
$FC1436  SUBQ.B #1,$126(A6)
$FC143A  BGE.B $FC1444
$FC143C  MOVE.W #$C000,$DFF09A
$FC1444  RTS
```

`$126(A6)` is the ExecBase interrupt disable nesting byte. The C operations
in `port/os/exec.c` wrap that byte exactly. The bridge in
`port/os/exec_glue.c` executes each source instruction at its own boundary,
including opcode and data bus accesses, condition flags, cycle charges,
the conditional interrupt enable write, and return stack effects. The runner
checks all 30 ROM bytes before enabling the bridge. It is the default for
the translated runner with the pinned ROM; `--no-os-exec-interrupts` selects
ROM execution for comparison. `--no-recomp` defaults to the ROM path.

The original transition inventory counted 40,560 entries to each ROM leaf
across the three sealed native recordings. A 300-frame demo comparison
between the C path and explicit ROM execution produced identical RAM and
registers; the ROM path entered each leaf 312 times and the C path entered
neither. Full C-path trials of demo01, qual_carrier_success, and
qual_fail_crashes produced the sealed final RAM hash for each recording and
no entries to either ROM leaf. The default-on full gate and frame parity
also passed: 386 routines, 726,979 matching shadow calls and 925,873 matching
sandbox calls across three recordings, zero mismatches, identical poison
frames, and 10/10 exact parity frames 393-402. GNU and MSVC Release produced
identical RAM in the focused 300-frame C-path run. A separate 300-frame
interpreter-only run entered each ROM leaf 300 times with both the default and
explicit ROM settings, confirming that baseline remains on the ROM path.
