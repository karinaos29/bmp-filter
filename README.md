# BMP Grayscale Filter

A lightweight C utility that applies a luminance-based grayscale filter to 24-bit BMP images. It processes images at the bit level by manually handling file headers, pixel arrays, and row padding.

## Features
- Direct binary manipulation of BMP headers.
- Handles BMP row padding (4-byte alignment).
- Uses the NTSC luminance formula for accurate grayscale conversion.
- No external dependencies beyond the C standard library.

## Notes
- Supports uncompressed 24-bit BMP files only
- Handles row padding according to BMP specification
- Height is treated as absolute to support top-down BMPs

## Building
Compile using `gcc`:
```bash
gcc main.c -o filter
./filter input.bmp output.bmp