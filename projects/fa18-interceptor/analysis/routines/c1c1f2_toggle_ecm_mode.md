# `toggle_ecm_mode` at `$C1C1F2` (Hunk 5 +`$757A`)

Classification: **behavioural**. The first run003 `J` press becomes raw Amiga
key `$26` and enters this slice. Its `$C33186` child returns to `$C1C20E` in
1,924 instructions with no future input; P-code is
`pcode/raw/run003_j_command_child/`.

The handler sets `$C4599B` bit 3, writes mode 3 at `$C45840`, and toggles the
byte at `$C458B5` between zero and one before joining the common queue. `J` is
therefore the observed ECM toggle. A separately bounded raw `$27` (`K`) trace
matches no command branch and only queues the event.
