import struct
import argparse

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

#? Reads the color table (CT) from a 1-4-8 BPP bitmap and returns it as a list of RGB565 values.
#? The color table contains the RGB values of the colors used in the image, codified in 4 bytes,
#? where the first 3 bytes are the RGB values and the last byte is reserved. The size of the CT
#? is defined by the BPP -> 2^bpp.
def read_palette(bmp, bpp):
    palette = []
    for col in [bmp.read(4) for _ in range(2**bpp)]:    # 2, 16 and 256 colors with 1, 4, 8 bits, respectively
        r, g, b, _ = struct.unpack("<BBBB", col)
        palette.append(to_565(r, g, b))
    return palette

#? Reads the pixel colors from the BMP file using the color table (only 1-4-8 BPP), and returns
#? them as a list of RGB565 values. The pixels are stored in the BMP file as indexes to the CT.
def get_pixels_1_4_8(bmp, width, height, bpp):
    palette = read_palette(bmp, bpp)
    pixels = []

    # Need to calculate the stride, i.e. the alignment of the rows. It depends on the width and BPP.
    stride = (width * bpp + 7) // 8     # e.g. 4 BPP 34x34 = 18 bytes
    padded_bytes = (stride + 3) & ~3    # & with ~3 means having 4 byte alignment (first 2 bits = 0), e.g. 18 -> 20
    
    for _ in range(height):
        row_bytes = bmp.read(stride)            # Read signficant bytes (e.g. 18) from a row
        bmp.seek(padded_bytes - stride, 1)      # Skip padding bytes from current position (1), so that next read is aligned

        idxs = []
        for byte in struct.unpack(f"<{len(row_bytes)}B", row_bytes):
            match bpp:
                case 1:
                    idxs.extend([(byte >> i) & 0x1 for i in range(7, -1, -1)])  # MSB to LSB (range goes from 7 to 0)
                case 4:
                    idxs.extend([(byte >> i) & 0xF for i in (4, 0)])            # High and low portions
                case 8:
                    idxs.append(palette[byte])                                  # Direct index to the color table

        pixels.extend([palette[idx] for idx in idxs])
    return pixels[::-1]                                   # Reverse the rows since BMP files are stored bottom-up

#? Read the pixel colors from 24 bit bitmaps, and returns them as a list of RGB565 values.
#? The pixels are stored in the BMP file as 3 bytes per pixel, in the order BGR.
def get_pixels_24(bmp, width, height):
    pixels = []

    # Stride for determining the padding of the rows
    stride = (width * 24) // 8          # e.g 3x3, stride = 9 bytes
    padded_bytes = (stride + 3) & ~3    # e.g 9 + 3 = 12 = 0b1100 & 0b1100 = 12
    for _ in range(height):
        row_bytes = bmp.read(stride)        # Read 3 bytes per pixel
        bmp.seek(padded_bytes - stride, 1)  # Skip padding bytes (e.g. 12 - 9 = 3), from current position (1)
        pixels.extend([to_565(r, g, b) for b, g, r in struct.iter_unpack("<BBB", row_bytes)])
    return pixels[::-1]                     # Reverse the rows since BMP files are stored bottom-up

#? Read the pixel colors from 32 bit bitmaps, and returns them as a list of either RGB565 or RGB8565 values.
#? The pixels are stored in the BMP file as 4 bytes per pixel, in the order RGBA. Alignment is not needed.
def get_pixels_32(bmp, width, height):
    pixels = []
    for _ in range(height):
        for r, g, b, _ in struct.iter_unpack("<BBBB", bmp.read(width * 4)):
            pixels.append(to_565(r, g, b))
    return pixels[::-1]

#? Write the pixel colors to a C uint16_t array.
def dump_to_c_arr(h_file, bmp_name, pixels):
    with open(h_file, "w") as h:
        incl_guard = f"__{bmp_name.upper().replace('.', '_')}_H"
        c_arr_name = f"{bmp_name.lower().replace('.', '_')}"
        h.write(f"#ifndef {incl_guard}\n#define {incl_guard}\n\n#include <stdint.h>\n\n")
        
        h.write(f"const uint16_t {c_arr_name}[] = {{\n")
        for px in pixels:
            h.write(f"\t0x{px:04X}, \n") # RG565, ie. 16-bit
        h.write("};\n\n#endif\n")

#? Main
parser = argparse.ArgumentParser(prog='bmp2rgb565', 
                                 description='Convert BMP image into a C-style array of RGB565 values')

parser.add_argument('bmp_file', type=str, help='BMP file to convert (1-4-8-24-32 BPP files)')
parser.add_argument('out_file', type=str, help='Output file in which to write the C-style array')

args = parser.parse_args()
bmp_file = args.bmp_file
h_file = args.out_file

with open(bmp_file, "rb") as bmp:
    sign, _, _, pixel_offset = struct.unpack("<2sIII", bmp.read(14))                # From Header
    _, w, h, _, bpp, _, _, _, _, _, _ = struct.unpack("<IIIHHIIIIII", bmp.read(40)) # From InfoHeader
    
    if sign != b"BM":
        parser.error(f"Invalid BMP file: {bmp_file}")
    if bpp != 32 and args.include_alpha:
        parser.error("Alpha channel can only be included in 32 BPP BMP files")

    match bpp:
        case 1 | 4 | 8:
            pixels = get_pixels_1_4_8(bmp, w, h, bpp)
        case 24:
            bmp.seek(pixel_offset) # Skip to the pixel data
            pixels = get_pixels_24(bmp, w, h)
        case 32:
            bmp.seek(pixel_offset) # Skip to the pixel data
            pixels = get_pixels_32(bmp, w, h)
        case _:
            parser.error(f"Unsupported BPP: {bpp}")

    dump_to_c_arr(h_file, bmp.name, pixels)
    print(f"BMP info: BPP: {bpp} Size: {w}x{h} :: Output: {h_file}")