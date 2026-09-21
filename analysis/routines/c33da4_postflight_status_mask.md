# `$C33DA4` postflight status-mask gate

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

It tests bits `$4200` of `$C45B50`. If either is set, it clears those bits
using mask `$FFFFBDFF` and sets bit 3 of `$C45B54`; otherwise it returns
unchanged. The status bit meanings remain unproven.
