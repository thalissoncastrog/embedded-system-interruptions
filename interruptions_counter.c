#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "interruptions_counter.pio.h"

#define NUM_PIXELS 25
#define WS2812_PIN 7
#define IS_RGBW false
#define DEBOUNCE_US 200000  // 200ms debounce

// Configurações dos pinos
const uint ledR_pin = 13; // RED=> GPIO13
const uint button_A = 5; // Botão A = 5
const uint button_B = 6; // Botão B = 6

// Variáveis globais
static int current_number = 0;
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)

// Prototipação da função de interrupção
static void gpio_irq_handler(uint gpio, uint32_t events);
static void gpio_irq_handler_button_B(uint gpio, uint32_t events);

static inline void put_pixel(uint32_t pixel_grb);
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);
void set_one_led(uint8_t r, uint8_t g, uint8_t b);

#define time 100

static volatile uint COUNTER = 0;

// Variável global para armazenar a cor (Entre 0 e 255 para intensidade)
uint8_t led_r = 0; // Intensidade do vermelho
uint8_t led_g = 0; // Intensidade do verde
uint8_t led_b = 200; // Intensidade do azul

bool led_number_0[NUM_PIXELS] = {
    0, 1, 1, 1, 0,
    1, 1, 0, 0, 1,
    1, 0, 1, 0, 1,
    1, 0, 0, 1, 1,
    0, 1, 1, 1, 0,
};

bool led_number_1[NUM_PIXELS] = {
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 1, 1, 0, 0, 
    0, 0, 1, 0, 0
};

bool led_number_2[NUM_PIXELS] = {
    1, 1, 1, 1, 0,
    0, 0, 1, 0, 0,
    0, 1, 0, 0, 0,
    0, 1, 0, 0, 1,
    0, 1, 1, 0, 0
};

bool led_number_3[NUM_PIXELS] = {
    0, 1, 1, 1, 0,
    1, 0, 0, 0, 1,
    0, 1, 1, 0, 0,
    1, 0, 0, 0, 1,
    0, 1, 1, 1, 0
};

bool led_number_4[NUM_PIXELS] = {
    0, 0, 1, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 1, 1, 1,
    1, 0, 1, 0, 0,
    0, 0, 1, 0, 1
};

bool led_number_5[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    0, 0, 0, 0, 1,
    1, 1, 1, 1, 1,
    1, 0, 0, 0, 0,
    1, 1, 1, 1, 1
};

bool led_number_6[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    1, 0, 0, 0, 1,
    1, 1, 1, 1, 1,
    1, 0, 0, 0, 0,
    1, 1, 1, 1, 1
};

bool led_number_7[NUM_PIXELS] = {
    0, 0, 0, 1, 0,
    0, 0, 1, 0, 0,
    0, 1, 0, 0, 0,
    0, 0, 0, 0, 1,
    1, 1, 1, 1, 1,
};

bool led_number_8[NUM_PIXELS] = {
    0, 1, 1, 1, 0,
    1, 0, 0, 0, 1,
    0, 1, 1, 1, 0,
    1, 0, 0, 0, 1,
    0, 1, 1, 1, 0
};

bool led_number_9[NUM_PIXELS] = {
    1, 1, 1, 1, 1,
    0, 0, 0, 0, 1,
    1, 1, 1, 1, 1,
    1, 0, 0, 0, 1,
    1, 1, 1, 1, 1
};

bool turn_off_all_leds[NUM_PIXELS] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
};

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

    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &interruptions_counter_program);

    // Configuração da interrupção com callback
    gpio_set_irq_enabled_with_callback(button_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled(button_B, GPIO_IRQ_EDGE_FALL, true);

    interruptions_counter_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    set_one_led(led_r, led_g, led_b);

    // Loop principal
    while (true)
    {
        gpio_put(ledR_pin, true);
        sleep_ms(time);
        gpio_put(ledR_pin, false);
        sleep_ms(time);

        
    }
}

static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

bool* draw_number(int number)
{
    switch (number)
    {
    case 0:
        return led_number_0;
        break;
    case 1:
        return led_number_1;
        break;
    case 2:
        return led_number_2;
        break;
    case 3:
        return led_number_3;
        break;
    case 4:
        return led_number_4;
        break;
    case 5:
        return led_number_5;
        break;
    case 6:
        return led_number_6;
        break;
    case 7:
        return led_number_7;
        break;
    case 8:
        return led_number_8;
        break;
    case 9:
        return led_number_9;
        break;    
    default:
        return turn_off_all_leds;
        break;
    }
}


void set_one_led(uint8_t r, uint8_t g, uint8_t b)
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    bool* led_number = draw_number(COUNTER);

    // Define todos os LEDs com a cor especificada
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        if (led_number[i])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0);  // Desliga os LEDs com zero no buffer
        }
    }
}

// Função de interrupção com debouncing
void gpio_irq_handler(uint gpio, uint32_t events)
{
    static absolute_time_t last_interrupt_time = {0};
    absolute_time_t current_time = get_absolute_time();

    if (absolute_time_diff_us(last_interrupt_time, current_time) < DEBOUNCE_US) {
        return;  // Ignora interrupções muito próximas
    }

    if (gpio == button_A) {
        COUNTER = (COUNTER + 1) % 10;
    } else if (gpio == button_B) {
        COUNTER = (COUNTER - 1 + 10) % 10;
    }

    set_one_led(led_r, led_g, led_b);
    last_interrupt_time = current_time;
}