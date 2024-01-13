#include <stdio.h>
//
#include "f_util.h"
#include "ff.h"
#include "pico/stdlib.h"
#include "rtc.h"
//
#include "hw_config.h"

#include "png-help.h"

int main() {
    stdio_init_all();
    time_init();

    sleep_ms(3000);

    puts("Hello, world!");

    FATFS fs;
    FRESULT fr = f_mount(&fs, "", 1);
    if (FR_OK != fr) panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
    FIL fil;


    const char* const filename = "08.png";
    fr = f_open(&fil, filename, FA_READ);
    
    if (FR_OK != fr && FR_EXIST != fr) {
        panic("f_open(%s) error: %s (%d)\n", filename, FRESULT_str(fr), fr);
    }

    display_png(&fil);

    fr = f_close(&fil);
    if (FR_OK != fr) {
        printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    }
    
    f_unmount("");

    puts("Goodbye, world!");
    for (;;);
}