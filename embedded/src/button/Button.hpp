#pragma once

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace alarm_clock {

namespace button {

class Button {
  public:
    Button(gpio_num_t pin) : _pin{pin}, _pressedSince{0}, _state{0} {
        gpio_set_direction(pin, gpio_mode_t::GPIO_MODE_INPUT);
    }

    bool TryDetectPress() {
        const auto level = gpio_get_level(_pin) == 0;

        if (!level) {
            _state = 0;
            return false;
        }

        if (_state == 2) {
            // waiting for release
            return false;
        }

        if (_state == 0) {
            _state = 1;
            _pressedSince = xTaskGetTickCount();
            return false;
        }

        if (xTaskGetTickCount() - _pressedSince > pdMS_TO_TICKS(100)) {
            _state = 2;
            return true;
        }

        return false;
    }

  private:
    gpio_num_t _pin;
    TickType_t _pressedSince;
    uint8_t
        _state; // 0 = not pressed, 1 = pressed, 2 = pressed, wait for release
};

} // namespace button

} // namespace alarm_clock
