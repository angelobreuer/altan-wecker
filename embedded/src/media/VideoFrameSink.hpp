#pragma once

#include <media/FrameSink.hpp>
#include <hagl.h>
#include <hagl_hal.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <cstring>

namespace alarm_clock
{
    namespace media
    {
        template <size_t Width, size_t Height, size_t BitsPerPixel>
        class VideoFrameSink : public alarm_clock::media::FrameSink<Width * Height * BitsPerPixel / 8>
        {
        public:
            VideoFrameSink()
            {
                _haglBackend = hagl_init();
                hagl_bitmap_init(&_haglBitmap, Width, Height, BitsPerPixel, nullptr);
            }

            void Process(const std::array<uint8_t, Width * Height * BitsPerPixel / 8> *buffer) override
            {
                _haglBitmap.buffer = const_cast<uint8_t *>(buffer->data());
                hagl_blit(_haglBackend, 0, 0, &_haglBitmap);
                hagl_flush(_haglBackend);
            }

        private:
            constexpr static const uint8_t FRAME_LOADED = (1 << 0);

            hagl_bitmap_t _haglBitmap;
            hagl_backend_t *_haglBackend;
        };
    }
}