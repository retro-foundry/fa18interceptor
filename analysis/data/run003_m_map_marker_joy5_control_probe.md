# Run003 M-map marker: `J 0 5` control probe

Classification: **negative controlled-input result**.

This probe tests whether the previously documented joystick recording code
`J 0 5` can perturb the black M-map marker from the preserved run003
pre-map state.  It cannot in this particular state and interval, so it must
not be used as player-ownership evidence.

## Reproduction

Starting from `build/run003_pre_m_2183/state.bin`, both streams defer the M
key press until replay frame 101.  The treatment stream additionally holds
`J 0 5` for frames 1--80:

```text
# idle: build/run003_delayed_m_idle.e9k
F 101 K 109 109 16 1

# treatment: build/run003_joy5_then_m.e9k
F 1 J 0 5 1
F 81 J 0 5 0
F 101 K 109 109 16 1
```

Both were replayed for 150 normal frames followed by 24 instruction-traced
frames with `scripts/engine9000_bridge.py`.  Their final display hash is the
same:

```text
1828e0c0717c86ddb8ee3d4afa1ce15b2f0c9b028b8a0201cbae65cdc462f9f3
```

The traces retain two `$C2B93E` entries each.  At their associated
`$C2FA7E` calls with `A5=$C4C598`, both emit the same three source points,
twice:

```text
(31,130), (25,131), (26,132)
```

The pre-trace slow-RAM snapshots are also identical at the relevant observed
control fields: `$C45776=$00CB`, `$C45778=$0048`, and `$C4577C=$0048`.

## Boundary

Earlier run001 evidence establishes that `J 0 5` can affect `$C45778` in its
own flight interval.  This run003 pre-map state does not reproduce that
effect.  Therefore the present probe neither moves the marker nor tests its
relationship to the player; a later state-specific player-motion control is
still required before assigning the marker's owner.
