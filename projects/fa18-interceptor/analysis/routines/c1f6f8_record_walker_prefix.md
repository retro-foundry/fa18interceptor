# `$C1F6F8-$C1F79F`: ordinary record-walker control prefix

Classification: **bounded control-stream producer**.

The exact source initializes `$C456E6=$000FFFFF`, clears `$C456EA`, then
walks the stream published by `$C1EF10`. Its ordinary nonnegative branch
either expands three 3-word records from `$C48390`, forwards six words to the
hex handler, or invokes the distinct other-handler branch. A zero child result
uses the current signed relative word; a nonzero result uses the following
signed relative word. `$FFFF` reaches the proved return-zero path.

`record_walker_prefix.{c,h}` ports those branches with required caller-owned
handlers and a caller-supplied step budget. A non-`$FFFF` negative control
word reports the separate `$C1F7A0` selector boundary. The source-cleared
local record-count path at `$C1F716/$C1F844` is outside this prefix's handler
contract, so it remains a caller boundary rather than being inferred. Its
synthetic contract verifies triple expansion, all three handler selections,
relative looping, terminal return, and the external negative route.
