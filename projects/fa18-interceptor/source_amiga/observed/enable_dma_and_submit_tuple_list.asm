; Byte-exact $C2FF48-$C2FF57 display-submission wrapper (Hunk 36 +$AF8).
; The bounded run001 call writes DMACON then delegates list submission.

                org     $C2FF48

CUSTOM_DMACON                 equ $DFF096
DMACON_SET_BITPLANE_DMA       equ $8400
SUBMIT_BOUNDED_TUPLE_LIST     equ $C301F6
SUBMISSION_RETURN              equ $C2FF46

enable_dma_and_submit_tuple_list:
                nop
                move.w  #DMACON_SET_BITPLANE_DMA,CUSTOM_DMACON.l
                bsr.w   SUBMIT_BOUNDED_TUPLE_LIST
                bne.s   SUBMISSION_RETURN