; Byte-exact strided record-word decrement gate $C22C80-$C22CCD.
; Record ownership remains structural.

                org     $C22C80

RECORD_STRIDE_STATE_FLAG        equ $C457AE
RECORD_STRIDE_BASE              equ $C48188

decrement_c22c80_record_stride_words:
                move.l  d5,-(sp)
                tst.b   RECORD_STRIDE_STATE_FLAG.l
                bne.b   $C22CCE
                lea     RECORD_STRIDE_BASE.l,a0
                subq.w  #1,(a0)
                subq.w  #1,$20(a0)
                subq.w  #1,$40(a0)
                subq.w  #1,$60(a0)
                subq.w  #1,$80(a0)
                subq.w  #1,$A0(a0)
                subq.w  #1,$C0(a0)
                subq.w  #1,$E0(a0)
                subq.w  #1,$100(a0)
                subq.w  #1,$120(a0)
                subq.w  #1,$140(a0)
                subq.w  #1,$160(a0)
                subq.w  #1,$180(a0)
                subq.w  #1,$1A0(a0)
                subq.w  #1,$1C0(a0)
                subq.w  #1,$1E0(a0)
