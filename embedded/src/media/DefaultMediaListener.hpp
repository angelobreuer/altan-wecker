#pragma once

#include <ctime>
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
    static void Fill(std::array<uint8_t, VIDEO_BYTES_PER_FRAME> *buffer, int x,
                     int y, int w, int h) {
        constexpr auto stride = VIDEO_WIDTH * sizeof(uint16_t);
        auto ptr = &buffer->data()[(x * sizeof(uint16_t)) + (y * stride)];

        for (auto y = 0; y < h; y++) {
            memset(ptr, 0xFF, w);
            ptr += stride;
        }
    }

    static void DrawSegment(std::array<uint8_t, VIDEO_BYTES_PER_FRAME> *buffer,
                            int segmentIndex, int xOffset) {
        const auto segment = _segments.at(segmentIndex);

        auto offsetY = xOffset < 0 ? VIDEO_HEIGHT + xOffset * 36 : xOffset * 36;

        auto x0 = segment.x;
        auto y0 = segment.y + offsetY;

        Fill(buffer, x0, y0, segment.w, segment.h);
    }

    static void DrawDot(std::array<uint8_t, VIDEO_BYTES_PER_FRAME> *buffer,
                        int yOffset) {
        constexpr const auto DOT_SIZE = 4;
        Fill(buffer, VIDEO_WIDTH / 2 - DOT_SIZE / 2 + yOffset,
             VIDEO_HEIGHT / 2 + DOT_SIZE / 2, DOT_SIZE, DOT_SIZE);
    }

    static void DrawDigit(std::array<uint8_t, VIDEO_BYTES_PER_FRAME> *buffer,
                          int digit, int xOffset) {
        const auto mask = digits.at(digit);

        for (auto index = 0; index < 7; index++) {
            if (mask & (1 << index)) {
                DrawSegment(buffer, index, xOffset);
            }
        }
    }

    static void ShowClock(void *pvParameters) {
        static bool blink = false;
        auto listener = static_cast<DefaultMediaListener *>(pvParameters);
        auto writer = alarm_clock::media::videoFrameBuffer.GetWriter();

        while (true) {
            while (listener->_show) {
                auto buffer = writer.Rent();
                buffer->fill(0);

                std::time_t time = std::time(0);
                std::tm *now = std::localtime(&time);

                const auto hourPart1 = now->tm_hour / 10;
                const auto hourPart2 = now->tm_hour % 10;
                const auto minutePart1 = now->tm_min / 10;
                const auto minutePart2 = now->tm_min % 10;

                DrawDigit(buffer, hourPart1, 0);
                DrawDigit(buffer, hourPart2, 1);
                DrawDigit(buffer, minutePart1, -2);
                DrawDigit(buffer, minutePart2, -1);

                blink = !blink;

                if (blink) {
                    DrawDot(buffer, -10);
                    DrawDot(buffer, 10);
                }

                writer.Release(buffer, true);
                vTaskDelay(pdMS_TO_TICKS(1000));
            }

            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }

    bool _show;

    constexpr const static std::array<uint8_t, 10> digits = {
        // 1 << 0: Top Horizontal
        // 1 << 1: Middle Horizontal
        // 1 << 2: Bottom Horizontal
        // 1 << 3: Bottom Left
        // 1 << 4: Top Left
        // 1 << 5: Bottom Right
        // 1 << 6: Top Right

        0b01111101 /* Zero */,
        0b01100000, /* One */
        0b01001111, /* Two */
        0b01100111, /* Three */
        0b01110010, /* Four */
        0b00110111, /* Five */
        0b00111111, /* Six */
        0b01100001, /* Seven */
        0b01111111, /* Eight */
        0b01110111, /* Nine */
    };

    struct Rectangle {
        int x;
        int y;
        int w;
        int h;
    };

    constexpr const static auto SEGMENT_SIZE = 10;
    constexpr const static auto SEGMENT_LENGTH = 26;
    constexpr const static auto PADDING_HORIZONTAL = 5;
    constexpr const static auto PADDING_VERTICAL = 40;

    constexpr const static std::array<Rectangle, 7> _segments = {
        (Rectangle{
            VIDEO_WIDTH - PADDING_VERTICAL,
            PADDING_HORIZONTAL + SEGMENT_SIZE / 4,
            SEGMENT_SIZE,
            SEGMENT_LENGTH,
        }), // Top Horizontal

        (Rectangle{
            VIDEO_WIDTH / 2,
            PADDING_HORIZONTAL + SEGMENT_SIZE / 4,
            SEGMENT_SIZE,
            SEGMENT_LENGTH,
        }), // Middle Horizontal

        (Rectangle{
            PADDING_VERTICAL,
            PADDING_HORIZONTAL + SEGMENT_SIZE / 4,
            SEGMENT_SIZE,
            SEGMENT_LENGTH,
        }), // Bottom Horizontal

        (Rectangle{
            PADDING_VERTICAL + SEGMENT_SIZE / 4,
            PADDING_HORIZONTAL,
            SEGMENT_LENGTH * 2,
            SEGMENT_SIZE / 2,
        }), // Bottom Left

        (Rectangle{
            VIDEO_WIDTH - PADDING_VERTICAL - SEGMENT_LENGTH + SEGMENT_SIZE / 4,
            PADDING_HORIZONTAL,
            SEGMENT_LENGTH * 2,
            SEGMENT_SIZE / 2,
        }), // Top Left

        (Rectangle{
            PADDING_VERTICAL + SEGMENT_SIZE / 4,
            SEGMENT_LENGTH + PADDING_HORIZONTAL,
            SEGMENT_LENGTH * 2,
            SEGMENT_SIZE / 2,
        }), // Bottom Right

        (Rectangle{
            VIDEO_WIDTH - PADDING_VERTICAL - SEGMENT_LENGTH + SEGMENT_SIZE / 4,
            SEGMENT_LENGTH + PADDING_HORIZONTAL,
            SEGMENT_LENGTH * 2,
            SEGMENT_SIZE / 2,
        }), // Top Right
    };
};

} // namespace media

} // namespace alarm_clock
