#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "f_util.h"
#include "ff.h"
#include "pico/stdlib.h"
#include "rtc.h"

#include "hw_config.h"

#include "png-help.h"
#include "file-help.h"

#define ROSC_RANDOMBIT_OFFSET _u(0x0000001c)
#define ROSC_BASE _u(0x40060000)

uint16_t get_a_random_index(uint16_t);

uint16_t get_a_random_index(uint16_t max)
{
    int k, random=0;
    int random_bit1, random_bit2;
    volatile uint32_t *rnd_reg=(uint32_t *)(ROSC_BASE + ROSC_RANDOMBIT_OFFSET);
    
    for(k=0;k<32;k++){
        while(1){
            random_bit1=0x00000001 & (*rnd_reg);
            random_bit2=0x00000001 & (*rnd_reg);
            if(random_bit1!=random_bit2) break;
        }

	    random = random << 1;        
        random=random + random_bit1;
    }

    return (uint16_t)(random&0x7FFFFFFF) % max;
}

int main() {
    stdio_init_all();

    sleep_ms(3000);

    display_init();    

    FATFS fs;
    FIL fil;

    FRESULT fr = f_mount(&fs, "", 1);
    if (FR_OK != fr) panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);

    uint16_t pngs = enumerate_pngs("");
    printf("PNGs: %d\n", pngs);

    while(1) {
        uint16_t random_number = get_a_random_index(pngs);

        size_t ret = get_random_png_from_path(random_number, "", &fil);
        if (0 != ret) {
            printf("get_random_png_from_path error: %d %d\n", random_number, ret);
            sleep_ms(5000);
            continue;
        }
        
        display_png(&fil);
        
        fr = f_close(&fil);
        if (FR_OK != fr) {
            printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
        }

        sleep_ms(5000);
    }

    f_unmount("");
    return 0;
}