#pragma once

#include "Alarm.hpp"

#include <array>
#include <nvs.h>
#include <stdint.h>

#include "esp_log.h"

namespace alarm_clock {

namespace alarm {

class AlarmManager {
    constexpr const static char TAG[] = "alarm-manager";

  public:
    AlarmManager() : _handle{0}, _alarms{{}} {
        ESP_LOGI(TAG, "Loading alarms...");

        ESP_ERROR_CHECK(
            nvs_open("alarms", nvs_open_mode_t::NVS_READWRITE, &_handle));

        size_t length = sizeof(Alarm) * _alarms.size();

        auto result = nvs_get_blob(_handle, "content", _alarms.data(), &length);

        if (result == ESP_ERR_NVS_NOT_FOUND) {
            _alarms.fill({});
        } else {
            ESP_ERROR_CHECK(result);
        }

        ESP_LOGI(TAG, "Loaded alarms.");
    }

    const std::array<Alarm, 16> &GetAlarms() const { return _alarms; }
    std::array<Alarm, 16> &GetAlarms() { return _alarms; }

    void Commit() {
        ESP_LOGI(TAG, "Committing alarms...");

        nvs_set_blob(_handle, "content", _alarms.data(),
                     sizeof(Alarm) * _alarms.size());
        nvs_commit(_handle);

        ESP_LOGI(TAG, "Committed alarm.");
    }

    ~AlarmManager() { nvs_close(_handle); }

  private:
    nvs_handle_t _handle;
    std::array<Alarm, 16> _alarms;
};

} // namespace alarm

} // namespace alarm_clock
