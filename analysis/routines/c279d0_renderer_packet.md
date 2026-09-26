# `$C279D0` renderer packet

Classification: **scenario-backed renderer traversal**, not flight-model
integration.

Authority is the sealed run060 qualification replay:

```text
python scripts/trace_from_breakpoint.py --restore captures/run060/restored-state.bin --playback captures/run060/playback.e9k --address 0xC279D0 --arm-frame 8241 --return-pc 0xC0F0C8 --frames 8260 --max-instructions 5000 --ignore-future-input --output build/run060_frame8241_c279d0_renderer_packet
```

The breakpoint hits at replay frame 8,241 and returns to its caller at
`$C0F0C8` after 4,877 instructions. The caller is the `$C0F090` conditional
stage in the parent update routine, but caller placement alone is not a
subsystem identity.

## Observed renderer contract

- The prologue writes four renderer state words at `$C456E6`, sets renderer
  selector `$C45954=$0003`, and branches by byte `$C4586B`.
- The executed traversal loop at `$C27B20-$C27C40` runs 96 times, reading
  three-word record terms from `A3` and selecting byte values from `$C27D24`.
- The packet uses matrix cache `$C45BD8` at `$C27C66`.
- It calls `$C2FF48` three times; those calls lead into renderer line/blit
  helpers (`$C301F6`, `$C2FA7E`, `$C2F5F4`, `$C2F60A`).
- The run060 entry context includes `A3=$C4B9C8`, a mutable renderer/workspace
  region; no player-record identity is implied.

Therefore `$C279D0` is a renderer packet selected from the parent update
sequence, not a flight-state integrator. Its older source name used
"flight-helper" solely because of call-site location and has been corrected.

This removes `$C279D0` from the flight-model search. The remaining direct
candidate path is the JOY0DAT accumulator consumers and whichever persistent
state those paths update before world placement or landing evaluation.
