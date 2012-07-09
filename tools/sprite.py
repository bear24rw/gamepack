import Image
import gameduino.prep as gdprep
import sys

f = sys.argv[1]

header = open("header.h", "w")
img_ram = gdprep.ImageRAM(header)
img_pal = gdprep.palettize(Image.open(f), 256)
img_ram.addsprites("sprite", (16, 16), img_pal, gdprep.PALETTE256A, center = (0,0))

gdprep.dump(header, "sprite_img", img_ram.used())
gdprep.dump(header, "sprite_pal", gdprep.getpal(img_pal))

