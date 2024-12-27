from PIL import Image
import argparse
import os

#? Converts RGB888 to RGB565 and return it as a 16-bit value
#? Format: RRRRR-GGGGGG-BBBBB
def to_565(r, g, b):
    r565 = (r >> 3) & 0x1F
    g565 = (g >> 2) & 0x3F
    b565 = (b >> 3) & 0x1F
    return (r565 << 11) | (g565 << 5) | b565

#? Converts RGB8888 to RGB8565 and return it as a 24-bit value
#? Format: AAAAAAAA-RRRRRR-GGGGGG-BBBBB
def to_8565(r, g, b, a):
    return (a << 16) | to_565(r, g, b) 

#? Write the pixel colors to a C uint32_t array.
def dump_to_c_arr(h_file, png_name, pixels):
    with open(h_file, "w") as h:
        incl_guard = f"__{png_name.upper().replace('.', '_')}_H"
        c_arr_name = f"{png_name.lower().replace('.', '_')}"
        h.write(f"#ifndef {incl_guard}\n#define {incl_guard}\n\n#include <stdint.h>\n\n")
        
        h.write(f"const uint32_t {c_arr_name}[] = {{\n")
        for px in pixels:
            h.write(f"\t0x{px:06X}, \n") # RGB8565, ie. 24-bit
        h.write("};\n\n#endif\n")

#? Main
parser = argparse.ArgumentParser(prog='img2rgb8565', 
                                    description='Converts an image into a C-style array of RGB8565 values')

parser.add_argument('png_file', type=str, help='Image file to convert')
parser.add_argument('out_file', type=str, help='Output file in which to write the C-style array')

# TODO: Add scale options

args = parser.parse_args()
img_file = args.png_file
h_file = args.out_file

img_filename = os.path.splittext(os.path.basename(img_file))[0]

with Image.open(img_file) as img:
    rgb_image = img.convert("RGBA")
    w, h = rgb_image.size

    pixels = []
    for r, g, b, a in rgb_image.getdata():
        pixels.append(to_8565(r, g, b, a))

    dump_to_c_arr(h_file, img_filename, pixels)

    

