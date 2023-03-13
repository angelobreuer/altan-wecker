#pragma once

#include <stdlib.h>
#include <array>

namespace alarm_clock
{
    namespace media
    {
        template <size_t UnitSize>
        class FrameSink
        {
        public:
            virtual void Process(const std::array<uint8_t, UnitSize> *buffer) = 0;
        };

    } // namespace media

} // namespace alarm_clock
