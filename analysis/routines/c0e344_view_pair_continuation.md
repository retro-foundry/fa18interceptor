# `$C0E344`: post-construction ViewPort continuation

Classification: **callback-scheduled continuation**.

At cold-boot replay frame 5,926, the slot-zero initializer leaves through
`$C160D2/$C160D4`. Single instruction stepping proves its return PC is
`$C0E344`, not `$C1612C` and not the ordinary `$C15D80` outer-update loop:

```text
$C160BC  ADDQ.L #4,SP
$C160BE  MOVE.L $C1821C,$C182BA
$C160C8  MOVE.L $C18232,$C182C2
$C160D2  UNLK A6
$C160D4  RTS
$C0E344  BSR.W $C0E65E
```

The source callback table at `$C55058` contains `$C160BC`, while the live
return continuation is `$C0E344`. This establishes a scheduled callback
relationship rather than a static direct caller edge.

The immediately observed `$C0E344` prefix is:

```text
$C0E344  BSR.W $C0E65E
$C0E348  WaitBOVP($C1822A) through $C53F88
$C0E356  call $C53F30 with $C18218
$C0E364  read the six live pointers from $C1AADC
$C0E380  read the two live words at $C1AAE0/$C1AAE2
$C0E39C  call $C24DB0 with those six pointers and two words
```

If `$C24DB0` returns `$C560`, the continuation performs another `WaitBOVP`,
calls `$C0E78A` with `$21000`, clears a caller-selected 32-word region below
the first raster source at `$C18252`, and continues through the ViewPort
palette/list path. The function's wider semantics remain unassigned.

This is enough to constrain the native port: `game.c` must not initialize the
two flight-page identities at scene entry or from a presentation-frame number.
Their creator belongs to the recovered callback/timeline owner, and its
continuation synchronizes the ViewPort before renderer setup. The exact
callback dispatcher and `$C0E65E/$C24DB0` state contracts remain required
before this path can be scheduled natively.
