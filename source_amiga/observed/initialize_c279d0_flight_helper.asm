; Byte-exact observed prologue of parent flight-update helper $C279D0-$C279F9.
; The fall-through after the final BEQ was not exercised by this capture and
; remains outside this bounded source slice.

                org     $C279D0

RENDERER_STATE_WORDS            equ     $C456E6
RENDERER_SELECTOR               equ     $C45954
FLIGHT_UPDATE_MODE_BYTE         equ     $C4586B
FLIGHT_UPDATE_DEPTH_PATH        equ     $C27A0C

initialize_c279d0_flight_helper:
                link.w  a6,#-$24
                move.w  #4,d0
                move.w  #0,d1
                clr.w   d2
                move.w  #-1,d3
                movem.w d0-d3,RENDERER_STATE_WORDS.l
                move.w  #3,RENDERER_SELECTOR.l
                move.b  FLIGHT_UPDATE_MODE_BYTE.l,d3
                beq.b   FLIGHT_UPDATE_DEPTH_PATH
