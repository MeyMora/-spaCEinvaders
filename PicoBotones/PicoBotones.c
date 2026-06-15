#include "pico/stdlib.h"
#include <stdio.h>

#define BTN_CONTROL 15
#define BTN_SHOOT   14

int main()
{
    stdio_init_all();

    sleep_ms(3000);

    gpio_init(BTN_CONTROL);
    gpio_set_dir(BTN_CONTROL, GPIO_IN);
    gpio_pull_up(BTN_CONTROL);

    gpio_init(BTN_SHOOT);
    gpio_set_dir(BTN_SHOOT, GPIO_IN);
    gpio_pull_up(BTN_SHOOT);

    printf("PICO INICIADA\n");

    absolute_time_t ultimo_toque = get_absolute_time();
    int toques = 0;

    while (true)
    {
        // Botón dirección
        if (!gpio_get(BTN_CONTROL))
        {
            sleep_ms(50);

            if (!gpio_get(BTN_CONTROL))
            {
                toques++;

                printf("TOQUE %d\n", toques);

                while (!gpio_get(BTN_CONTROL))
                {
                    sleep_ms(1);
                }

                ultimo_toque = get_absolute_time();
            }
        }

        // Decidir LEFT o RIGHT
        if (toques > 0 &&
            absolute_time_diff_us(ultimo_toque, get_absolute_time()) > 300000)
        {
            if (toques == 1)
            {
                printf("LEFT\n");
            }
            else
            {
                printf("RIGHT\n");
            }

            toques = 0;
        }

        // Botón disparo
        if (!gpio_get(BTN_SHOOT))
        {
            sleep_ms(50);

            if (!gpio_get(BTN_SHOOT))
            {
                printf("SHOOT\n");

                while (!gpio_get(BTN_SHOOT))
                {
                    sleep_ms(1);
                }
            }
        }

        sleep_ms(10);
    }
}