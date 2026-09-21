# Space and mode-command continuation at `$C1B21C`

Classification: **structural continuation**. The byte-exact slice
`dispatch_space_and_mode_commands.asm` covers `$C1B21C-$C1B263`.

The first route gates the established `$C0833E` Space-command helper on the
observed input-state byte and returns through `$C1C23C`. Adjacent entries call
`$C08394` or cycle `$C459C4` from one through three, set bit 15, write two
state bytes, and use the same fallback. The field names remain neutral because
the wider command semantics are not yet isolated.
