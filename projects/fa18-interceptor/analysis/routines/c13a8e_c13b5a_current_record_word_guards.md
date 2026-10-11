# Current-record word guards at `$C13A8E` and `$C13B5A`

Classification: **behavioural state-field guards**.  Both helpers are called
from repeatedly observed update-stage sites (`$C136A8/$C136B6` and `$C13972`).
They use the active indexed-record pointer at `$C18210`, which `$C149BE`
publishes from selector `$C459B4`.

`$C13A8E-$C13AB5` reads the current record word at `$66`, stores pointers to
that word and its `$56` companion in locals, and returns immediately when the
`$66` value is non-positive.  Its positive continuation is raw.

`$C13B5A-$C13B7D` similarly pairs `$6A` with `$5A`.  When `$6A` is zero, the
observed `$C13B96-$C13B9F` continuation clears `$5A` and returns; its nonzero
continuation is raw.  The record fields' gameplay names remain unassigned.
