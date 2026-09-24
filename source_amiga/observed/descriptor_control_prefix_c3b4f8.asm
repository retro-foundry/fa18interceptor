; Byte-exact static descriptor control prefix $C3B4F8-$C3B4FD.
; Observed as the +8 field selected from descriptor $C22708 in map-mode
; traces. C1F6F8 can consume it in a debugger-only probe, but no unmodified
; sampled map continuation reaches it at the walker; preserve raw fields.
                org     $C3B4F8
descriptor_control_prefix_c3b4f8:
                dc.b    $42,$c0,$40,$86,$c0,$14
