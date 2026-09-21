# `$C0D730`: display-buffer preparation gate

The cockpit trace calls this routine from `$C0F05C`. It tests bit 13 of `$C458D2`; clear selects `$C2FD8C`, whose four blits target the Copper-visible cockpit planes. Set selects `$C0DA38`, not observed in this packet. The name denotes only this dispatch role.
