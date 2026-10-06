# Native collision effects and resets

The playable caller is `fa18_native` -> `native_frontend_tick` ->
`native_flight_tick` -> `native_records_update` -> `dynamics` -> the shared
C25B66 record-dynamics owner. The previously unconnected collision-sound child
now calls existing C17F8C `start_sound_6(0x1c, 0x30)`, matching the arguments at
C260EC-C260F8. Its source voice gate retains the FIRE_STATE and C45797 effects.
The C06C02 fault child calls the original release-build empty `fault_hook`.
No instruction/CPU/chipset adapter enters these paths. Sample output is still
unimplemented; native sample-voice pointers remain unloaded.

`check_collision.py` uses the sealed crash recording by main-loop iteration,
after ordinary native intro/menu startup. At iteration 2022, 160 record updates
have executed and no reset has occurred. At iteration 2150, 288 record updates
have executed and two source postflight resets have completed; the aircraft is
grounded again with zero speed. This prefix previously stopped at the missing
collision-sound child. No capture supplies native runtime state.

At both checkpoints, actual original C12098/C1C63E bodies match every non-stack
RAM byte. Six focused C17F8C/C06C02 cases also match non-stack RAM and demonstrate
that the carried gameplay values remain unchanged in the connected voice-gate
contract (disabled sound or enabled sound with an unloaded voice). The future
loaded-sample branch and its carried return values are outside this proof.

This milestone is complete (100% of collision effects and the bounded reset
scope). It does not establish full crash-recording outcome or frame parity.
The one extended native run next reaches missing C0F920, the sequence reset to
the main menu. Source menu/flight cadence, full recorded outcomes and other
flight/render/HUD children remain open. No full reference replay suite was
repeated; Copper fade remains excluded.
