# `$C1CCBC`: follow-up reset and record selection prefix

Classification: **static/dataflow prefix, shared flight follow-up**. Both
outcomes of the `$C265E8` parent decision call this helper.

Its entry writes `$01` to `$C45864`, `$04` to `$C4585B`, clears `$C458BD`,
`$C45ABA`, `$C45932`, and `$C459AA`, then reads a word from `$C4E98A +
$C459AA`. A positive word is published as `$C459B4`; the helper doubles its
`<<8` form into `$C459B6` and forms the corresponding `$C46184` 512-byte
record pointer. The subsequent magnitude-preparation phase begins at `$C1CD0E`.

This identifies a common reset/select-record handoff following either flagged
slot outcome, without assigning the selected record a gameplay identity.
Evidence: static bytes `$C1CCBC-$C1CD0D`, parent calls at `$C0F100/$C0F110`,
and the existing byte-exact `$C1CD0E` follow-up source.
