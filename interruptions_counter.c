#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"

// Configurações dos pinos
const uint ledR_pin = 13; // RED=> GPIO13
const uint button_A = 5; // Botão A = 5
const uint button_B = 6; // Botão B = 6

#define time 100

int main()
{
    // Inicializações
    stdio_init_all();

    // Inicializa o LED
    gpio_init(ledR_pin);              // Inicializa o pino do LED
    gpio_set_dir(ledR_pin, GPIO_OUT); // Configura o pino como saída

    gpio_init(button_A);
    gpio_set_dir(button_A, GPIO_IN); // Configura o pino como entrada
    gpio_pull_up(button_A);          // Habilita o pull-up interno

    gpio_init(button_B);
    gpio_set_dir(button_B, GPIO_IN); // Configura o pino como entrada
    gpio_pull_up(button_B);          // Habilita o pull-up interno

    // Loop principal
    while (true)
    {
        gpio_put(ledR_pin, true);
        sleep_ms(time);
        gpio_put(ledR_pin, false);
        sleep_ms(time);
    }
}