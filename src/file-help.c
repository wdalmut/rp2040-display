#include <stdio.h>
#include <stdlib.h>
//
#include "f_util.h"
#include "ff.h"
#include "pico/stdlib.h"

#include <string.h>

uint16_t enumerate_pngs(char *path)
{
    FRESULT fr;
    FILINFO fileInfo;
    DIR directory;

    uint16_t count = 0;

    // Open the directory you want to list PNG files from
    fr = f_opendir(&directory, path);
    if (FR_OK != fr) {
        printf("Error opening directory: %d\n", fr);
        return fr;
    }

    for (uint16_t i=0; i<0xFFFF; i++) {
        fr = f_readdir(&directory, &fileInfo);
        if (fr != FR_OK || fileInfo.fname[0] == 0) {
            // Error or end of directory
            break;
        }

        // Check if the file has a ".png" extension
        if (strstr(fileInfo.fname, ".png") != NULL) {
            count++;
        }
    }

    // Close the directory
    f_closedir(&directory);

    return count;
}

size_t get_random_png_from_path(uint16_t position, char *path, FIL *fil)
{
    FRESULT fr;
    FILINFO fileInfo;
    DIR directory;

    uint16_t count = 0;

    // Open the directory you want to list PNG files from
    fr = f_opendir(&directory, path);
    if (FR_OK != fr) {
        printf("Error opening directory: %d\n", fr);
        return 1;
    }

    for (uint16_t i=0; i<0xFFFF; i++) {
        fr = f_readdir(&directory, &fileInfo);
        if (fr != FR_OK || fileInfo.fname[0] == 0) {
            // Error or end of directory
            break;
        }

        // Check if the file has a ".png" extension
        if (strstr(fileInfo.fname, ".png") != NULL) {
            if (count++ == position) {
                printf("file %s\n", fileInfo.fname);
                break;
            }
        }
    }

    // Close the directory
    f_closedir(&directory);

    // error
    if (fileInfo.fname[0] == 0) {
        printf("Error finding file\n");
        return 1;
    }

    // Open the file you want to read from
    char *filename = (char *)malloc(strlen(path) + strlen(fileInfo.fname) + 1);
    strcpy(filename, path);
    strcpy(filename + strlen(path), fileInfo.fname);
    printf("filename %s\n", filename);

    fr = f_open(fil, filename, FA_READ);
    if (FR_OK != fr && FR_EXIST != fr) {
        printf("f_open(%s) error: %s (%d)\n", fileInfo.fname, FRESULT_str(fr), fr);
        return 1;
    }

    return 0;
}