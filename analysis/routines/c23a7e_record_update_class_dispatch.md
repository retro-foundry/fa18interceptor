# Record-update class dispatch at `$C23A7E`

Classification: **bounded behavioural dispatch entry**. The no-key and
attract-cockpit traces both call this entry from `$C22C80`. Its byte-exact
source is `source_amiga/observed/dispatch_record_update_class.asm`.

It first rejects a nonzero `$C457AE` event byte, then masks the high nibble of
the current record's `+$62` byte. Observed comparisons route zero to `$C23F4A`
and `$30` to `$C23D82`; `$20` returns through `$C23A7A`. The captured generic
path continues after these gates. Record class meanings remain unassigned.
