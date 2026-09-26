# Run060 root record cockpit-altitude formatter

Classification: **scenario-backed cockpit altitude dataflow**.

Authority is the sealed run060 replay, traced at the selected-record formatter:

```text
python scripts/trace_replay_breakpoint.py --restore captures/run060/restored-state.bin --playback captures/run060/playback.e9k --address 0xC3201A --arm-frame 8241 --frames 8260 --instructions 300 --output build/run060_frame8241_c3201a_altitude_formatter_trace
```

The formatter hits at replay frame 8,244. Its live selected base is
`A0=$C46184`, agreeing with the nearby run060 root-selection and projection
packets.

## Measured conversion

At `$C32044`, the routine loads selected-record `+$18`:

```text
$C46184 + $18 = $00007708
$C32048  arithmetic_shift_right 7  -> $000000EE
$C3204A  arithmetic_shift_right 3  -> $0000001D (29)
$C3204C..$C32052  multiply by 5    -> $00000091 (145)
```

The same byte-exact formatter has an alternate submission that appends literal
`FT`. This proves that selected-record `+$18` is the cockpit altitude source
in this run060 interval, and its measured value is **145 FT**.

## Boundary

This identifies one physical component of the selected root record: vertical
altitude. It does not identify `+$14` or `+$1C` as horizontal coordinates,
does not identify the `+$92..+$A2` matrix as player orientation, and does not
prove the root record contains all player state. Those require independent
world-placement/camera consumer evidence.
