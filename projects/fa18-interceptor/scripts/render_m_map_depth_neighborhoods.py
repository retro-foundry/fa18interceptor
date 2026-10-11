"""Render verified M-map depth-selected local selector neighborhoods."""
from pathlib import Path
from PIL import Image, ImageDraw

ROOT=Path(__file__).resolve().parents[1]
SIZE, CELL, MARGIN=700,80,150

def main():
    out=ROOT/'analysis/plots/m_map_depth_selector_neighborhoods.png'
    image=Image.new('RGB',(SIZE,SIZE),'#10151c'); d=ImageDraw.Draw(image)
    d.text((20,18),'M-map projection-depth selector neighborhoods',fill='#f1f5f9')
    d.text((20,38),'middle band: inner 3x3; high band: full 5x5. Local offsets only, not world coordinates.',fill='#cbd5e1')
    for y in range(-2,3):
        for x in range(-2,3):
            left=MARGIN+(x+2)*CELL; top=100+(2-y)*CELL
            inner=abs(x)<=1 and abs(y)<=1
            d.rectangle((left,top,left+CELL-4,top+CELL-4),fill='#55c6be' if inner else '#3e92cc',outline='#d5e0ea')
            d.text((left+18,top+27),f'({x},{y})',fill='#10151c')
    d.rectangle((MARGIN,100,MARGIN+5*CELL-4,100+5*CELL-4),outline='#f1f5f9',width=3)
    d.text((20,535),'teal: selected at middle depth ($C2A072, codes 0..8)',fill='#55c6be')
    d.text((20,557),'blue: additionally selected at high depth ($C2A0C2, codes 9..24)',fill='#3e92cc')
    d.text((20,600),'Source: C2AC1A threshold branches -> C29F00 signed offset table.',fill='#cbd5e1')
    out.parent.mkdir(parents=True,exist_ok=True); image.save(out)
    print(out)
if __name__=='__main__': main()
