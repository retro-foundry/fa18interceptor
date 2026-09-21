; Byte-exact runtime-backed entry $C2F5F4-$C2F609 for the renderer helper.
                org $C2F5F4
RENDER_POINTERS equ $C456B6
enter_renderer_table_helper:
 movea.l RENDER_POINTERS.l,a1
 lea $C2F766.l,a3
 lea $C2F786.l,a4
 bra.w $C2F688
