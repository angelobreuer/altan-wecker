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
    AlarmManager() : _handle{0}, _alarms{{}}, _times{{}} {
        ESP_LOGI(TAG, "Loading alarms...");

        ESP_ERROR_CHECK(
            nvs_open("alarms", nvs_open_mode_t::NVS_READWRITE, &_handle));
    }

    void Load() {

        size_t length = sizeof(Alarm) * _alarms.size();

        auto result = nvs_get_blob(_handle, "content", _alarms.data(), &length);

        if (result == ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGI(TAG, "Note: NVS for alarms not found.");
            _alarms.fill({});
        } else {
            ESP_ERROR_CHECK(result);
        }

        ComputeTimes();
        ESP_LOGI(TAG, "Loaded alarms (size: %u).", length);
    }

    const std::array<Alarm, 16> *GetAlarms() const { return &_alarms; }
    std::array<Alarm, 16> *GetAlarms() { return &_alarms; }

    void Commit() {
        ESP_LOGI(TAG, "Committing alarms...");

        ESP_ERROR_CHECK(nvs_set_blob(_handle, "content", _alarms.data(),
                                     sizeof(Alarm) * _alarms.size()));
        ESP_ERROR_CHECK(nvs_commit(_handle));

        ComputeTimes();
        ESP_LOGI(TAG, "Committed alarms.");
    }

    ~AlarmManager() { nvs_close(_handle); }

    const Alarm *GetAlarmToInvoke() {
        auto currentTime = std::time(nullptr);

        for (auto index = 0; index < _times.size(); index++) {
            auto previousInvocationTime = _times.at(index);

            if (previousInvocationTime != 0 &&
                currentTime >= previousInvocationTime) {

                _times.at(index) =
                    GetNextInvocationTime(_alarms.at(index), currentTime);

                return &_alarms.at(index);
            }
        }

        return nullptr;
    }

  private:
    nvs_handle_t _handle;
    std::array<Alarm, 16> _alarms;
    std::array<time_t, 16> _times;

    void ComputeTimes() {
        auto currentTime = std::time(nullptr);

        for (auto index = 0; index < _alarms.size(); index++) {
            _times.at(index) =
                GetNextInvocationTime(_alarms.at(index), currentTime);
        }
    }

    time_t GetNextInvocationTime(Alarm alarm, const time_t currentTimeValue) {
        if (!alarm.IsConfigured()) {
            return 0;
        }

        auto now = std::localtime(&currentTimeValue);
        tm currentTime{*now};
        currentTime.tm_sec = 0;
        mktime(&currentTime);

        auto nextDay = [&]() {
            currentTime.tm_mday++;
            mktime(&currentTime);
        };

        AlarmTime current{
            static_cast<uint8_t>(currentTime.tm_hour),
            static_cast<uint8_t>(currentTime.tm_min),
        };

        if (current > alarm.time) {
            nextDay();
        }

        while (true) {
            // Convert the Sunday-Saturday range to Monday-Sunday, then add
            // two to use the correct bit mask offset, and shift one bit
            // accordingly
            const auto flags = static_cast<AlarmFlags>(
                1 << (((currentTime.tm_wday + 6) % 7) + 2));

            if (!alarm.HasFlag(flags)) {
                nextDay();
                continue;
            }

            // adjust invocation time
            currentTime.tm_hour = static_cast<int>(alarm.time.hour);
            currentTime.tm_min = static_cast<int>(alarm.time.minute);

            auto time = mktime(&currentTime);

            if (currentTimeValue >= time) {
                nextDay();
                continue;
            }

            // TODO: may print more than allowed if alarm.name has 32 chars
            ESP_LOGI(TAG, "Alarm '%s' will be invoked at %s.",
                     alarm.name.data(), ctime(&time));

            return time;
        }
    }
};

} // namespace alarm

} // namespace alarm_clock
