#include "ff.h"
#include <setjmp.h>

#include <png.h>
#define PNG_READ_SUPPORTED

#include "DEV_Config.h"
#include "LCD_1in14.h"
#include "GUI_Paint.h"
#include "png-help.h"

void custom_read_data(png_structrp png_ptr, png_bytep data, size_t length) {
    printf("Custom read data...\n");
    UINT bytesRead;
    custom_file *filep = (custom_file*)png_get_io_ptr(png_ptr);
    f_read(filep->file, data, length, &bytesRead);
}

static void error(png_structp png_ptr, const char *message)
{
    printf("Error from libpng: %s\n", message);
}

void display_init(void)
{
    printf("Initialize display...\n");
    if (DEV_Module_Init() != 0) {
        return;
    }

    /* LCD Init */
    LCD_1IN14_Init(HORIZONTAL);
    LCD_1IN14_Clear(BLACK);

    /* Turn backlight on */
    printf("Turning on backlight...\n");
    EPD_BL_PIN = 25;
    DEV_GPIO_Mode(EPD_BL_PIN, GPIO_OUT);
    DEV_Digital_Write(EPD_CS_PIN, 1);
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_BL_PIN, 1);

    // printf("DEV_Module_Exit...\n");
    // DEV_Module_Exit();
}

void display_png(FIL *file)
{
    printf("Creating read structure...\n");
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, error, NULL);

    if (png_ptr == NULL) {
        printf("png_create_read_struct error\n");
        return;
    }

    printf("Allocating memory for image information...\n");
    png_infop info_ptr = png_create_info_struct(png_ptr);

    if (info_ptr == NULL) {
        printf("png_create_info_struct error\n");
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return;
    }

    png_set_palette_to_rgb(png_ptr);

    printf("Setting up the custom read function...\n");
    custom_file filep;
    filep.file = file;
    png_set_read_fn(png_ptr, &filep, custom_read_data);

    printf("Setting up LongJump...\n");

    if (setjmp(png_jmpbuf(png_ptr)) == 0) {
        printf("LongJump set...\n");
    } else {
        printf("We got a LongJump, destroying read struct...\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return;
    }

    // The call to png_read_info() gives us all of the information from the
    // PNG file before the first IDAT (image data chunk). REQUIRED.
    printf("Reading info...\n");
    png_read_info(png_ptr, info_ptr);

    printf("Parsing image info...\n");
    png_uint_32 width, height;
    int bit_depth, color_type, interlace_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
    printf("PNG info: width: %d, height: %d, bit_depth: %d, color_type: %d\n", width, height, bit_depth, color_type);
    
    png_bytep row_pointers;
    int col, row;
    
    int maxCol = LCD_1IN14.WIDTH;
    int maxRow = LCD_1IN14.HEIGHT;

    int num_palette = 0;
    png_colorp palette = NULL;

    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        printf("Palette found...\n");
        png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
        printf("num_palette: %d\n", num_palette);
    }

    printf("rowbytes: %d\n", png_get_rowbytes(png_ptr, info_ptr));

    int i = 0;
    png_byte channels = png_get_channels(png_ptr, info_ptr);
    printf("channels: %d\n", channels);

    for (row = 0; row < maxRow; row++) {
        row_pointers = (png_bytep)png_malloc(png_ptr, png_get_rowbytes(png_ptr, info_ptr));

        png_read_rows(png_ptr, &row_pointers, NULL, 1);

        for (i=0, col = 0; i < maxCol; col+=channels, i++) {
            png_byte red, green, blue;

            if (channels == 4 && (&row_pointers[col+3] + 4) == 0) {
                printf("Pixel at (%d, %d) is transparent.\n", col, row);
                continue;
            }

            if ((color_type == PNG_COLOR_TYPE_PALETTE) && (palette != NULL)) {
                red = palette[row_pointers[col]].red;
                green = palette[row_pointers[col]].green;
                blue = palette[row_pointers[col]].blue;
            } else {
                // if the image is paletted but we don't have a palette, display as grayscale using palette index.
                png_bytep pixel = &row_pointers[col];
                red = *(pixel++);
                green = *(pixel++);
                blue = *(pixel++);
            }
            
            LCD_1IN14_DisplayPoint(i, row, ((red>>3) << 11) | ((green>>2) << 5) | blue >> 3);
        }

        png_free(png_ptr, row_pointers);
        row_pointers = NULL;
    }

    printf("Done! Destroying read struct...\n");
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
}
