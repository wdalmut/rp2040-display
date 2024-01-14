#include "ff.h"
#include <setjmp.h>

#include <png.h>
#define PNG_READ_SUPPORTED

#include "DEV_Config.h"
#include "LCD_1in14.h"
#include "GUI_Paint.h"
#include "png-help.h"
#include "display.h"

static void _render(Png_t *, uint16_t, uint16_t);

void display_init(void)
{
    printf("Initialize display...\n");
    if (DEV_Module_Init() != 0) {
        return;
    }

    /* LCD Init */
    LCD_1IN14_Init(HORIZONTAL);
    display_clear(BLACK);
    
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

void display_clear(uint16_t color)
{
    LCD_1IN14_Clear(color);
}

void display_png(Png_t *p)
{
    _render(p, 0, 0);
}

void display_png_at(Png_t *p, uint16_t row, uint16_t col)
{
    _render(p, row, col);
}

static void _render(Png_t *p, uint16_t at_row, uint16_t at_col)
{
    int maxCol = p->width > LCD_1IN14.WIDTH ? LCD_1IN14.WIDTH : p->width;
    int maxRow = p->height > LCD_1IN14.HEIGHT ? LCD_1IN14.HEIGHT : p->height;

    for (int i=0, row = at_row; i < maxRow; i++, row++) {
        png_bytep row_pointers = w_png_read_row(p);

        for (int j=0, col = 0; j < maxCol; col+=p->channels, j++) {
            png_byte red, green, blue;

            if (p->channels == 4) {
                png_bytep pixel = &row_pointers[col] + 3;
                if (*pixel == 0) {
                    continue;
                }
            }

            if ((p->color_type == PNG_COLOR_TYPE_PALETTE) && (p->palette != NULL)) {
                red = p->palette[row_pointers[col]].red;
                green = p->palette[row_pointers[col]].green;
                blue = p->palette[row_pointers[col]].blue;
            } else {
                // if the image is paletted but we don't have a palette, display as grayscale using palette index.
                png_bytep pixel = &row_pointers[col];
                red = *(pixel++);
                green = *(pixel++);
                blue = *(pixel++);
            }
            
            LCD_1IN14_DisplayPoint(j + at_col, row, ((red>>3) << 11) | ((green>>2) << 5) | blue >> 3);
        }

        w_png_free_row(p, row_pointers);
    }
}
