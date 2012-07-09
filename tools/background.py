import sys
import Image
import gameduino
import gameduino.prep as gdprep

(dpic, dchr, dpal) = gdprep.encode(Image.open(sys.argv[1]))
header = open("background.h", "w")
gdprep.dump(header, "titlescreen_pic", dpic)
gdprep.dump(header, "titlescreen_chr", dchr)
gdprep.dump(header, "titlescreen_pal", dpal)
