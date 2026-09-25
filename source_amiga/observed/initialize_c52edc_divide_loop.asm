; Byte-exact observed long-division loop setup $C52EDC-$C52EEB.

                org     $C52EDC

C52EEC                         equ     $C52EEC

initialize_c52edc_divide_loop:
                ; Preserve the captured immediate-shift opcode forms.
                dc.w    $4282,$761f,$e380,$e392
                dc.w    $b481,$6504,$9481,$5280
