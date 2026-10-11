; Byte-exact base-record transform and packed output $C24170-$C241A5.

                org     $C24170

RUN_BASE_RECORD_TRANSFORM         equ $C091E0

run_base_record_update_transform:
                moveq   #0,d3
                moveq   #0,d4
                move.w  #-$24,d5
                exg     a1,a3
                jsr     RUN_BASE_RECORD_TRANSFORM.l
                exg     a1,a3
                move.l  d0,d3
                move.l  d2,d4
                asr.l   #8,d3
                asr.l   #8,d4
                andi.w  #$3FFF,d3
                andi.w  #$3FFF,d4
                swap    d0
                swap    d2
                asr.w   #6,d0
                asr.w   #6,d2
                movem.w d0/d2-d4,$2C(a1)
                asr.l   #8,d1
                move.l  d1,$34(a1)
