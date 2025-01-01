from PIL import Image
from argparse import ArgumentParser as ArgParser

def to_565(r, g, b):
    r = (r >> 3) & 0x1F
    g = (g >> 2) & 0x3F
    b = (b >> 3) & 0x1F
    return (r << 11) | (g << 5) | b

def rle_compress(data, width):
    if not data:
        return []
    
    compressed = []
    for row_start in range(0, len(data), width): # Iterate over each row
        row = data[row_start:row_start + width]  # Get the entire row data
        
        count = 1
        prev_value = row[0]
        for value in row[1:]:
            if value == prev_value:
                count += 1
                # RLE count maxes out at 65535 for 16-bit storage
                if count == 65535:
                    compressed.append((count, prev_value))
                    count = 0
            else:
                compressed.append((count, prev_value))
                count = 1
            prev_value = value

        # Append the last run of the row
        compressed.append((count, prev_value))
    return compressed

def dump_to_file(filename, width, height, compressed_data, arr_name):
    with open(filename, "w") as f:
        incl_guard = f"__{arr_name.upper()}_H"
        f.write(f"#ifndef {incl_guard}\n#define {incl_guard}\n\n#include <stdint.h>\n\n")
        f.write(f"const uint16_t {arr_name}Width = {width};\n")
        f.write(f"const uint16_t {arr_name}Height = {height};\n")
        f.write(f"const uint16_t {arr_name}[] = {{\n")
        
        for count, value in compressed_data:
            f.write(f"\t{count}, 0x{value:04X},\n")
        
        f.write("};\n")
        f.write("#endif\n")

# Main program
parser = ArgParser(prog='img2c-rle', description='Converts an image into a C-style RLE-compressed array of RGB565 values')
parser.add_argument('img_file', type=str, help='Image file to convert')
parser.add_argument('out_file', type=str, help='Output file in which to write the C-style array')

image_opts = parser.add_mutually_exclusive_group()
image_opts.add_argument('-s', type=float, help='Scale factor to apply to the image')
image_opts.add_argument('-r', type=int, nargs=2, help='Resize the image to the given dimensions')

output_opts = parser.add_argument_group()
output_opts.add_argument('-S', type=str, help='Save the image with the given scale or dimensions')
output_opts.add_argument('-n', type=str, help='Name of the array to generate', required=True)
args = parser.parse_args()

try:
    # Load the image
    with Image.open(args.img_file) as img:
        img = img.convert("RGB")
        
        # Scale or resize if requested
        if args.s:
            img_w, img_h = img.size
            img = img.resize((int(img_w * args.s), int(img_h * args.s)))
        elif args.r:
            img = img.resize(tuple(args.r))
        
        # Optionally save the processed image
        if args.S:
            img.save(args.S)

        # Convert to RGB565
        pixels = [to_565(r, g, b) for r, g, b in img.getdata()]
        width, height = img.size

        # Compress using RLE
        compressed_pixels = rle_compress(pixels, width)

        # Save to header file
        dump_to_file(args.out_file, width, height, compressed_pixels, args.n)

except Exception as e:
    print(f"Something bad happened: {e}")