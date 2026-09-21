; Byte-exact raw-$44 command slice $C1BB7A-$C1BC11.
; Sealed run003 Return event reaches the default weapon-cycle route.

                org     $C1BB7A

COMMAND_CONTEXT_MODE           equ $C458A6
COMMAND_CONTEXT_MODE_TWO       equ 2
COMMAND_CONTEXT_MODE_SPECIAL   equ $7D
COMMAND_CONTEXT_STATE          equ $C4579A
CONTROL_COMMAND_FLAGS          equ $C46200
COMMAND_REQUEST_FLAGS          equ $C4599A
WEAPON_SELECTION_NIBBLE        equ $C461E7
WEAPON_SELECTION_REQUEST_A     equ $C45843
WEAPON_SELECTION_REQUEST_B     equ $C45844
WEAPON_SELECTION_STATE         equ $C458B4
SPECIAL_RECORD_FLAGS           equ $C46186
SPECIAL_RECORD_BIT             equ 3
WEAPON_REQUEST_BIT             equ 4
WEAPON_NIBBLE_MASK             equ $F0
WEAPON_LOW_NIBBLE_MASK         equ $0F
WEAPON_NIBBLE_STEP             equ $10
WEAPON_NIBBLE_WRAP_VALUE       equ $30
WEAPON_REQUEST_VALUE           equ 3
SPECIAL_COMMAND_ACTION          equ $401F

COMMAND_SIDE_EFFECT            equ $C33186
PUBLISH_COMMAND_ACTION         equ $C25704
SHARED_COMMAND_FALLBACK        equ $C1C23C

cycle_weapon_selection:
                cmpi.b  #COMMAND_CONTEXT_MODE_TWO,COMMAND_CONTEXT_MODE.l
                beq.b   .mode_two
                cmpi.b  #COMMAND_CONTEXT_MODE_SPECIAL,COMMAND_CONTEXT_MODE.l
                bne.b   .default_route
                tst.b   COMMAND_CONTEXT_STATE.l
                bge.b   .special_action
.mode_two:
                jsr     COMMAND_SIDE_EFFECT.l
                bset.b  #SPECIAL_RECORD_BIT,SPECIAL_RECORD_FLAGS.l
                bra.w   SHARED_COMMAND_FALLBACK
.special_action:
                swap    d0
                move.w  #SPECIAL_COMMAND_ACTION,d0
                jsr     PUBLISH_COMMAND_ACTION.l
                swap    d0
                bra.w   SHARED_COMMAND_FALLBACK
.default_route:
                jsr     COMMAND_SIDE_EFFECT.l
                move.b  CONTROL_COMMAND_FLAGS.l,d4
                andi.b  #WEAPON_LOW_NIBBLE_MASK,d4
                bne.w   SHARED_COMMAND_FALLBACK
                bset.b  #WEAPON_REQUEST_BIT,COMMAND_REQUEST_FLAGS.l
                move.b  WEAPON_SELECTION_NIBBLE.l,d4
                andi.b  #WEAPON_NIBBLE_MASK,d4
                subi.b  #WEAPON_NIBBLE_STEP,d4
                bge.b   .store_selection_nibble
                move.b  #WEAPON_NIBBLE_WRAP_VALUE,d4
.store_selection_nibble:
                andi.b  #WEAPON_LOW_NIBBLE_MASK,WEAPON_SELECTION_NIBBLE.l
                or.b    d4,WEAPON_SELECTION_NIBBLE.l
                move.b  #WEAPON_REQUEST_VALUE,WEAPON_SELECTION_REQUEST_A.l
                move.b  #WEAPON_REQUEST_VALUE,WEAPON_SELECTION_REQUEST_B.l
                clr.b   WEAPON_SELECTION_STATE.l
                bra.w   SHARED_COMMAND_FALLBACK
