# `$C33DA4` postflight status-mask gate

Classification: **partially runtime-observed structural/dataflow**. The
run024 frame-23000 continuation executes the mask-test path; alternate branch
behavior remains unobserved.

It tests bits `$4200` of `$C45B50`. If either is set, it clears those bits
using mask `$FFFFBDFF` and sets bit 3 of `$C45B54`; otherwise it returns
unchanged. The status bit meanings remain unproven.
