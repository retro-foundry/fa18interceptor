; Byte-exact generic record-update prehelper route $C23B16-$C23B2B.

                org     $C23B16

RUN_RECORD_UPDATE_PREHELPER      equ $C24568

run_record_update_prehelper:
                dc.w    $0829,$0000,$0002       ; btst.b #0,$02(a1)
                bne.w   $C23CA6
                tst.w   $2C(a1)
                blt.w   $C23CA6
                bsr.w   RUN_RECORD_UPDATE_PREHELPER
