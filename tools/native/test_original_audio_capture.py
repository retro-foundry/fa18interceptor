"""Exercise the real original capture caller and reject incomplete/corrupt PCM."""
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile

from check_original_audio_capture import validate

ROOT = Path(__file__).resolve().parents[2]


def main():
    with tempfile.TemporaryDirectory(prefix='original-pcm-',dir=ROOT/'build') as temporary:
        work = Path(temporary)
        base = [sys.executable,str(ROOT/'scripts/engine9000_bridge.py'),
                '--frames','32','--restore',str(ROOT/'captures/native/demo01/state.bin')]
        for name, extra in (('baseline',[]),('capture',['--wav'])):
            result = subprocess.run([*base,'--output',str(work/name),*extra],cwd=ROOT,
                                    capture_output=True,text=True,timeout=20)
            assert result.returncode == 0, result.stderr
        result = validate(work/'capture',work/'baseline')
        assert result['replay_calls'] == result['chunks'] == 32
        assert result['recorded_audio']['sample_frames'] == 32*882
        for change in ('drop','offset','pcm'):
            damaged = work/change
            shutil.copytree(work/'capture',damaged)
            log = damaged/'audio_chunks.jsonl'
            rows = log.read_text().splitlines()
            if change == 'drop':
                del rows[10]
            elif change == 'offset':
                row = json.loads(rows[0]);row['first_sample'] += 1;rows[0]=json.dumps(row)
            else:
                wav = damaged/'original.wav'
                data = bytearray(wav.read_bytes());data[44] ^= 1;wav.write_bytes(data)
            log.write_text('\n'.join(rows)+'\n')
            try:
                validate(damaged,work/'baseline')
            except AssertionError:
                pass
            else:
                raise AssertionError(f'Accepted damaged original PCM: {change}')
        for name, extra, message in (
            ('stepping',['--wav','--trace-frames','1'],'ordinary full-frame'),
            ('budget',['--wav','--capture-budget-mib','8'],'exceeds capture budget')):
            result = subprocess.run([*base,'--output',str(work/name),*extra],cwd=ROOT,
                                    capture_output=True,text=True,timeout=20)
            assert result.returncode and message in result.stderr, result.stderr
            assert not (work/name/'original.wav').exists()
    print('Real 32-frame original PCM preserves complete state; missing chunks, shifted offsets, changed PCM, stepping and budget failures rejected')


if __name__ == '__main__':
    try:
        main()
    finally:
        subprocess.run([sys.executable,str(ROOT/'scripts/prune_build_artifacts.py'),'--quiet'],cwd=ROOT,check=True)
