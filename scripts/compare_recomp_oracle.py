"""Compare a fa18_recomp 320x256 PPM with an Engine9000 720x287 screenshot.

The oracle's 320x200 game image is doubled horizontally at x=40..679,
y=16..215; its first row is beam line $2A, the fa18_recomp image origin.
Prints matching/total pixels as JSON."""
import argparse, json
from PIL import Image

p = argparse.ArgumentParser()
p.add_argument("native"); p.add_argument("oracle"); p.add_argument("--diff")
a = p.parse_args()
n = Image.open(a.native).convert("RGB"); o = Image.open(a.oracle).convert("RGB")
q = lambda px: tuple(round(c / 17) for c in px)
same = total = 0
diff = Image.new("RGB", (320, 200))
for y in range(200):
    ny = y
    for x in range(320):
        op = q(o.getpixel((40 + 2 * x, 16 + y)))
        npx = q(n.getpixel((x, ny))) if 0 <= ny < 256 else (0, 0, 0)
        total += 1
        if op == npx: same += 1
        else: diff.putpixel((x, y), (255, 0, 255))
if a.diff: diff.save(a.diff)
print(json.dumps({"matching": same, "total": total, "ratio": round(same / total, 5)}))
