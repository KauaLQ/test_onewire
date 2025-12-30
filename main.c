// baseado em: https://github.com/raspberrypi/pico-examples/blob/master/pio/onewire/onewire.c

// #include <stdio.h>
// #include "pico/stdlib.h"
// #include "modules/temperature/ds18b20.h"

// int main() {
//     stdio_init_all();
//     sleep_ms(1000);

//     ds18b20_t sensor;
//     ds18b20_init(&sensor, pio0, 17);

//     while (true) {
//         float temp = ds18b20_read_temperature(&sensor);

//         if (temp == DS18B20_ERROR_TEMP) {
//             printf("Sensor desconectado\n");
//         } else {
//             printf("Temperatura: %.2f C\n", temp);
//         }

//         sleep_ms(100);
//     }
// }

/******************************************************************************************************/
/*Para testar a versão com freeRTOS, use esse exemplo (lembre-se de comentar o exemplo de uso normal)*/
/****************************************************************************************************/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "FreeRTOS.h"
#include "task.h"

#include "modules/RTOS_friendly/ds18b20.h"

// GPIO do barramento 1-Wire
#define DS18B20_GPIO 17

// Stack e prioridade da task
#define TEMP_TASK_STACK_SIZE  512
#define TEMP_TASK_PRIORITY   (tskIDLE_PRIORITY + 1)

static ds18b20_t ds18b20;

/* Task responsável pela leitura do sensor */
void temperature_task(void *pvParameters) {
    (void) pvParameters;

    while (true) {
        float temp = ds18b20_rtos_read_temperature(&ds18b20);

        if (temp == DS18B20_ERROR_TEMP) {
            printf("[DS18B20] Sensor desconectado ou erro\n");
        } else {
            printf("[DS18B20] Temperatura: %.2f °C\n", temp);
        }

        // Intervalo entre leituras
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

int main() {
    stdio_init_all();
    sleep_ms(1500);   // tempo para abrir o terminal

    printf("Inicializando DS18B20 com FreeRTOS...\n");

    // Inicializa o sensor (PIO0)
    if (!ds18b20_rtos_init(&ds18b20, pio0, DS18B20_GPIO)) {
        printf("Falha ao inicializar DS18B20!\n");
        while (true) {
            tight_loop_contents();
        }
    }

    // Cria task de temperatura
    BaseType_t result = xTaskCreate(
        temperature_task,
        "TempTask",
        TEMP_TASK_STACK_SIZE,
        NULL,
        TEMP_TASK_PRIORITY,
        NULL
    );

    if (result != pdPASS) {
        printf("Erro ao criar task de temperatura!\n");
        while (true) {
            tight_loop_contents();
        }
    }

    printf("Iniciando scheduler...\n");
    vTaskStartScheduler();

    // Nunca deve chegar aqui
    while (true) {
        tight_loop_contents();
    }
}