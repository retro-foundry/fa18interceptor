"""Seal a closed human recording, preserving raw input and the exact initial state."""
import argparse
import ctypes as C
import hashlib
import json
from pathlib import Path

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('run', type=Path)
a=p.parse_args()
m=json.loads((a.run/'run.json').read_text())
kernel=C.windll.kernel32
kernel.OpenProcess.restype=C.c_void_p
kernel.OpenProcess.argtypes=[C.c_uint32,C.c_bool,C.c_uint32]
kernel.GetExitCodeProcess.argtypes=[C.c_void_p,C.POINTER(C.c_uint32)]
kernel.CloseHandle.argtypes=[C.c_void_p]
handle=kernel.OpenProcess(0x1000,False,m['pid'])
if handle:
    code=C.c_uint32()
    ok=kernel.GetExitCodeProcess(handle,C.byref(code))
    kernel.CloseHandle(handle)
    if ok and code.value==259:
        raise RuntimeError('Engine9000 is still recording. Close its window before finalizing.')
raw=(a.run/'inputs.e9k').read_bytes()
lines=raw[m['prelude_bytes']:].decode('ascii').splitlines()
previous=0
events=0
playback_lines=[]
for line in lines:
    parts=line.split()
    if not parts or parts[0]!='F':
        raise ValueError(f'Invalid recording line: {line}')
    # Engine9000 records a terminal close marker as "F <frame> C" when the
    # emulator window is closed normally.  It is not an input event.
    if len(parts) == 3 and parts[2] == 'C':
        continue
    frame=int(parts[1])
    if frame<previous:
        raise ValueError('Frame numbers moved backwards: replay contains a restore/rewind and must be segmented explicitly.')
    previous=frame
    events+=1
    playback_lines.append(line)
if not events:
    raise ValueError('No human input events recorded after setup')
playback='E9K_INPUT_V1\n'+'\n'.join(playback_lines)+'\n'
(a.run/'playback.e9k').write_text(playback,encoding='ascii')
m.update(status='sealed',event_count=events,last_input_frame=previous,
         hashes={f:hashlib.sha256((a.run/f).read_bytes()).hexdigest()
                 for f in ['initial_state.bin','config.uae','inputs.e9k','playback.e9k','toolchain.json']})
(a.run/'run.json').write_text(json.dumps(m,indent=2)+'\n')
print(json.dumps({'run':str(a.run),'events':events,'last_input_frame':previous},indent=2))
