#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "pico/multicore.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#include "ws2812.pio.h"

#include "include/raspberry26x32.h"
#include "include/ssd1306_font.h"

#include "include/pin.h"
#include "include/led.h"
#include "include/button.h"
#include "include/ws2812.h"
#include "include/ssd1306_i2c.h"

volatile uint8_t led_color = MIN_LED;

volatile uint8_t number = MIN_NUMBER;

volatile bool led_green_state = false;
volatile bool led_blue_state = false;

// Função para inicializar o LED RGB
void led_init() {    
    gpio_init(LED_RGB_RED_PIN);
    gpio_set_dir(LED_RGB_RED_PIN, GPIO_OUT);
    gpio_init(LED_RGB_GREEN_PIN);
    gpio_set_dir(LED_RGB_GREEN_PIN, GPIO_OUT);
    gpio_init(LED_RGB_BLUE_PIN);
    gpio_set_dir(LED_RGB_BLUE_PIN, GPIO_OUT);
}

// Função para inicializar os botões
void button_init() {
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    // Configura interrupções para os botões
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_button_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_button_callback);
}

// Função para inicializar o controlador WS2812
void ws2812_init() {
    // Inicializa o controlador WS2812
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, false);
}

void core1_entry() {
    blink_led();
}

void switch_led() {
    if (led_color < MAX_LED) {
        led_color++;
    } else {
        led_color = MIN_LED;
    }
    
    // Reiniciar o núcleo 1 para atualizar a cor do LED
    multicore_reset_core1();

    // Iniciar o núcleo 1 para piscar o LED com a nova cor
    multicore_launch_core1(core1_entry);
}

// Função para verificar se ambos os botões foram pressionados
void check_both_buttons() {   
    if (button_a_pressed && button_b_pressed) {
        button_a_pressed = false;
        button_b_pressed = false;
        
        number = 0;

        switch_led();

        ws2812_clear();
        ws2812_draw();
    }

    sleep_ms(20);
}

// Implement the logic to display the symbol on the WS2812 matrix
void display_symbol(uint8_t number) {
    // This is a placeholder function
    ws2812_clear();
    ws2812_draw_number(number);
}

void toggle_green_led(struct render_area *frame_area) {
    led_green_state = !led_green_state;
    gpio_put(LED_RGB_GREEN_PIN, led_green_state);

    char message[32];
    snprintf(message, sizeof(message), "LED Verde: %s", led_green_state ? "Ligado" : "Desligado");

    uint8_t buf[SSD1306_BUF_LEN];
    memset(buf, 0, SSD1306_BUF_LEN);
    render(buf, frame_area);

    WriteString(buf, 0, 0, message);
    render(buf, frame_area);

    printf("%s\n", message);
}

void toggle_blue_led(struct render_area *frame_area) {
    led_blue_state = !led_blue_state;
    gpio_put(LED_RGB_BLUE_PIN, led_blue_state);

    char message[32];
    snprintf(message, sizeof(message), "LED Azul: %s", led_blue_state ? "Ligado" : "Desligado");

    uint8_t buf[SSD1306_BUF_LEN];
    memset(buf, 0, SSD1306_BUF_LEN);
    render(buf, frame_area);

    WriteString(buf, 0, 0, message);
    render(buf, frame_area);

    printf("%s\n", message);
}

int main() {
    stdio_init_all();

    // Inicializa os pinos do LED RGB
    led_init();

    // Inicializa os pinos dos botões com pull-up
    button_init();

    // Inicializa o controlador WS2812
    ws2812_init();
    
    #if !defined(i2c_default) || !defined(I2C_SDA_PIN) || !defined(I2C_SCL_PIN)
    
    #warning i2c / SSD1306_i2c example requires a board with I2C pins
    puts("Default I2C pins were not defined");
    
    #else

    bi_decl(bi_2pins_with_func(I2C_SDA_PIN, I2C_SCL_PIN, GPIO_FUNC_I2C));
    bi_decl(bi_program_description("SSD1306 OLED driver I2C example for the Raspberry Pi Pico"));

    printf("Hello, SSD1306 OLED display! Look at my raspberries..\n");

    i2c_init(i2c_default, SSD1306_I2C_CLK * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    SSD1306_init();

    struct render_area frame_area = {
        .start_col = 0,
        .end_col = SSD1306_WIDTH - 1,
        .start_page = 0,
        .end_page = SSD1306_NUM_PAGES - 1
    };

    calc_render_area_buflen(&frame_area);

    uint8_t buf[SSD1306_BUF_LEN];
    memset(buf, 0, SSD1306_BUF_LEN);
    render(buf, &frame_area);

    led_init();
    button_init();
    ws2812_init();

    while (true) {
        if (stdio_usb_connected()) {
            int c = getchar_timeout_us(0);

            if (c != PICO_ERROR_TIMEOUT) {
                SSD1306_clear();
                WriteChar(buf, 0, 0, (char)c);
                render(buf, &frame_area);

                if (c >= '0' && c <= '9') {
                    display_symbol(c - '0');
                }
            }
        }

        if (button_a_pressed && !button_b_pressed) {
            button_a_pressed = false;
            toggle_green_led(&frame_area);
        }

        if (button_b_pressed && !button_a_pressed) {
            button_b_pressed = false;
            toggle_blue_led(&frame_area);
        }

        check_both_buttons();
    }
    #endif

    return 0;
}