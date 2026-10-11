; Byte-exact observed adjustment-stage entry $C2DD4E-$C2DD71.
; The bit-7 path and interior arithmetic branches remain raw.

                org     $C2DD4E

CALL_DEPTH_RECORD_GUARD         equ     $C2DE96
CALL_DEPTH_SCALE_LOOKUP         equ     $C2E6DA
CONTINUE_RECORD_ADJUSTMENT      equ     $C2DDB2
RETURN_FLAGGED_RECORD           equ     $C2DE7A

prepare_record_depth_adjustment:
                dc.w    $48E7,$C880 ; movem.l d0-d1/d4/a0,-(a7)
                dc.w    $0829,$0007,$0000 ; btst.b #7,0(a1); retain original EA
                bne.w   RETURN_FLAGGED_RECORD
                move.w  d7,d4
                asr.w   #3,d4
                move.w  d4,d1
                cmpi.w  #$384,d4
                bge.b   $C2DD72
                bsr.w   CALL_DEPTH_RECORD_GUARD
                bsr.w   CALL_DEPTH_SCALE_LOOKUP
                bra.b   CONTINUE_RECORD_ADJUSTMENT
