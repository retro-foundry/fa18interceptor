"""Compare every initial music request in a validated separate original launch.

Matching uses original sample bytes and ordered requests, not relocated pointer
values, a searched subsequence or a shifted WAV. Timing differences remain
reported; this does not accept complete native/original recorded sound.
"""
import argparse
import array
import gzip
import hashlib
import json
from pathlib import Path
import wave

from check_native_audio_trace import read_audio_trace
from check_original_audio_events import validate_events
from check_original_audio_capture import recorded_bytes


def digest(data):
    return hashlib.sha256(data).hexdigest()


def requests(folder,layout):
    banks=[recorded_bytes(folder,name) for name in ('chip.bin','slow.bin')]
    registers=[{} for _ in range(4)]
    pending=[None]*4
    result=[[] for _ in range(4)]
    for line in (folder/'audio_events.jsonl').open():
        row=json.loads(line)
        if row.get('kind')!='write' or not 0xdff0a0<=row['address']<=0xdff0da:
            continue
        channel,register=divmod(row['address']-0xdff0a0,16)
        registers[channel][register]=row['value']
        offset=row['source']-layout['hunk_76']
        if offset==44:
            assert register==4 and pending[channel] is None
            pointer=(registers[channel][0]<<16|registers[channel][2])&~1
            size=2*(row['value'] or 65536)
            bank,at=(banks[0],pointer) if pointer<0x80000 else (banks[1],pointer-0xc00000)
            payload=bank[at:at+size]
            assert at>=0 and len(payload)==size
            pending[channel]={'call':row['call'],'hardware_frame':row['hardware_frame'],
                'vpos':row['vpos'],'hpos':row['hpos'],'samples':pointer,
                'bytes':size,'sha256':digest(payload)}
        elif offset==308 and pending[channel] is not None:
            assert register==8
            pending[channel].update(period=registers[channel][6],volume=row['value'])
            result[channel].append(pending[channel]);pending[channel]=None
    assert all(item is None for item in pending)
    return result


def compare(original,native):
    for channel,events in enumerate(original):
        assert len(native[channel])>=len(events)
        for index,event in enumerate(events):
            actual=native[channel][index]
            assert all(event[field]==actual[field] for field in ('bytes','sha256','period','volume')),(channel,index)


def onset(path):
    with wave.open(str(path)) as wav:
        rate=wav.getframerate()
        samples=array.array('h',wav.readframes(wav.getnframes()))
    first=[next((i//2 for i,value in enumerate(samples) if i%2==channel and value),None)
           for channel in (0,1)]
    assert all(value is not None for value in first)
    return {'rate_hz':rate,'first_nonzero_stereo_frame_by_channel':first,
            'right_minus_left_frames':first[1]-first[0]}


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--original',type=Path,required=True)
    parser.add_argument('--baseline',type=Path,required=True)
    parser.add_argument('--native-trace',type=Path,required=True)
    parser.add_argument('--native-report',type=Path,required=True)
    parser.add_argument('--pcm-reference',type=Path,
                        help='Explicit reusable original WAV; still checked against every original PCM block/hash')
    parser.add_argument('--out',type=Path,required=True)
    args=parser.parse_args()
    validation=validate_events(args.original,args.baseline,args.pcm_reference)
    report=json.loads(args.native_report.read_text())
    trace=gzip.decompress(args.native_trace.read_bytes()) if args.native_trace.suffix=='.gz' else args.native_trace.read_bytes()
    assert digest(trace)==report['complete_audio_trace_sha256']
    read_audio_trace(args.native_trace,report['stats'])
    native=[[] for _ in range(4)]
    for line in trace.splitlines():
        row=json.loads(line)
        if row.get('kind')=='request' and row['active']:
            native[row['channel']].append(row)
    original=requests(args.original,validation['resolved_voice_layout'])
    assert sum(map(len,original))==validation['source_writes'][f"{validation['resolved_voice_layout']['hunk_76']+44:06X}"]
    assert all(original[channel] for channel in (0,1)) and not original[2] and not original[3]
    compare(original,native)
    for field,value in (('bytes',2),('sha256','0'*64),('period',124),('volume',0)):
        damaged=[[dict(event) for event in channel] for channel in original]
        damaged[0][2][field]=value
        try: compare(damaged,native)
        except AssertionError: pass
        else: raise AssertionError(f'Accepted changed {field}')
    timing={}
    for channel in (0,1):
        events=original[channel]
        # Report alternative beam-line interpretations. This is diagnostic
        # inference about the reference recording, not a native clock change.
        residuals={}
        for lines in (312,313,314):
            values=[]
            for index in range(2,len(events)-1):
                before,after=events[index:index+2]
                assert before['period']==after['period']==358
                cycles=((after['hardware_frame']-before['hardware_frame'])*lines+
                        after['vpos']-before['vpos'])*227+after['hpos']-before['hpos']
                values.append(cycles-events[index-1]['bytes']*358)
            residuals[str(lines)]={'intervals':len(values),'min_cycles':min(values),'max_cycles':max(values)}
        native_error=[]
        for index in range(2,len(events)):
            expected=sum(event['bytes']*358 for event in events[:index-1])
            actual=native[channel][index]['sample_frame']*3546895/48000
            native_error.append(actual-expected)
        timing[str(channel)]={'beam_line_candidates':residuals,
            'native_refill_rounding_cycles':{'min':min(native_error),'max':max(native_error)},
            'original_initial_and_prime':events[:2],
            'native_initial_and_prime_output_frames':[native[channel][i]['sample_frame'] for i in (0,1)]}
    result={'scope':'Every original initial-music request in the separate 5000-call launch versus native ordered startup requests; onset and reference-clock differences reported, not accepted as identical WAVs',
        'original_validation':validation,'native_runner_sha256':report['runner_sha256'],
        'native_trace_sha256':digest(trace),'requests_by_channel':[len(channel) for channel in original],
        'all_original_requests_match_ordered_native_payload_period_volume':True,
        'comparison_search_shift_trim_or_gain_conversion':False,
        'wrong_payload_length_period_volume_rejected':True,'timing':timing,
        'original_onset':onset(args.pcm_reference or args.original/'original.wav'),
        'native_onset':onset(args.native_report.parent/'native.wav'),
        'whole_flight_sound_acceptance':False}
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(result,indent=2)+'\n')
    print(f"Every {sum(map(len,original))} original startup music request matches native bytes/order/period/volume; onset/timing differences retained")


if __name__=='__main__':
    main()
