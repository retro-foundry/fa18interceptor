# `$C0FA80`: post-input completion callback

Static, byte-exact reconstruction: `source_amiga/observed/complete_post_input_followup.asm`.

A negative `$C45AD6` clears `$C457AE`, sets `$C45795`, and installs `$C10C08`
in `$C1820C`. Runtime use remains unobserved.

`fa18_complete_demo_followup` ports these direct writes as a native followup
pending flag, auxiliary flag, and typed continuation. Its contract test uses
an explicit expired state; no run075 reachability is claimed.
