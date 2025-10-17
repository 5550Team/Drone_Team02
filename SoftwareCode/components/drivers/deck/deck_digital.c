
//#include "deck.h"
#include "driver/gpio.h"
#include "deck_digital.h"
#include "config.h"

void pinMode(uint32_t pin, uint32_t mode)
{
    if (!GPIO_IS_VALID_GPIO((int)pin)) {
        return;
    }

    gpio_config_t io_conf = {
        //bit mask of the pins that you want to set,e.g.GPIO18/19
        .pin_bit_mask = (1ULL << pin),
        //disable pull-down mode
        .pull_down_en = 0,
        //disable pull-up mode
        .pull_up_en = 0,
        //configure GPIO with the given settings
    };
    //set as output mode
    if (mode == OUTPUT) {
        io_conf.mode = GPIO_MODE_OUTPUT;
    }

    if (mode == INPUT_PULLUP) {
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pull_up_en = 1;
    }

    if (mode == INPUT_PULLDOWN) {
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pull_down_en = 1;
    }

    gpio_config(&io_conf);
}

void digitalWrite(uint32_t pin, uint32_t val)
{
    if (!GPIO_IS_VALID_GPIO((int)pin)) {
        return;
    }

    if (val) {
        val = 1;
    }

    gpio_set_level(pin, val);
}

int digitalRead(uint32_t pin)
{
    if (!GPIO_IS_VALID_GPIO((int)pin)) {
        return -1;
    }

    int val = gpio_get_level(pin);
    return val;
}
