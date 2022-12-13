// #include <stdio.h>
// #include <string.h>

// #include "cJSON.h"
// #include "ff.h"
// #include "hardware/irq.h"
// #include "hardware/pwm.h"
// #include "pico/stdlib.h"
// #include "pico/time.h"
// #include "sd.h"
// #include "sd_card.h"

// #define PWM_PIN = 0

// char jsonString[100];

// void printTest(void) {
//     char* thisString = NULL;
//     cJSON* testData = cJSON_CreateObject();
//     cJSON_AddStringToObject(testData, "Key", "Values");

//     // thisString = cJSON_Print(testData);
//     thisString = cJSON_PrintUnformatted(testData);
//     // cJSON_PrintPreallocated(testData, thisString, sizeof(jsonString), false);

//     printf("Hello bitches! %s\n", thisString);
//     sprintf(jsonString, "%s", thisString);
//     cJSON_Delete(testData);
// }

// int main() {
//     char buf[100];
//     char filename[] = "bitch please 2.txt";

//     gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);

//     uint slice_num = pwm_gpio_to_slice(PWM_PIN);
//     pwm

//     printTest();

//     // Initialize chosen serial port
//     stdio_init_all();

//     sleep_ms(3000);
//     printf("Version: %i.%i\n\n", VERSION_MAJOR, VERSION_MINOR);
//     // Wait for user to press 'enter' to continue

//     // Initialize SD card
//     sdInit();

//     sdRead(filename);

//     // // Loop forever doing nothing

//     while (true) {
//         sleep_ms(1000);

//         // printf("size of jsonString: %i\n", strlen(jsonString));

//         // uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 7, 8};

//         // printf("Size of numbers: %i", sizeof(numbers));

//         // sdWriteString(filename, jsonString);
//         // sdWriteData("peepee.hex", numbers, sizeof(numbers));
//         // // sdWriteString("peepoo.hex", numbers, sizeof(numbers), true);
//         // printf("%s\n", jsonString);
//     }
// }

/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Fade an LED between low and high brightness. An interrupt handler updates
// the PWM slice's output level each time the counter wraps.

#include <stdio.h>

#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "pico/time.h"

#define PWM_PIN 0

int main() {
    int fanSpeed = 0;
    bool fanDirFwd = true;

    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN);

    pwm_set_enabled(slice_num, true);

    pwm_set_wrap(slice_num, 500);

    while (1) {
        pwm_set_chan_level(slice_num, PWM_CHAN_A, fanSpeed);
        if (fanDirFwd) {
            fanSpeed++;
        } else {
            fanSpeed--;
        }

        if (fanSpeed > 500) {
            fanSpeed == 499;
            fanDirFwd = false;
        }

        if (fanSpeed < 0) {
            fanSpeed == 1;
            fanDirFwd = true;
        }

        sleep_ms(100);
    }
}