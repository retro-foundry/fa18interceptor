# `$C1B410` bounded three-axis control update

Classification: **structural with byte-exact source**. The routine decodes
three two-bit direction fields from `$65(A1)` and updates signed bytes at
`$28(A1)`, `$29(A1)`, and `$2A(A1)`. The first two axes change by 1 and clamp
to `$EC..$14`; the third changes by 3 and clamps to `$C4..$3C`.

The axis names describe independent state slots and bounded arithmetic. Their
physical flight-control binding requires a frame-bounded caller trace.

## run060 third-lane instance

The sealed run060 frame-1754 packet enters with root `+$65=$09`, so bits 3:2
are `$08`. It takes the third-lane decrement path and writes root signed
`+$2A: $00 -> $FD` (0 to -3); `+$28` and `+$29` remain zero in this packet.
This directly confirms the later `J 0 6` route's bounded three-unit effect on
the third control lane. Its downstream transform or motion consumer is still
unassigned.

## run003 second-lane instance

The sealed run003 comma hold provides the corresponding live instance for the
otherwise unexercised second lane.  The raw `$38` key dispatcher has already
published `$80` in active root record `$C46184 + $65`.  A bounded replay trace
at absolute frame 5,360 then enters this routine with `A1=$C46184` and takes
the `$C0` field's decrement route.  It writes:

```text
root +$28: $00 -> $00
root +$29: $00 -> $FF   (signed 0 -> -1)
root +$2A: $00 -> $00
```

The trace contains 23 instructions and ends at the local RTS.  This proves
that comma's `$80` command feeds the second bounded lane; it does **not** prove
that this lane is an aircraft rudder/yaw axis.  The `rudder` wording in the
raw-key helper is retained as its observed input-command label only.

## run060 initial-turn instance

The sealed run060 replay enters this exact routine at frame 949 with
`A1=$C46184`. Root control byte `+$65` is `$21`; its first two-bit field
(`$30`) is `$20`, the routine's decrement code. The bounded 22-instruction
packet writes root `+$28` from `$FF` (signed −1) to `$FE` (signed −2).
The `$C0` and `$0C` fields are zero, so it writes zero to both `+$29` and
`+$2A`.

The immediately following `$C1342C` packet consumes that `+$28 = −2` lane to
select the table-driven target for the first run060 pitch-like transform
update. This proves the data dependency from record control bits to the
numeric orientation path, while leaving the origin and real-world meaning of
`+$65` unassigned.
