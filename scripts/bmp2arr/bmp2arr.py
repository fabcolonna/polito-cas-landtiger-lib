import sys
import struct
import argparse

# Read the color table (CT) from the BMP file (only 1-4-8 bits/pixel)
#? The color table contains the RGB values of the colors used in the image,
#? codified in 4 bytes (R, G, B, <reserved>). The number of colors in the CT
#? is defined by the BPP -> 2^bpp.

def read_ct(bmp, bpp):
    palette = []
    palette_len = 2**bpp # 2, 16 and 256 colors with 1, 4, 8 bits, respectively
    for col in [bmp.read(4) for _ in range(palette_len)]:
        r, g, b, _ = struct.unpack("<BBBB", col)
        palette.append((r, g, b))
    return palette

# Read the pixel colors from the BMP file using the color table (only 1-4-8 bits/pixel)

def read_pixel_colors_ct(bmp, palette, width, height, bpp):
    colors= []
    for _ in range(height):
        indexes = []
        # Reading the entire row of pixels, made of w number of values of bpp bits
        row_total_bytes = int((width * bpp) / 8)
        for byte in struct.unpack(f"<{row_total_bytes}B", bmp.read(row_total_bytes)):
            if bpp == 1: # The byte contains 8 indexes
                indexes.extend([(byte >> i) & 1 for i in range(7, -1, -1)])
            elif bpp == 4: # The byte contains 2 indexes
                indexes.extend([(byte >> i) & 0x0F for i in (4, 0)])
            else: # The byte contains 1 index
                indexes.append(byte)
        # Converting the indexes to RGB values using the palette
        row_colors = [palette[i] for i in indexes]
        colors.append(row_colors) 
    # BMP pixels are stored bottom-up, so we need to reverse the list
    return colors[::-1]

# Write the pixel colors to a C header file

def write_to_c_header(h_file, bmp_name, pixel_colors, width, height):
    with open(h_file, "w") as h:
        incl_guard = f"__{bmp_name.upper().replace('.', '_')}_H\n"
        c_arr_name = f"{bmp_name.lower().replace('.', '_')}_rgb"

        h.write(f"#ifndef {incl_guard}")
        h.write(f"#define {incl_guard}\n")
        h.write("#include <stdint.h>\n\n")

        h.write(f"const uint8_t {c_arr_name}[] = {{\n")
        for row in range(height):
            h.write("\t")
            for col in range(width):
                r, g, b = pixel_colors[row][col]
                h.write(f"0x{r:02X}, 0x{g:02X}, 0x{b:02X} /* - */, ")
            h.write("\n")
        
        h.write("}};\n\n")
        h.write(f"#endif // {incl_guard}\n")

# Main function

if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog='bmp2arr', description='Convert BMP image to an RGB, C-style array')
    parser.add_argument('bmp_file', type=str, help='BMP file to convert (only 1-4-8 BPP)')
    parser.add_argument('out_file', type=str, help='Output file in which to write the C-style array')
    parser.add_argument('--rgb565', action='store_false', help='Convert RGB888 to RGB565 for 16-bit displays')
    parser.add_argument('--merge-rgb', action='store_false', help='Merge RGB values into a single 16-bit value')

    args = parser.parse_args()
    bmp_file = args.bmp_file
    h_file = args.out_file

    with open(bmp_file, "rb") as bmp:
        pixel_colors = []

        # BMP structure: http://ue.eti.pg.gda.pl/fpgalab/zadania.spartan3/zad_vga_struktura_pliku_bmp_en.html
        sign, _, _, pixel_offset = struct.unpack("<2sIII", bmp.read(14)) # from header
        _, w, h, _, bpp, _, _, _, _, _, _ = struct.unpack("<IIIHHIIIIII", bmp.read(40)) # from info header
        
        # If BPP is 1, 4 or 8, the BMP file contains a color table and each pixel is an index to the
        # entry in the CT that corresponds to the pixel color. For BPP > 8, the RGB values are stored
        # directly in the pixel data.
        if bpp in (1, 4, 8):
            color_palette = read_ct(bmp, bpp)
            pixel_colors = read_pixel_colors_ct(bmp, color_palette, w, h, bpp)
        else:
            raise ValueError("Unsupported BMP format (supported: 1, 4, 8 bits/pixel)")

        if args.rgb565:
            pixel_colors = rgb_to_rgb565(pixel_colors)
        if args.merge_rgb:
            pixel_colors

        write_to_c_header(h_file, bmp.name, pixel_colors, w, h)