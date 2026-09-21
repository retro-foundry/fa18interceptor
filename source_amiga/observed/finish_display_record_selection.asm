; Byte-exact success/reject returns $C0DA70-$C0DA9F for the $C0D7E0 selector.
                org $C0DA70
DISPLAY_SELECTION_WORD_A equ $C456E6
DISPLAY_SELECTION_WORD_B equ $C456E8
DISPLAY_SELECTION_LONG equ $C456EA
DISPLAY_SELECTION_FLAG equ $C4589E
finish_display_record_selection:
 move.w #1,DISPLAY_SELECTION_WORD_A.l
 move.w #1,DISPLAY_SELECTION_WORD_B.l
 clr.l DISPLAY_SELECTION_LONG.l
 move.b #1,DISPLAY_SELECTION_FLAG.l
 moveq #0,d0
 unlk a6
 rts
reject_display_record_selection:
 clr.b DISPLAY_SELECTION_FLAG.l
 moveq #1,d0
 unlk a6
 rts
