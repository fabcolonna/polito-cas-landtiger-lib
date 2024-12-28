from PIL import Image
from argparse import ArgumentParser as ArgParser

#? Converts RGB8888 to RGB8565 and return it as a 24-bit value
#? Format: AAAAAAAA-RRRRRR-GGGGGG-BBBBB
def to_8565(r, g, b, a):
    r >>= 3 & 0x1F
    g >>= 2 & 0x3F
    b >>= 3 & 0x1F
    return (a << 16) | (r << 11) | (g << 5) | b

#? Main Program
parser = ArgParser(prog='img2c', description='Converts an image into a C-style array of RGB8565 values')
parser.add_argument('img_file', type=str, help='Image file to convert')
parser.add_argument('out_file', type=str, help='Output file in which to write the C-style array')

image_opts = parser.add_mutually_exclusive_group()
image_opts.add_argument('-s', type=float, help='Scale factor to apply to the image')
image_opts.add_argument('-r', type=int, nargs=2, help='Resize the image to the given dimensions')

output_opts = parser.add_argument_group()
output_opts.add_argument('-S', type=str, help='Save the image with the given scale or dimensions')
output_opts.add_argument('-n', type=str, help='Name of the array to generate', required=True)

args = parser.parse_args()
pixels = []

with Image.open(args.img_file) as img:
    img = img.convert("RGBA")

    if args.s:
        img_w, img_h = img.size
        img = img.resize((int(img_w * args.s), int(img_h * args.s)))
    elif args.r:
        resize_w, resize_h = args.r
        img = img.resize((resize_w, resize_h))

    if args.S:
        img.save(args.S)

    pixels = [to_8565(r, g, b, a) for r, g, b, a in img.getdata()]

with open(args.out_file, "w") as out_header:
    arr_name = args.n or args.img_file.split('.')[0].replace('.', '_')
    incl_guard = f"__{arr_name.upper()}_H"

    out_header.write(f"""
#ifndef {incl_guard}
#define {incl_guard}

#include <stdint.h>

const uint16_t {arr_name}_width = {img.width};
const uint16_t {arr_name}_height = {img.height};

const uint16_t {arr_name}[] = {{
""")

    for row in range(img.height):
        for col in range(img.width):
            out_header.write(f"\t0x{pixels[row * img.width + col]:06X}, ")
        out_header.write("\n")

    out_header.write("""
};

#endif
                     
""")

