; Byte-exact four-call context helper $C0F4A6-$C0F4D5.

                org     $C0F4A6

CONTEXT_HELPER                  equ $C17B08

invoke_context_helper_four_times:
                clr.l   -(a7)
                jsr     CONTEXT_HELPER.l
                addq.l  #4,a7
                moveq   #1,d0
                move.l  d0,-(a7)
                jsr     CONTEXT_HELPER.l
                addq.l  #4,a7
                moveq   #2,d0
                move.l  d0,-(a7)
                jsr     CONTEXT_HELPER.l
                addq.l  #4,a7
                moveq   #3,d0
                move.l  d0,-(a7)
                jsr     CONTEXT_HELPER.l
                addq.l  #4,a7
                rts
