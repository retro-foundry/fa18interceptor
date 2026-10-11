"""Compare the linked native output filter with unchanged original functions.

Signed impulses, DC, full-scale alternation, deterministic component vectors
and silence exercise history and truncation. An explicit retained cold-start
case also compares every original recorded sample through the native filter.
"""
import argparse
import array
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import tempfile

from check_original_filter_response import function, pcm, SOURCE

ROOT=Path(__file__).resolve().parents[2]


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--filter',type=Path,required=True)
    parser.add_argument('--out',type=Path,required=True)
    parser.add_argument('--recording-case',type=Path,
                        help='Explicit retained filter-response-cold folder with complete-raw/complete-filtered')
    args=parser.parse_args()
    args.out.mkdir(parents=True,exist_ok=True)
    compiler=shutil.which('gcc')
    assert compiler, 'Original SoftFloat reference needs the GNU compiler'
    report={'native_filter_sha256':sha(args.filter.read_bytes()),
        'source_sha256':sha((SOURCE/'audio.c').read_bytes()),'cases':{}}
    with tempfile.TemporaryDirectory(prefix='pcm-filter-',dir=args.out) as directory:
        work=Path(directory)
        source=(SOURCE/'audio.c').read_text()
        bodies=[function(source,declaration) for declaration in
                ('static int filter (','static float rc_calculate_a0 (')]
        (work/'original_pcm_filter.h').write_text('\n\n'.join(bodies)+'\n')
        reference=work/'reference.exe'
        subprocess.run([compiler,'-O3','-I'+str(work),'-I'+str(SOURCE),'-I'+str(SOURCE/'include'),
            str(ROOT/'tools/native/original_pcm_filter.c'),str(SOURCE/'softfloat/softfloat.c'),
            str(SOURCE/'softfloat/softfloat_fpsp.c'),'-o',str(reference)],check=True,capture_output=True)
        samples=array.array('h');state=0x13579bdf
        for frame in range(32768):
            if frame<512 or frame>=24576: values=(0,0)
            elif frame==512: values=(32767,0)
            elif frame==513: values=(0,-32768)
            elif frame<2048: values=(31000,-31000)
            elif frame<4096: values=(32767,-32768) if frame&1 else (-32768,32767)
            else:
                generated=[]
                for channel in range(2):
                    state=(state*1664525+1013904223)&0xffffffff
                    generated.append((state>>16)-32768)
                values=generated
            samples.extend(values)
        input_path,output_path,reference_path=work/'input.pcm',work/'output.pcm',work/'reference.pcm'
        input_path.write_bytes(samples.tobytes())
        for rate in (44100,48000):
            result=subprocess.run([str(reference),str(input_path),str(reference_path),str(rate),'1','1','--plain-pcm'],
                                  check=True,capture_output=True,text=True)
            expected=reference_path.read_bytes()
            assert expected!=input_path.read_bytes() and any(expected)
            for block in (2048,13,1):
                subprocess.run([str(args.filter.resolve()),str(input_path),str(output_path),str(rate),str(block),'plain'],
                               check=True,capture_output=True)
                assert output_path.read_bytes()==expected,(rate,block)
            for model,led in ((1,0),(2,1),(3,1)):
                subprocess.run([str(reference),str(input_path),str(reference_path),str(rate),str(model),str(led),'--plain-pcm'],
                               check=True,capture_output=True)
                assert reference_path.read_bytes()!=expected,(rate,model,led)
            report['cases'][str(rate)]={'stereo_frames':len(samples)//2,'output_sha256':sha(expected),
                'source_coefficients':result.stdout.strip(),'blocks_identical':[2048,13,1],
                'wrong_led_a1200_fixed_only_rejected':True}
        unsupported=subprocess.run([str(args.filter.resolve()),str(input_path),str(output_path),'48001','13','plain'],
                                   capture_output=True,text=True)
        assert unsupported.returncode and 'Unsupported source-validated' in unsupported.stderr
        report['unsupported_rate_rejected']=True
        if args.recording_case:
            folder=args.recording_case
            raw,filtered=[json.loads((folder/name/'snapshot.json').read_text()) for name in
                          ('complete-raw','complete-filtered')]
            raw_pcm=pcm(folder/'complete-raw',raw)
            actual=pcm(folder/'complete-filtered',filtered)
            assert any(raw_pcm) and any(actual) and len(raw_pcm)==len(actual)
            input_path.write_bytes(raw_pcm)
            rate=raw['recorded_audio']['sample_rate']
            for block in (2048,13):
                subprocess.run([str(args.filter.resolve()),str(input_path),str(output_path),str(rate),str(block),'reference-mix'],
                               check=True,capture_output=True)
                assert output_path.read_bytes()==actual,('complete original recording',block)
            report['original_recording']={'stereo_frames':len(actual)//4,'rate_hz':rate,
                'original_authority':filtered['authority'],'complete_pcm_sha256':sha(actual),
                'blocks_identical':[2048,13],'reference_gain_conversion_test_only':True,
                'samples_shifted_trimmed_excluded':False,
                'scope':'Actual native filter on complete original cold-start PCM input; native game onset/handoff alignment is separate'}
    (args.out/'report.json').write_text(json.dumps(report,indent=2)+'\n')
    print('Native filter matches unchanged original functions at 44100/48000 Hz across every signed component sample and block split')
    if args.recording_case:
        print(f"Every {report['original_recording']['stereo_frames']} original cold-start stereo frame also matches through the native filter")


if __name__=='__main__':
    main()
