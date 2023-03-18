#pragma once

#include <button/Button.hpp>
#include <media/MediaManager.hpp>

namespace alarm_clock {

namespace button {

class ButtonController {

  public:
    ButtonController(alarm_clock::media::MediaManager *mediaManager)
        : _abort{false}, _button{GPIO_NUM_26}, _mediaManager{mediaManager} {
        xTaskCreate(&RunTaskInternal, "Button", 2048, this, 1000, nullptr);
    }

    ~ButtonController() { _abort = true; }

  private:
    static constexpr const char TAG[] = "button";

    void RunTask() {
        while (!_abort) {
            vTaskDelay(pdMS_TO_TICKS(10));

            if (!_button.TryDetectPress()) {
                continue;
            }

            ESP_LOGI(TAG, "Button pressed.");
            _mediaManager->Stop();
        }
    }

    static void RunTaskInternal(void *pvParameters) {
        static_cast<ButtonController *>(pvParameters)->RunTask();
        vTaskDelete(nullptr);
    }

    bool _abort;
    alarm_clock::button::Button _button;
    alarm_clock::media::MediaManager *_mediaManager;
};

} // namespace button

} // namespace alarm_clock
