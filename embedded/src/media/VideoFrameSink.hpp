#pragma once

#include <cstring>
#include <freertos/event_groups.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <media/FrameSink.hpp>
#include <mipi_display.h>

static spi_device_handle_t spi;

namespace alarm_clock {
namespace media {
template <size_t Width, size_t Height, size_t BitsPerPixel>
class VideoFrameSink
    : public alarm_clock::media::FrameSink<Width * Height * BitsPerPixel / 8> {
  public:
    VideoFrameSink() { mipi_display_init(&spi); }

    void Process(const std::array<uint8_t, Width * Height * BitsPerPixel / 8>
                     *buffer) override {
        mipi_display_write(spi, 0, 0, Width, Height,
                           const_cast<uint8_t *>(buffer->data()));
    }

  private:
    constexpr static const uint8_t FRAME_LOADED = (1 << 0);
};
} // namespace media
} // namespace alarm_clock