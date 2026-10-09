"""Check every cold-start PCM sample against the original filter functions.

The controlled recordings differ only in sound_filter=off/emulated, with
floppy clicks muted at 100 in both. No shift, trimming or normalization.
This is original reference-path validation, not native audio acceptance.
"""
import argparse
import array
from collections import deque
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import tempfile
import wave

from check_original_audio_capture import recorded_bytes

ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / 'tools/engine9000-src/ami9000/sources/src'


def sha(data):
    return hashlib.sha256(data).hexdigest()


def function(source, declaration):
    start = source.index(declaration)
    begin = source.index('{', start)
    depth, end = 1, begin + 1
    while depth:
        if source[end] == '{': depth += 1
        elif source[end] == '}': depth -= 1
        end += 1
    return source[start:end]


def pcm(folder, report):
    descriptor = report['recorded_audio']
    path = folder / descriptor['file']
    assert sha(path.read_bytes()) == descriptor['wav_sha256']
    with wave.open(str(path)) as wav:
        assert (wav.getnchannels(), wav.getsampwidth(), wav.getframerate()) == (2, 2, descriptor['sample_rate'])
        data = wav.readframes(wav.getnframes())
        assert len(data) == descriptor['sample_frames'] * 4
    covered, calls = 0, []
    for line in (folder / 'audio_chunks.jsonl').read_text().splitlines():
        row = json.loads(line)
        assert row['first_sample'] == covered and row['frames'] > 0
        end = covered + row['frames']
        assert sha(data[4*covered:4*end]) == row['pcm_sha256']
        covered = end
        calls.append(row['call'])
    assert calls == list(range(descriptor['first_replay_call'], descriptor['first_replay_call'] + descriptor['replay_calls']))
    assert covered == descriptor['sample_frames'] and sha(data) == descriptor['pcm_sha256']
    return data


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--raw', type=Path, required=True)
    parser.add_argument('--filtered', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    raw, filtered = [json.loads((path/'snapshot.json').read_text()) for path in (args.raw,args.filtered)]
    for report, mode in ((raw,'off'), (filtered,'emulated')):
        assert report['authority']['initial_state_sha256'] is None, 'Cold-start history required'
        assert report['core_options']['puae_sound_filter'] == mode
        assert report['core_options']['puae_sound_filter_type'] == 'auto'
        assert report['core_options']['puae_sound_interpol'] == 'anti'
        assert report['core_options']['puae_sound_stereo_separation'] == '100%'
        assert report['core_options']['puae_floppy_sound'] == '100', 'Mute click samples; original mixing gain remains'
    def execution(report):
        return {key: ({k:v for k,v in value.items() if k != 'config_sha256'} if key == 'authority' else
                      {k:v for k,v in value.items() if k != 'puae_sound_filter'} if key == 'core_options' else value)
                for key,value in report.items() if key not in ('recorded_audio','audio_sha256','wall_seconds')}
    assert execution(raw) == execution(filtered), 'Filter choice changed original execution'
    for name in ('chip.bin','slow.bin','state.bin'):
        assert recorded_bytes(args.raw,name) == recorded_bytes(args.filtered,name), name
    assert (args.raw/'screen.png').read_bytes() == (args.filtered/'screen.png').read_bytes()
    raw_pcm, filtered_pcm = pcm(args.raw,raw), pcm(args.filtered,filtered)
    assert len(raw_pcm) == len(filtered_pcm) and any(raw_pcm) and any(filtered_pcm), 'Non-silent complete recordings required'
    source = (SOURCE/'audio.c').read_text()
    bodies = [function(source,name) for name in ('static int filter (','static float rc_calculate_a0 (')]
    compiler = shutil.which('gcc')
    assert compiler, 'GNU compiler required for the pinned original floating-point reference'
    args.out.parent.mkdir(parents=True,exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='original-filter-',dir=args.out.parent) as directory:
        work = Path(directory)
        (work/'original_pcm_filter.h').write_text('\n\n'.join(bodies)+'\n')
        executable = work/'filter.exe'
        subprocess.run([compiler,'-O3','-I'+str(work),'-I'+str(SOURCE),'-I'+str(SOURCE/'include'),
            str(ROOT/'tools/native/original_pcm_filter.c'),str(SOURCE/'softfloat/softfloat.c'),
            str(SOURCE/'softfloat/softfloat_fpsp.c'),'-o',str(executable)],check=True,capture_output=True)
        input_path, output_path = work/'input.pcm',work/'output.pcm'
        input_path.write_bytes(raw_pcm)
        rate = raw['recorded_audio']['sample_rate']
        result = subprocess.run([str(executable),str(input_path),str(output_path),str(rate),'1','1'],
                                check=True,capture_output=True,text=True)
        expected = output_path.read_bytes()
        assert len(expected) == len(filtered_pcm)
        a,b = array.array('h',filtered_pcm),array.array('h',expected)
        differences, first_differences, last_differences = 0, [], deque(maxlen=16)
        for i,(x,y) in enumerate(zip(a,b)):
            if x != y:
                differences += 1
                if len(first_differences)<16: first_differences.append((i,x-y))
                last_differences.append((i,x-y))
        report = {'scope':'Every cold-start original PCM sample versus unchanged A500/LED-on filter functions and original SoftFloat; native acceptance remains open',
            'raw_capture':str(args.raw),'filtered_capture':str(args.filtered),
            'authority':filtered['authority'],'source_sha256':sha((SOURCE/'audio.c').read_bytes()),
            'softfloat_sources_sha256':{name:sha((SOURCE/name).read_bytes()) for name in
                ('softfloat/softfloat.c','softfloat/softfloat_fpsp.c','softfloat/softfloat.h','softfloat/softfloat_fpsp_tables.h')},
            'compiler':subprocess.run([compiler,'--version'],capture_output=True,text=True,check=True).stdout.splitlines()[0],
            'reference_executable_sha256':sha(executable.read_bytes()),
            'filter_coefficients':result.stdout.strip(),'rate_hz':rate,'stereo_frames':len(a)//2,
            'nonzero_filtered_samples':sum(x!=0 for x in a),'different_samples':differences,
            'first_differences':first_differences,'last_differences':list(last_differences),
            'actual_pcm_sha256':sha(filtered_pcm),'expected_pcm_sha256':sha(expected),
            'complete_original_execution_unchanged':True,'samples_shifted_trimmed_normalized':False}
        # Retain a report of any rejected result, including startup differences.
        args.out.write_text(json.dumps(report,indent=2)+'\n')
        assert not differences, f"Original filter response differs in {differences} samples; see {args.out}"
        for model,led in ((1,0),(2,1),(3,1)):
            subprocess.run([str(executable),str(input_path),str(output_path),str(rate),str(model),str(led)],
                           check=True,capture_output=True)
            assert output_path.read_bytes() != filtered_pcm, (model,led)
        report['wrong_led_a1200_fixed_only_responses_rejected'] = True
        args.out.write_text(json.dumps(report,indent=2)+'\n')
    print(f"Every {report['stereo_frames']} cold-start stereo frame matches original A500/LED filter; native acceptance remains open")


if __name__ == '__main__':
    main()
