# Record-update class dispatch at `$C23A7E`

Classification: **bounded behavioural dispatch entry**. The no-key and
attract-cockpit traces both call this entry from `$C22C80`. Its byte-exact
source is `source_amiga/observed/dispatch_record_update_class.asm`.

It first rejects a nonzero `$C457AE` event byte, then masks the high nibble of
the current record's `+$62` byte. Observed comparisons route zero to `$C23F4A`
and `$30` to `$C23D82`; `$20` returns through `$C23A7A`. The captured generic
path continues after these gates. Record class meanings remain unassigned.

The traced generic route now has byte-exact source through `$C23BA3`: it
decrements the local `+$4C` word, applies event/flag gates, calls `$C24568`,
and derives a selected 512-byte record pointer from the `+$38` byte when that
selector is valid.

The next traced fragments load the selected record's `+$6C` word, apply the
`+$02/$04/$05` record gates, and select the observed `$1D40` default threshold.
The next measured gates test the pending event and `+$05 == 8`, selecting the
observed non-mode-eight continuation at `$C23D3C`.
Its traced generic successor builds `A3 = $C46184 + ((record +$38 & $7F) << 9)`
and transfers to `$C24056`; the `$FF` selector follows a separate route.
