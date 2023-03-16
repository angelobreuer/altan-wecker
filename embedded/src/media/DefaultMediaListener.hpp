#pragma once

#include <font5x8.h>
#include <fontx.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <hagl/backend.h>
#include <media/MediaListener.hpp>
#include <media/MediaManager.hpp>

namespace alarm_clock {

namespace media {

class DefaultMediaListener : public MediaListener {
  public:
    DefaultMediaListener() : _show{true} {
        xTaskCreate(&ShowClock, "media listener", 4096, this, 0, nullptr);
    }

    void OnTrackPlayStart() override { _show = true; }
    void OnTrackPlayEnd() override { _show = false; }

  private:
    static void ShowClock(void *pvParameters) {
        fontx_glyph_t glyph;

        auto listener = static_cast<DefaultMediaListener *>(pvParameters);
        auto writer = alarm_clock::media::videoFrameBuffer.GetWriter();

        while (true) {
            while (listener->_show) {
                auto buffer = writer.Rent();
                buffer->fill(0);

                writer.Release(buffer, true);
            }

            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }

    bool _show;

    constexpr const static std::array<uint8_t, 3> digits = {
        0b01110111 /* Zero */, 0b00100100, /* One */
        0b01011101,                        /* Two */
    };
};

} // namespace media

} // namespace alarm_clock
