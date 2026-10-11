; Byte-exact alternate renderer entry $C2F626-$C2F639.
                org $C2F626
RENDER_POINTERS equ $C456B6
enter_alternate_renderer_table_helper:
 movea.l RENDER_POINTERS.l,a1
 lea $C2F7C6.l,a3
 lea $C2F786.l,a4
 bra.b $C2F688
