# `$C2F490`: renderer-state initializer

Static, byte-exact reconstruction: `source_amiga/observed/initialize_renderer_state_long.asm`.

It writes `$000FFFFF` to `$C456E6` and returns. `$C456E6` is subsequently
read by renderer helpers; the state value's precise role is not yet assigned.
