#pragma once

#include <array>
#include <locale>
#include <stdint.h>

namespace alarm_clock {

namespace alarm {
enum class AlarmFlags : uint16_t {
    kNone = 0,

    kEnabled = 1 << 0,
    kRandomCategory = 1 << 1,

    kMonday = 1 << 2,
    kTuesday = 1 << 3,
    kWednesday = 1 << 4,
    kThursday = 1 << 5,
    kFriday = 1 << 6,
    kSaturday = 1 << 7,
    kSunday = 1 << 8,
};

struct AlarmTime {
    uint8_t hour;
    uint8_t minute;

    bool operator>=(const AlarmTime time) const {
        if (hour == time.hour) {
            return minute >= time.minute;
        }

        return hour >= time.hour;
    }

    bool operator<=(const AlarmTime time) const {
        if (hour == time.hour) {
            return minute <= time.minute;
        }

        return hour <= time.hour;
    }

    bool operator<(const AlarmTime time) const {
        if (hour == time.hour) {
            return minute < time.minute;
        }

        return hour < time.hour;
    }

    bool operator>(const AlarmTime time) const {
        if (hour == time.hour) {
            return minute > time.minute;
        }

        return hour > time.hour;
    }

    bool operator==(const AlarmTime time) const {
        return hour == time.hour && minute == time.minute;
    }

    bool operator!=(const AlarmTime time) const { return !(time == *this); }
};

struct Alarm {
    std::array<char, 32> name;
    AlarmFlags flags;
    AlarmTime time;
    uint16_t toneId;

    inline bool IsEnabled() const { return HasFlag(AlarmFlags::kEnabled); }
    inline void SetEnabled(bool value) { SetFlag(AlarmFlags::kEnabled, value); }
    inline bool IsConfigured() const { return HasFlag(ConfiguredFlags); }

    constexpr const static AlarmFlags ConfiguredFlags =
        static_cast<AlarmFlags>(static_cast<uint16_t>(AlarmFlags::kMonday) |
                                static_cast<uint16_t>(AlarmFlags::kTuesday) |
                                static_cast<uint16_t>(AlarmFlags::kWednesday) |
                                static_cast<uint16_t>(AlarmFlags::kThursday) |
                                static_cast<uint16_t>(AlarmFlags::kFriday) |
                                static_cast<uint16_t>(AlarmFlags::kSaturday) |
                                static_cast<uint16_t>(AlarmFlags::kSunday));

    inline bool HasFlag(AlarmFlags flag) const {
        return static_cast<uint16_t>(flags) & static_cast<uint16_t>(flag);
    }

    inline void SetFlag(AlarmFlags flag, bool value) {
        if (value) {
            flags = static_cast<AlarmFlags>(static_cast<uint16_t>(flags) |
                                            static_cast<uint16_t>(flag));
        } else {
            flags = static_cast<AlarmFlags>(static_cast<uint16_t>(flags) &
                                            ~static_cast<uint16_t>(flag));
        }
    }
};
} // namespace alarm
} // namespace alarm_clock