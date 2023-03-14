#pragma once

#include "AlarmToneFlags.hpp"
#include "WeekDayFlags.hpp"

#include <array>
#include <stdint.h>

namespace alarm_clock {

namespace alarm {

struct Alarm {
    std::array<char, 32> name;
    bool enabled;
    uint8_t hour;
    uint8_t minute;
    WeekDayFlags weekDays;
    uint16_t toneId;
    AlarmToneFlags toneFlags;
};
} // namespace alarm
} // namespace alarm_clock