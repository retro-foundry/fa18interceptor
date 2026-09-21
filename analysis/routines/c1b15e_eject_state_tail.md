# Eject-command state tail at `$C1B15E`

Classification: **structural continuation**. The byte-exact slice
`finish_eject_command_state.asm` covers `$C1B15E-$C1B1A3`, immediately after
the observed `$C1B126` eject-command dispatcher.

It toggles the word at `$C458D8`, chooses an adjacent code at `$C45984`,
derives and stores a scaled long at `$C45918`, clears `$C457A9`, and joins
`$C1BA86`. The surrounding input trace supports the eject-command entry; the
field names remain neutral where their wider role is unproven.
