#pragma once

#include <stdint.h>

namespace alarm_clock {

namespace alarm {

enum class AlarmToneFlags : uint8_t {
    kNone = 0,
    kCategoryRandom = 1 << 0,
};

}
} // namespace alarm_clock