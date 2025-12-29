// baseado em: https://github.com/raspberrypi/pico-examples/blob/master/pio/onewire/onewire.c
#include <stdio.h>
#include "pico/stdlib.h"
#include "modules/temperature/ds18b20.h"

int main() {
    stdio_init_all();
    sleep_ms(1000);

    ds18b20_t sensor;
    ds18b20_init(&sensor, pio0, 17);

    while (true) {
        float temp = ds18b20_read_temperature(&sensor);

        if (temp == DS18B20_ERROR_TEMP) {
            printf("Sensor desconectado\n");
        } else {
            printf("Temperatura: %.2f C\n", temp);
        }

        sleep_ms(100);
    }
}
