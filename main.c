// baseado em: https://github.com/raspberrypi/pico-examples/blob/master/pio/onewire/onewire.c

#include <stdio.h>
#include "pico/stdlib.h"

#include "modules/onewire_library/onewire_library.h"
#include "modules/onewire_library/ds18b20.h"
#include "modules/onewire_library/ow_rom.h"

int main() {
    stdio_init_all();

    PIO pio = pio0;
    uint gpio = 17;

    OW ow;
    uint offset;

    // Adiciona o programa onewire ao PIO
    if (!pio_can_add_program(pio, &onewire_program)) {
        printf("Erro ao adicionar programa onewire.\n");
        while (1);
    }

    offset = pio_add_program(pio, &onewire_program);

    // Inicializa driver
    if (!ow_init(&ow, pio, offset, gpio)) {
        printf("Erro ao inicializar driver Onewire.\n");
        while (1);
    }

    // --- Buscar apenas 1 sensor ---
    uint64_t romcode;
    int found = ow_romsearch(&ow, &romcode, 1, OW_SEARCH_ROM);

    if (found <= 0) {
        printf("Nenhum DS18B20 encontrado!\n");
        while (1);
    }

    printf("Sensor encontrado: 0x%llx\n\n", romcode);

    while (true) {
        // Inicia conversão
        ow_reset(&ow);
        ow_send(&ow, OW_MATCH_ROM);

        // Envia ROM
        for (int b = 0; b < 64; b += 8)
            ow_send(&ow, romcode >> b);

        ow_send(&ow, DS18B20_CONVERT_T);

        // Aguarda conversão terminar
        while (ow_read(&ow) == 0);

        // Ler scratchpad
        ow_reset(&ow);
        ow_send(&ow, OW_MATCH_ROM);
        for (int b = 0; b < 64; b += 8)
            ow_send(&ow, romcode >> b);

        ow_send(&ow, DS18B20_READ_SCRATCHPAD);

        int16_t raw_temp = ow_read(&ow) | (ow_read(&ow) << 8);
        float temp_c = raw_temp / 16.0f;

        printf("Temperatura: %.2f C\n", temp_c);
        sleep_ms(1000);
    }
}
