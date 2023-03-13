#pragma once

#include <media/FrameBuffer.hpp>
#include <media/FrameSink.hpp>
#include <freertos/task.h>

namespace alarm_clock
{
    namespace media
    {
        template <size_t UnitSize, size_t TickFrequency>
        class FramePlayout
        {
            constexpr static const char *TAG = "frame-playout";

        public:
            FramePlayout(
                alarm_clock::media::FrameBufferReader<UnitSize> frameBufferReader,
                alarm_clock::media::FrameSink<UnitSize> *frameSink)
                : _abort{false}, _frameBufferReader{frameBufferReader}, _frameSink{frameSink}
            {
                auto result = xTaskCreate(RunTask, "Frame Playout", 2048, this, 500, nullptr);

                if (result == pdFAIL)
                {
                    ESP_LOGE(TAG, "Failed to create frame reader task!");
                    abort();
                }
            }

            ~FramePlayout()
            {
                _abort = true;
            }

        private:
            static void RunTask(void *pvParameters)
            {
                auto instance = static_cast<FramePlayout<UnitSize, TickFrequency> *>(pvParameters);
                instance->Run(&instance->_abort);
            }

            void Run(bool *abort)
            {
                auto lastWakeTime = xTaskGetTickCount();
                ESP_LOGI(TAG, "Frame playout started (%d bytes).", UnitSize);

                while (!*abort)
                {
                    auto frameBuffer = _frameBufferReader.Wait();
                    _frameSink->Process(frameBuffer);
                    _frameBufferReader.Release(frameBuffer);
                    vTaskDelayUntil(&lastWakeTime, TickFrequency);
                }

                vTaskDelete(nullptr);
            }

            bool _abort;
            alarm_clock::media::FrameBufferReader<UnitSize> _frameBufferReader;
            alarm_clock::media::FrameSink<UnitSize> *_frameSink;
        };
    }
}