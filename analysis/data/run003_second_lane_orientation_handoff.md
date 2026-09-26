# run003 second control lane to orientation handoff

Classification: **scenario-backed behavioural dataflow**.

The sealed run003 comma hold gives a complete bounded path for packed control
field bits 7:6. The input routine publishes `$80`; `$C1B410` updates root
`+$29` from signed zero to `-1` at frame 5,360. The following `$C1342C` leaf
returns after 165 instructions and converts that lane to a second working
component:

```text
+$29=-1 -> C3D690[+2]=$001B -> target -27
target -27 -> shaped target -18 -> root +$58=-4 ($FFFC)
```

`$C2DEE0` then returns normally after 323 instructions with input
`$0000/$FFFC/$0000` and output `D4/D5/D6=$6D40/$60E8/$0000`. A separate
same-frame trace at `$C2D954` observes the publisher storing those exact words
to root `+$66/+68/+6A`.

This completes the scenario-backed packed-field -> signed lane -> table ->
working input -> transform -> publication contract for the second lane. It
does not establish a player/camera owner or label it as yaw/rudder: the
available evidence proves dataflow and arithmetic only.
