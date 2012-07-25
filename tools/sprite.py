import Image
import gameduino.prep as gdprep
import sys
import argparse

## Command line arguments
parser = argparse.ArgumentParser(description='Process a sprite sheet and generate a header file.',
                                 epilog='Arguments can be called using --[first letter]. For example --output can be used by typing --o.')
parser.add_argument('--output', default='default', help='Name of output header file')
parser.add_argument('--palette', dest='palette_size', type=int, default=16, choices=[4,16,256],
                    help='Choose 4, 16, or 256 to represent the number of colors in the palette')
## say something about what the tuple does..
parser.add_argument('--center', default=[0,0], nargs=2, type=int,
                    help='Two integers ranging from 0 to 16 representing (x, y)')
parser.add_argument('--variable-name', dest='varname', default='sprite', 
                    help='Name of the arrays that will be generated')
parser.add_argument(dest='sheet', help='Sprite sheet to be processed')

args = parser.parse_args()

if (args.output == 'default'):
    ## get name of the input file and make input.h ex. mario.png gets mario.h
    args.output = args.sheet.split('.')[0] + '.h'

## else use output file provided. Check if it is a .h file
else:
    check_header = args.output.split('.')
    if (len(check_header) != 2 or check_header[1] != 'h'):
        raise TypeError('Supply a .h file! You supplied %s.' % args.output)

## palette checks itself by having choices=[...]

## check the arguments to center
if (args.center[0] < 0 or args.center[0] > 16):
    raise ValueError('x coordinate is not within 0 to 16.')
if (args.center[1] < 0 or args.center[1] > 16):
    raise ValueError('y coordinate is not within 0 to 16.')

## check file type of sprite sheet
check_sprite = args.sheet.split('.')
if (len(check_sprite) != 2 or check_sprite[1] != 'png'):
    raise TypeError('Supply a .png file! You supplied %s.' % args.sheet)

header = open(args.output, "w")

img_ram = gdprep.ImageRAM(header)

## select the correct palette
if (args.palette_size == 4):
    palette = gdprep.PALETTE4A

elif (args.palette_size == 16):
    palette = gdprep.PALETTE16A

else:
    palette = gdprep.PALETTE256A

img_pal = gdprep.palettize(Image.open(args.sheet), args.palette_size)
img_ram.addsprites(args.varname, (16, 16), img_pal, palette, center = args.center)

name_img = args.varname + '_img'
name_pal = args.varname + '_pal'
gdprep.dump(header, name_img, img_ram.used())
gdprep.dump(header, name_pal, gdprep.getpal(img_pal))

