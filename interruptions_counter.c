#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

// Configurações dos pinos
const uint ledR_pin = 13; // RED=> GPIO13
#define time 100

int main()
{
    // Inicializações
    stdio_init_all();

    // Inicializa o LED
    gpio_init(ledR_pin);              // Inicializa o pino do LED
    gpio_set_dir(ledR_pin, GPIO_OUT); // Configura o pino como saída

    // Loop principal
    while (true)
    {
        gpio_put(ledR_pin, true);
        sleep_ms(time);
        gpio_put(ledR_pin, false);
        sleep_ms(time);
    }
}