# Run060 root record to projection packet

Classification: **scenario-backed renderer-transform dataflow**. This is not
yet a proof of player position or orientation.

Authority is the sealed run060 replay, bounded at the direct publisher arm:

```text
python scripts/trace_from_breakpoint.py --restore captures/run060/restored-state.bin --playback captures/run060/playback.e9k --address 0xC1C5E0 --arm-frame 8241 --return-pc 0xC0F036 --frames 8260 --max-instructions 100 --ignore-future-input --output build/run060_frame8241_c1c5e0_projection_packet
```

Recorded input is delivered normally before the breakpoint. The publisher
breakpoint hits on replay frame 8,246, executes 18 instructions, and returns
to `$C0F036`, which is the instruction immediately after `$C0EFD4`'s call to
`$C1C63E`. This places the packet inside the identified repeating update
loop's parent update routine.

## Measured packet

At publisher entry:

```text
A2 = $C46184
D0,D1,D2 = $00000000, $00000500, $00001400
record +$14/+$18/+$1C = $11982C00, $00007708, $1059A000
```

The direct publisher arm performs the byte-exact operations at `$C1C5E0`:

```text
D0 += (long[A2+$14] & $003FFFFF)
D1 +=  long[A2+$18]
D2 += (long[A2+$1C] & $003FFFFF)
D0,D1,D2 = -D0,-D1,-D2
```

At `$C1C636`, immediately before its sole store to `$C45A78`, the measured
unshifted tuple is:

```text
D0,D1,D2 = $FFFFE7D4, $FFFFFF83, $FFFFE64C
```

The routine stores the shifted tuple at `$C45A72-$C45A76` and the complete
shifted middle component `$FFFFFF83` at `$C45A78`. The before/after snapshots
agree with those published values.

## Upstream matrix seed in the same packet

A second trace of the immediately preceding `$C1C54E` path reaches the same
return in 59 instructions. `$C458DE=$0000`, so the path explicitly selects
`A2=$C46184`. Record byte `+$62=$11` chooses the literal signed seed
`(D3,D4,D5)=(0,5,$14)`. The nine signed words at `+$92..+$A2` are:

```text
$92..+$A2 = $4000,$0000,$0000, $0000,$4000,$0000, $0000,$0000,$4000
```

`$C1C54E-$C1C5DF` performs the three dot-product-like sums, right shifts them
by six, adds the resulting components to the root `+$14/+18/+1C` triple, and
stores that intermediate tuple to `$C45A7C`. At `$C1C5E0` it supplies the
measured publisher inputs `(0,$500,$1400)`. This proves that the same root
record supplies both a three-component base triple and a nine-word transform
matrix to the projection path.

## Meaning boundary

This proves that the root record's `+$14/+18/+1C` longwords are live inputs to
a projection-transform publication in the real qualification replay, rather
than immutable terrain data. It does **not** identify them as aircraft world
position, camera position, orientation, or velocity: they are combined with
incoming `D0-D2`, masked on two axes, negated, and sampled root values remain
unchanged across the late-run sampling window. The selected root record is now
known to have both input-control and projection-context roles; its ownership
remains unresolved.

A repeat packet armed after a distinct run060 joystick event (`J 0 4` at
recorded frame 8,392) hits `$C1C5E0` at replay frame 8,395. Its root triple,
incoming `D0-D2`, and published tuple are byte-for-byte the same as this
frame-8,246 packet. This is limited negative evidence: the direct publisher
does not reflect that input event in its values over this interval, so it is
not by itself a moving-aircraft position publisher.

## Late-run writer exclusion

Two CPU-write watches over the full replay interval from frame 8,000 through
8,700 find no write to either root `+$14` (`$C46198`) or the first matrix word
at `+$92` (`$C46216`). In the same replay window, a write watch on `$C45778`
hits immediately at `$C1723E`, inside the already bounded JOY0DAT delta
callback. That callback is the proven per-frame writer of the constrained
control accumulator, whereas this root transform data is stable over the
late-run landing input sequence.

This excludes the sampled root base triple and matrix as the *late-run
per-frame integrated* flight state. It does not exclude their use as a fixed
camera/reference transform or as initialization state outside the watched
window, and it does not locate the actual moving aircraft state.

The next decisive trace is a controlled input-differential packet followed to
the producer of the `$C1C5E0` incoming `D0-D2`, then through camera/render
consumers. That distinguishes a fixed reference origin from a moving aircraft
transform.
