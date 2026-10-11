; Byte-exact $C1715C-$C1718D input-state sampler.
; Observed during the bounded run004 Space-release input phase.

                org     $C1715C

INPUT_STATUS_BYTE              equ $C1839A
JOYDAT                         equ $DFF016

INPUT_STATUS_BIT_6              equ 6
JOYDAT_BIT_10                   equ 10
INPUT_STATE_FLAG_1              equ 1

sample_input_state:
                link.w  a6,#-2
                movem.l d7,-(a7)
                moveq   #0,d7
                move.b  INPUT_STATUS_BYTE.l,d0
                btst    #INPUT_STATUS_BIT_6,d0
                bne.s   .read_joydat
                moveq   #INPUT_STATE_FLAG_1,d7
.read_joydat:
                move.w  JOYDAT.l,d0
                btst    #JOYDAT_BIT_10,d0
                bne.s   .return
                bset    #1,d7
.return:
                move.l  d7,d0
                movem.l (a7)+,d7
                unlk    a6
                rts
