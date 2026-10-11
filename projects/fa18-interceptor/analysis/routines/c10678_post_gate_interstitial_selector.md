# `$C10678`: post-gate interstitial selector

Classification: **scenario-backed text-selection dataflow**.  This callback is
installed by the run024 `$C10228` equality branch.

At frame 750, the bounded qualification prefix reaches `$C10688` with
`$C4582B=0`, `$C458A6=9`, and `$C45796=0`.  The static `$03..$08` range test
therefore rejects mode 9, and the observed path:

```text
$C10702  clear $C45796
$C1070C  write selector $0047 (71) at $C4574A
$C10718  clear terminator word $C4574C
$C10720  install $C1072E at $C1820C
```

Selector 71 resolves through `$C3ED0A` to descriptor `$C3EFB4`; its payload is
`CRACKED BY A-HA`.  This is an embedded crack/credit text record, not evidence
for a qualification state or game mission briefing.  The next callback
`$C1072E` remains unclassified.

At frame 790, its counter-success branch `$C10738` replaces the first selector
with code 4, clears `$C457C3`, sets `$C457E0=3`, and installs `$C1075A`.
Code 4 resolves to `$C3EFDE`, whose payload is whitespace only.  This is an
observed credit-clear transition; it does not establish the screen effect or
the subsequent callback's gameplay purpose.  The exact callback is reconstructed
in `source_amiga/observed/advance_interstitial_credit_sequence.asm`.
