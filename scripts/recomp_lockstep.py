"""Compare native fa18_recomp FA18_TRACE output with an Engine9000 trace.jsonl.

usage: recomp_lockstep.py TRACE.jsonl COUNT fa18_recomp.exe ARGS... (first divergence)"""
import json, subprocess, sys, os
trace, n = sys.argv[1], int(sys.argv[2])
env = dict(os.environ, FA18_TRACE=str(n))
cmd = sys.argv[3:]
p = subprocess.run(cmd, env=env, capture_output=True, text=True)
native = [l.split() for l in p.stderr.splitlines() if l.startswith('T ')]
names = [f'd{i}' for i in range(8)] + [f'a{i}' for i in range(8)]
with open(trace) as f:
    for i, (line, nat) in enumerate(zip(f, native)):
        ref = json.loads(line)
        pc = int(nat[1], 16)
        regs = [int(x, 16) for x in nat[2:18]]
        sr = int(nat[18], 16)
        bad = []
        if pc != ref['pc']: bad.append(f"pc {pc:06X} vs {ref['pc']:06X}")
        for k, v in zip(names, regs):
            if v != ref['registers'][k]: bad.append(f"{k} {v:08X} vs {ref['registers'][k]:08X}")
        if (sr & 0xFF1F) != (ref['registers']['sr'] & 0xFF1F): bad.append(f"sr {sr:04X} vs {ref['registers']['sr']:04X}")
        if bad:
            print(f"diverge at {i} frame {ref['frame']}: {ref['asm']} @ {ref['pc']:06X} ; native v={nat[19]} h={nat[20]}")
            print('  ' + '; '.join(bad))
            j = max(0, i - 6)
            print('  native prev:', [' '.join(x[1:2] + x[21:]) for x in native[j:i+1]])
            break
    else:
        print('no divergence in', min(len(native), i + 1), 'instructions')
