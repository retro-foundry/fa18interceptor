# `pause_command` entry at `$C1C06E` (Hunk 5 +`$73F6`)

Classification: **behavioural entry, capped downstream path**. The first
run003 `P` press becomes raw Amiga key `$19` and branches from `$C1B030` to
this entry. With `$C458AE` non-negative and `$C457AE` zero in the observed
state, it saves the raw key and calls `$C0F4A6`.

The dispatcher packet reaches its 1,000-instruction cap while in that helper.
A separate no-future-input packet armed at `$C0F4A6` also reaches its
3,000-instruction cap before `$C1C09A`. These packets are retained as
`pcode/raw/run003_p_key_dispatch/` and
`pcode/raw/run003_p_pause_helper/`. They prove the key mapping and entry path,
but do not prove the helper's complete pause/resume contract.
