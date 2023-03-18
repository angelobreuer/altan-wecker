#pragma once

#include <alarm/AlarmManager.hpp>
#include <media/MediaManager.hpp>

namespace alarm_clock {

namespace alarm {

class AlarmController {

  public:
    AlarmController(alarm_clock::alarm::AlarmManager *alarmManager,
                    alarm_clock::media::MediaManager *mediaManager)
        : _abort{false}, _alarmManager{alarmManager}, _mediaManager{
                                                          mediaManager} {
        xTaskCreate(&RunTaskInternal, "Button", 2048, this, 1000, nullptr);
    }

    ~AlarmController() { _abort = true; }

  private:
    static constexpr const char TAG[] = "alarm-controller";

    void RunTask() {
        while (!_abort) {
            vTaskDelay(pdMS_TO_TICKS(1000));

            const auto alarm = _alarmManager->GetAlarmToInvoke();

            if (alarm != nullptr) {
                ESP_LOGI(TAG, "Invoking alarm: %s.", alarm->name.data());
                _mediaManager->Play(alarm->toneId);
            }
        }
    }

    static void RunTaskInternal(void *pvParameters) {
        static_cast<AlarmController *>(pvParameters)->RunTask();
        vTaskDelete(nullptr);
    }

    bool _abort;
    alarm_clock::alarm::AlarmManager *_alarmManager;
    alarm_clock::media::MediaManager *_mediaManager;
};

} // namespace alarm

} // namespace alarm_clock
