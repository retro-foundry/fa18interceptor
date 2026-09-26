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
