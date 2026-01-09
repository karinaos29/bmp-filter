#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct __attribute__((packed)) {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} BMPHeader;

typedef struct {
__attribute__((packed))
    uint32_t size;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t image_size;
    int32_t  x_pixels_per_meter;
    int32_t  y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t colors_important;
} BMPInfoHeader;

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} Pixel;

int main(int argc, char *argv[]){

    if (argc < 3) {
        printf("Usage: %s <input_file.bmp> <output_file.bmp>\n", argv[0]);
        return 1;
    }

    char *input = argv[1];
    char *output = argv[2];

    FILE *file_ptr = fopen(input, "rb");
    if (!file_ptr) {
        fprintf(stderr, "Error opening input file: %s\n", input);
        return 1;
    }

    BMPHeader header;
    BMPInfoHeader info;

    if (fread(&header, sizeof(BMPHeader), 1, file_ptr) != 1) {
        fprintf(stderr, "Error reading BMP header\n");
        fclose(file_ptr);
        return 1;
    }
    if (fread(&info, sizeof(BMPInfoHeader), 1, file_ptr) != 1) {
        fprintf(stderr, "Error reading BMP info header\n");
        fclose(file_ptr);
        return 1;
    }

    if (info.bit_count != 24) {
        fprintf(stderr, "Error: Only 24-bit BMPs supported.\n");
        fclose(file_ptr);
        return 1;
    }

    int width = info.width;
    int height = abs(info.height);

    int padding = (4 - (width * sizeof(Pixel)) % 4) % 4;
    Pixel *image = malloc(width * height * sizeof(Pixel));
    if (!image) {
        printf("Error: could not allocate memory for image (%d x %d)\n", width, height);
        fclose(file_ptr);
        return 1;
    }

    fseek(file_ptr, header.offset, SEEK_SET);
    for (int i = 0; i < height; i++) {
        size_t read = fread(&image[i * width], sizeof(Pixel), width, file_ptr);
        if (read != (size_t)width) {
            fprintf(stderr, "Error reading pixel data on row %d (expected %d, got %zu)\n", i, width, read);
            free(image);
            fclose(file_ptr);
            return 1;
        }

        fseek(file_ptr, padding, SEEK_CUR);
    }
    fclose(file_ptr);

    for (int i = 0; i < width * height; i++) {
        uint8_t gray = (uint8_t)(0.299 * image[i].red + 0.587 * image[i].green + 0.114 * image[i].blue);
        image[i].red = gray;
        image[i].green = gray;
        image[i].blue = gray;
    }

    FILE *out = fopen(output, "wb");
    if (!out) {
        fprintf(stderr, "Error opening output file: %s\n", output);
        free(image);
        return 1;
    }

    if (fwrite(&header, sizeof(BMPHeader), 1, out) != 1) {
        fprintf(stderr, "Error writing BMP header to output\n");
        fclose(out);
        free(image);
        return 1;
    }
    if (fwrite(&info, sizeof(BMPInfoHeader), 1, out) != 1) {
        fprintf(stderr, "Error writing BMP info header to output\n");
        fclose(out);
        free(image);
        return 1;
    }
    uint8_t pad_byte = 0;
    for (int i = 0; i < height; i++) {
        if (fwrite(&image[i * width], sizeof(Pixel), width, out) != (size_t)width) {
            fprintf(stderr, "Error writing pixel data on row %d\n", i);
            fclose(out);
            free(image);
            return 1;
        }

        for (int p = 0; p < padding; p++) {
            fwrite(&pad_byte, 1, 1, out);
        }
    }

    printf("Done! Saved as %s\n", output);

    fclose(out);
    free(image);
    return 0;

}
