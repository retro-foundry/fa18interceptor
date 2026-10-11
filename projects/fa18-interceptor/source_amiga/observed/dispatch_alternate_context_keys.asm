; Byte-exact alternate-context raw-key dispatch $C1AEE0-$C1AF7B.

                org     $C1AEE0

COMMAND_CONTEXT_STATE          equ $C458AE
CONTEXT_STATE_SPECIAL           equ 6
RAW_KEY_RETURN                  equ $44
RAW_KEY_LEFT                    equ $3E
RAW_KEY_ONE                     equ $1E
RAW_KEY_X                       equ $2D
RAW_KEY_V                       equ $2F
RAW_KEY_Z                       equ $3D
RAW_KEY_TWO                     equ $1D
RAW_KEY_C                       equ $2E
RAW_KEY_RIGHT                   equ $3F
RAW_KEY_THREE                   equ $1F
RAW_KEY_FOUR                    equ $3C
RAW_KEY_RELEASE_ONE             equ $9E
RAW_KEY_RELEASE_X               equ $AD
RAW_KEY_RELEASE_V               equ $AF
RAW_KEY_RELEASE_Z               equ $BD
RAW_KEY_RELEASE_TWO             equ $9D
RAW_KEY_CONTEXT_SPECIAL         equ $43

DISPATCH_RETURN                 equ $C1BB7A
DISPATCH_LEFT                   equ $C1B8F8
DISPATCH_ONE                    equ $C1B914
DISPATCH_X                      equ $C1B930
DISPATCH_V                      equ $C1B960
DISPATCH_Z                      equ $C1B98A
DISPATCH_TWO                    equ $C1B9AC
DISPATCH_C                      equ $C1B7F8
DISPATCH_RIGHT                  equ $C1B780
DISPATCH_THREE                  equ $C1B79A
DISPATCH_FOUR                   equ $C1B7C2
QUEUE_FIRE_REQUEST_ACTION       equ $C1BB66
DISPATCH_CONTEXT_SPECIAL        equ $C1B664
DISPATCH_CONTEXT_DEFAULT        equ $C1AF7C

dispatch_alternate_context_keys:
                move.b  COMMAND_CONTEXT_STATE.l,d3
                beq.b   .normal_context
                cmpi.b  #CONTEXT_STATE_SPECIAL,d3
                beq.w   .special_context
                bra.w   DISPATCH_CONTEXT_DEFAULT
.normal_context:
                cmpi.b  #RAW_KEY_RETURN,d0
                beq.w   DISPATCH_RETURN
                cmpi.b  #RAW_KEY_LEFT,d0
                beq.w   DISPATCH_LEFT
                cmpi.b  #RAW_KEY_ONE,d0
                beq.w   DISPATCH_ONE
                cmpi.b  #RAW_KEY_X,d0
                beq.w   DISPATCH_X
                cmpi.b  #RAW_KEY_V,d0
                beq.w   DISPATCH_V
                cmpi.b  #RAW_KEY_Z,d0
                beq.w   DISPATCH_Z
                cmpi.b  #RAW_KEY_TWO,d0
                beq.w   DISPATCH_TWO
                cmpi.b  #RAW_KEY_C,d0
                beq.w   DISPATCH_C
                cmpi.b  #RAW_KEY_RIGHT,d0
                beq.w   DISPATCH_RIGHT
                cmpi.b  #RAW_KEY_THREE,d0
                beq.w   DISPATCH_THREE
                cmpi.b  #RAW_KEY_FOUR,d0
                beq.w   DISPATCH_FOUR
                cmpi.b  #RAW_KEY_RELEASE_ONE,d0
                beq.w   QUEUE_FIRE_REQUEST_ACTION
                cmpi.b  #RAW_KEY_RELEASE_X,d0
                beq.w   QUEUE_FIRE_REQUEST_ACTION
                cmpi.b  #RAW_KEY_RELEASE_V,d0
                beq.w   QUEUE_FIRE_REQUEST_ACTION
                cmpi.b  #RAW_KEY_RELEASE_Z,d0
                beq.w   QUEUE_FIRE_REQUEST_ACTION
                cmpi.b  #RAW_KEY_RELEASE_TWO,d0
                beq.w   QUEUE_FIRE_REQUEST_ACTION
.special_context:
                cmpi.b  #RAW_KEY_CONTEXT_SPECIAL,d0
                beq.w   DISPATCH_CONTEXT_SPECIAL
