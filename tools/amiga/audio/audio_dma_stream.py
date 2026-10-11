"""Decode complete legacy Engine9000 audio DMA streams."""
import gzip
from original_audio_dma import MAGIC, FRAME, FETCH, FOOTER

def exact(file, size):
    value = file.read(size)
    if len(value) != size:
        raise ValueError('Truncated original audio DMA stream')
    return value


def open_stream(path):
    return path.open('rb') if path.exists() else gzip.open(str(path)+'.gz','rb')


def frames(file, first_call, last_call, chip_size=0x80000):
    if exact(file, len(MAGIC)) != MAGIC:
        raise ValueError('Invalid original audio DMA stream header')
    next_call, previous_source, total = first_call, None, 0
    while True:
        tag = exact(file, 1)
        if tag == b'\0':
            calls, words = FOOTER.unpack(exact(file, FOOTER.size))
            if calls != last_call-first_call+1 or next_call != last_call+1 or words != total or file.read(1):
                raise ValueError('Incomplete original audio DMA footer/coverage')
            return
        if tag != b'\1':
            raise ValueError('Invalid original audio DMA frame tag')
        row = FRAME.unpack(exact(file, FRAME.size))
        call, hardware, source, toggle, hcount, vcount, offset, records, count, samples = row
        if call != next_call or call > last_call or hardware != call:
            raise ValueError('Missing or repeated original audio DMA call')
        if source < 0 or (previous_source is not None and source != previous_source+1):
            raise ValueError('Missing or repeated completed original DMA source frame')
        if toggle not in (0, 1) or (hcount, vcount, records) != (288, 1000, 288000) or abs(offset) >= hcount or count > records:
            raise ValueError('Invalid original audio DMA grid')
        payload = exact(file, count * FETCH.size)
        previous_index = -1
        for index, address, value, register, hpos, vpos, channel in FETCH.iter_unpack(payload):
            if index <= previous_index or index >= records:
                raise ValueError('Missing ordering or duplicated original DMA slot')
            if channel > 3 or register != 0xaa+16*channel or address & 1 or address >= chip_size:
                raise ValueError('Invalid original DMA channel/register/address')
            if hpos < 0 or not 0 <= vpos < vcount or index != vpos*hcount+hpos+offset:
                raise ValueError('Original DMA beam differs from its grid slot')
            previous_index = index
        yield row, payload
        next_call += 1
        previous_source = source
        total += count


