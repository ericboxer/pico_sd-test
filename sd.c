/* hw_config.c
Copyright 2021 Carl John Kugler III

Licensed under the Apache License, Version 2.0 (the License); you may not use
this file except in compliance with the License. You may obtain a copy of the
License at

   http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.
*/
/*

This file should be tailored to match the hardware design.

There should be one element of the spi[] array for each hardware SPI used.

There should be one element of the sd_cards[] array for each SD card slot.
The name is should correspond to the FatFs "logical drive" identifier.
(See http://elm-chan.org/fsw/ff/doc/filename.html#vol)
The rest of the constants will depend on the type of
socket, which SPI it is driven by, and how it is wired.

*/

#include "sd.h"

#include <string.h>
//
#include "my_debug.h"
//
#include "hw_config.h"
//
#include "ff.h" /* Obtains integer types */
//
#include "diskio.h" /* Declarations of disk functions */

void spi0_dma_isr();

FRESULT fr;
FATFS fs;
FIL fil;
char buf[1024];
int ret;
const char sdMount[2] = "0:";

// Hardware Configuration of SPI "objects"
// Note: multiple SD cards can be driven by one SPI if they use different slave
// selects.
static spi_t spis[] = {  // One for each SPI.
    {
        .hw_inst = spi1,  // SPI component
        .miso_gpio = 8,   // GPIO number (not pin number)
        .mosi_gpio = 11,
        .sck_gpio = 10,

        /* The choice of SD card matters! SanDisk runs at the highest speed. PNY
           can only mangage 5 MHz. Those are all I've tried. */
        // .baud_rate = 1000 * 1000,
        .baud_rate = 12500 * 1000,  // The limitation here is SPI slew rate.
        //.baud_rate = 25 * 1000 * 1000, // Actual frequency: 20833333. Has
        // worked for me with SanDisk.

        .dma_isr = spi0_dma_isr}};

// Hardware Configuration of the SD Card "objects"
static sd_card_t sd_cards[] = {  // One for each SD card
    {
        .pcName = sdMount,  // Name used to mount device
        .spi = &spis[0],    // Pointer to the SPI driving this card
        .ss_gpio = 9,       // The SPI slave select GPIO for this SD card
        .use_card_detect = false,
        .card_detect_gpio = 22,   // Card detect
        .card_detected_true = 1,  // What the GPIO read returns when a card is
                                  // present. Use -1 if there is no card detect.
        .m_Status = STA_NOINIT}};

void spi0_dma_isr() { spi_irq_handler(&spis[0]); }

/* ********************************************************************** */
size_t sd_get_num() { return count_of(sd_cards); }
sd_card_t *sd_get_by_num(size_t num) {
    if (num <= sd_get_num()) {
        return &sd_cards[num];
    } else {
        return NULL;
    }
}
size_t spi_get_num() { return count_of(spis); }
spi_t *spi_get_by_num(size_t num) {
    if (num <= sd_get_num()) {
        return &spis[num];
    } else {
        return NULL;
    }
}

// SD Card Functions

void sdInit() {
    if (!sd_init_driver()) {
        printf("ERROR: Could not initialize SD card\r\n");
    }

    // Mount drive
    fr = f_mount(&fs, "0:", 1);
    if (fr != FR_OK) {
        printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
    }
}

void sdWriteString(char *filename, char *data) {
    FILINFO fileInfo;
    uint *bw;

    // Check if the file exists.
    fr = f_stat(filename, &fileInfo);

    // Open the file
    switch (fr) {
        case FR_OK:
            fr = f_open(&fil, filename, FA_WRITE | FA_OPEN_APPEND);
            printf("Size: %lu\n", fileInfo.fsize);
            break;

        default:
            fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_NEW);
            break;
    }

    if (fr != FR_OK) {
        printf("ERROR: Could not open file (%d)\r\n", fr);
    }

    f_write(&fil, data, strlen(data), bw);

    sdClose();
}

void sdWriteData(char *filename, uint8_t data[], int dataSize) {
    FILINFO fileInfo;
    uint *bw;

    // Check if the file exists.
    fr = f_stat(filename, &fileInfo);

    // Open the file
    switch (fr) {
        case FR_OK:
            fr = f_open(&fil, filename, FA_WRITE | FA_OPEN_APPEND);
            printf("Size: %lu\n", fileInfo.fsize);
            break;

        default:
            fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_NEW);
            break;
    }

    if (fr != FR_OK) {
        printf("ERROR: Could not open file (%d)\r\n", fr);
    }

    f_write(&fil, data, dataSize, bw);
    sdClose();
}

void sdRead(char *filename) {
    // Open file for reading
    fr = f_open(&fil, filename, FA_READ);
    if (fr != FR_OK) {
        printf("ERROR: Could not open file (%d)\r\n", fr);
    }

    // Print every line in file over serial
    printf("Reading from file '%s':\r\n", filename);
    printf("---\r\n");

    while (f_gets(buf, sizeof(buf), &fil)) {
        printf(buf);
    }
    printf("\r\n---\r\n");

    sdClose();
}

void sdClose(void) {
    fr = f_close(&fil);
    if (fr != FR_OK) {
        printf("ERROR: Could not close file (%d)\r\n", fr);
        // while (true)
        //     ;
    }
}

void sdUnmount(void) {
    f_unmount(sdMount);
}

/* [] END OF FILE */
