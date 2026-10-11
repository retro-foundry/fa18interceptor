; Byte-exact observed activity-accumulator gate $C25312-$C2532D.
; A shared helper supplies a signed activity value; negative values leave via
; the distant route, while bit 8 of the activity flag selects the clear path.

                org     $C25312

ACTIVITY_PREPARE_HELPER         equ     $C16D04
ACTIVITY_VALUE                  equ     $C45B02
ACTIVITY_FLAGS                  equ     $C458CE

gate_activity_accumulator_update:
                jsr     ACTIVITY_PREPARE_HELPER
                move.l  ACTIVITY_VALUE,d1
                blt.w   $C253A4
                move.w  ACTIVITY_FLAGS,d0
                andi.w  #$100,d0
                beq.b   $C25346
