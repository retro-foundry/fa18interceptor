# run060 active root flight-update chain

Classification: **scenario-backed call/dataflow chain**. This identifies the
executed path that updates the active root flight pose and orientation in the
qualification replay. It does not claim every invocation of these generic
record stages belongs to the player/root record.

The long context captured before the first root `+$66` angle write shows this
executed route inside the ordinary parent update:

```text
$C0EFD4 parent update
  -> $C1C63E update stage
    -> $C22C80 record-update stage
      -> $C25B66 indexed record stage
        -> $C13D84 selects/publishes root $C46184
        -> $C2D408 matrix/update packet
          -> $C2D94E publishes root +$66/+68/+6A
            -> $C2E514 writes root +$92..+$A2
```

The same `$C25B66` instance reaches the root horizontal pair publication at
`$C25E6E/$C25E72`; the `$C13D84` continuation commits root altitude through
`$C14D32`. Thus this is the current bounded update chain for all identified
root flight-pose fields:

```text
root +$14/+18/+1C        moving pose tuple
root +$66/+68/+6A        orientation-angle tuple
root +$92..+$A2          composed orientation matrix
```

Authority:
`build/run060_frame00948_root_angle_long_context/memory_writes.json`,
`data/run060_root_pose_integrator.md`, and
`data/run060_root_attitude_matrix.md`.
