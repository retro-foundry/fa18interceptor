# `map_command` entry at `$C1BF8C` (Hunk 5 +`$7314`)

Classification: **behavioural entry plus static post-helper tail**. The first
run003 `M` press becomes raw `$37`, reaches `$C1BF8C`, and sets `$C4599C` bit
0. In the observed state it calls `$C0F4A6`; both the dispatcher and separately
armed helper packets exceed their instruction caps before that call returns.

`initialize_map_transition_state.asm` is a byte-exact static reconstruction
of `$C1BFBC-$C1C051`, the instructions immediately after the call. It sets up
three vector pairs, clears three transition counters, initializes `$C45A94` to
`$1C20`, and sets transition flags. Its names describe directly visible data
movement, not a completed runtime-return claim. P-code for the capped entry is
`pcode/raw/run003_m_key_dispatch/`.
