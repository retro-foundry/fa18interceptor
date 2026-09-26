"""Reconstruct ordered OCS blit submissions from an Engine9000 DMA capture."""
from pathlib import Path
import argparse, json

def main():
    p=argparse.ArgumentParser(); p.add_argument('capture',type=Path); p.add_argument('output',type=Path); a=p.parse_args()
    records=json.loads(a.capture.read_text())['selected_records']
    regs={}
    jobs=[]
    for r in records:
        addr=int(r['addr'],16); value=int(r['dat'],16)&0xffff
        if not 0xdff040 <= addr <= 0xdff076 or addr & 1: continue
        regs[addr]=value
        if addr != 0xdff058: continue
        def pair(hi,lo): return (regs.get(hi,0)<<16)|regs.get(lo,0)
        jobs.append({'index':r['index'],'hpos':r['hpos'],'vpos':r['vpos'],
                     'bltcon0':regs.get(0xdff040,0),'bltcon1':regs.get(0xdff042,0),
                     'first_mask':regs.get(0xdff044,0),'last_mask':regs.get(0xdff046,0),
                     'c':pair(0xdff048,0xdff04a),'b':pair(0xdff04c,0xdff04e),
                     'a':pair(0xdff050,0xdff052),'d':pair(0xdff054,0xdff056),
                     'bdat':regs.get(0xdff072,0),'adat':regs.get(0xdff074,0),
                     'size':value,'cmod':regs.get(0xdff060,0),
                     'bmod':regs.get(0xdff062,0),'amod':regs.get(0xdff064,0),
                     'dmod':regs.get(0xdff066,0)})
    report={'capture':str(a.capture),'count':len(jobs),'jobs':jobs}
    a.output.write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({'count':len(jobs),'output':str(a.output)}))
if __name__=='__main__': main()
