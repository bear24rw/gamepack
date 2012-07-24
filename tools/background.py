import sys
import Image
import gameduino
import gameduino.prep as gdprep
import argparse

## Command line arguments
parser = argparse.ArgumentParser(description='......something relevant.',
                                 epilog='Arguments can be called using --[first letter]. For example --output can be used by typing --o.')
parser.add_argument('--output', default='background.h', help='Name of output header file.')
parser.add_argument('--variable-name', default='background', dest='varname', 
                    help='Name of the arrays that will be generated')
## update name and replace in call to Image.open() and the check for sheet, update help message
parser.add_argument(dest='sheet', help='')

args = parser.parse_args()

## check if output is a .h file
check_header = args.output.split('.')
if (len(check_header) != 2 or check_header[1] != 'h'):
    raise TypeError('Supply a .h file! You supplied %s.' % args.output)

## check if sheet is a .png file
check_sheet = args.sheet.split('.')
if (len(check_sheet) != 2 or check_sheet[1] != 'png'):
    raise TypeError('Supply a .png file! You supplied %s.' %args.sheet)

(dpic, dchr, dpal) = gdprep.encode(Image.open(args.sheet))
header = open(args.output, "w")

name_pic = args.varname + '_pic'
name_chr = args.varname + '_chr'
name_pal = args.varname + '_pal'

gdprep.dump(header, name_pic, dpic)
gdprep.dump(header, name_chr, dchr)
gdprep.dump(header, name_pal, dpal)
