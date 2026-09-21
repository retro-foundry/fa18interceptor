; Byte-exact selected-record flag check $C207FE-$C2081B.
; The unobserved $C2081C-$C20821 block remains outside this slice.

                org     $C207FE

RECORD_CHECK_STATE_FLAG         equ $C45785
STREAM_RECORD_TABLE             equ $C46184
STREAM_RECORD_INDEX             equ $C458DE

check_c207fe_selected_record_flag:
                tst.b   RECORD_CHECK_STATE_FLAG.l
                bne.b   $C20822
                lea     STREAM_RECORD_TABLE.l,a3
                adda.w  STREAM_RECORD_INDEX.l,a3
                move.b  4(a3),d1
                andi.b  #$40,d1
                beq.b   $C20822
