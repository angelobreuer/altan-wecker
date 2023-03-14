#pragma once

#include <stdint.h>

namespace alarm_clock {

namespace alarm {

enum class WeekDayFlags : uint8_t {
    kNone = 0,
    kMonday = 1 << 0,
    kTuesday = 1 << 1,
    kWednesday = 1 << 2,
    kThursday = 1 << 3,
    kFriday = 1 << 4,
    kSaturday = 1 << 5,
    kSunday = 1 << 6,
    kAll = 0xFF,
};

}
} // namespace alarm_clock