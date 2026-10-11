"""Decode consumed-sample coverage from a retained observer stream."""
from audio_dma_stream import exact, open_stream
from original_audio_samples import MAGIC, FRAME, SAMPLE, FOOTER, sample_records

def frames(file, calls):
    assert exact(file, len(MAGIC)) == MAGIC, 'Invalid consumed-sample stream header'
    next_call, previous_cycle, total = 1, None, 0
    while True:
        tag = exact(file, 1)
        if tag == b'\0':
            footer_calls, samples = FOOTER.unpack(exact(file, FOOTER.size))
            assert next_call == calls+1 and footer_calls == calls and samples == total and not file.read(1), 'Incomplete consumed-sample footer'
            return
        assert tag == b'\1'
        call, count, before, after, pcm_end = FRAME.unpack(exact(file, FRAME.size))
        assert call == next_call and call <= calls and count <= 65536 and after >= before, 'Lost sample call/coverage'
        assert previous_cycle is None or before == previous_cycle, 'Lost consumed-sample cycle boundary'
        payload = exact(file, count*SAMPLE.size)
        yield (call, count, before, after, pcm_end), payload
        next_call += 1
        previous_cycle = after
        total += count


