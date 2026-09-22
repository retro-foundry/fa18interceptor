# `map_command` entry at `$C1BF8C` (Hunk 5 +`$7314`)

Classification: **behavioural entry, completed transition-helper return, and
static post-helper tail**. The first run003 `M` press becomes raw `$37`,
reaches `$C1BF8C`, and sets `$C4599C` bit 0.

The sealed pre-`M` checkpoint now has a focused no-future-input trace from
`$C1BF8C` to `$C1BFBC`: it returns after 5,619 instructions. The route calls
`$C0F4A6`, reaches `$C17B08` once and `$C4FFB0` four times, then returns via
`$C0F4D4`. The repeated `$C4FFB0` calls are transition/display-page control
work in this interval. It does not execute `$C1D3F4`, `$C1D442`, or `$C1D488`,
so this command helper is not evidence for a terrain-template or static
coastline data decoder.

Authority: `build/run003_m_map_transition_trace/trace.jsonl` (sealed
`build/run003_pre_m_2183/state.bin` plus `build/run003_m_press_only.e9k`).

`initialize_map_transition_state.asm` is a byte-exact static reconstruction
of `$C1BFBC-$C1C051`, the instructions immediately after the call. It sets up
three vector pairs, clears three transition counters, initializes `$C45A94` to
`$1C20`, and sets transition flags. Its names describe directly visible data
movement, not a completed runtime-return claim. P-code for the capped entry is
`pcode/raw/run003_m_key_dispatch/`.
