; Byte-exact observed trampoline into the bounded $C1EE14 indirect stage.
                org $C1ED48
INDIRECT_STAGE equ $C1EE14
branch_to_indirect_stage:
 bra.w INDIRECT_STAGE
