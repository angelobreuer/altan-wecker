#pragma once

#include <media/FrameSink.hpp>
#include <driver/dac.h>
#include <driver/gptimer.h>
#include <freertos/semphr.h>

namespace alarm_clock
{
    namespace media
    {
        template <size_t SampleRate, size_t Channels, size_t BytesPerSample>
        class AudioFrameSink : public alarm_clock::media::FrameSink<SampleRate * Channels * BytesPerSample>
        {
        public:
            AudioFrameSink() : _sampleCounter{0}, _buffer{nullptr}, _playoutSemaphoreHandle{nullptr}
            {
                dac_output_enable(DAC_CHANNEL_1);

                gptimer_handle_t gptimer = NULL;

                gptimer_config_t timer_config = {
                    .clk_src = GPTIMER_CLK_SRC_DEFAULT,
                    .direction = GPTIMER_COUNT_UP,
                    .resolution_hz = 1 * 1000 * 1000, // 1 MHz
                    .flags = 0,
                };
                ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

                _playoutSemaphoreHandle = xSemaphoreCreateBinary();
                xSemaphoreGive(_playoutSemaphoreHandle);

                gptimer_alarm_config_t alarm_config = {
                    .alarm_count = 1000000 / SampleRate,
                    .reload_count = 0,
                    .flags = {.auto_reload_on_alarm = true},
                };

                ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

                gptimer_event_callbacks_t timerEventCallbacks = {
                    .on_alarm = &DoProcessInterrupt,
                };

                ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &timerEventCallbacks, this));
                ESP_ERROR_CHECK(gptimer_enable(gptimer));
                ESP_ERROR_CHECK(gptimer_start(gptimer));
            }

            ~AudioFrameSink()
            {
                dac_output_voltage(DAC_CHANNEL_1, 0);
                dac_output_disable(DAC_CHANNEL_1);

                vSemaphoreDelete(_playoutSemaphoreHandle);
            }

            void Process(const std::array<uint8_t, SampleRate * Channels * BytesPerSample> *buffer) override
            {
                xSemaphoreTake(_playoutSemaphoreHandle, portMAX_DELAY);

                _sampleCounter = 0;
                _buffer = buffer;
            }

        private:
            volatile uint32_t _sampleCounter;
            const std::array<uint8_t, SampleRate * Channels * BytesPerSample> *volatile _buffer;
            SemaphoreHandle_t _playoutSemaphoreHandle;

            IRAM_ATTR static bool DoProcessInterrupt(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *frameSink)
            {
                auto instance = static_cast<AudioFrameSink<SampleRate, Channels, BytesPerSample> *>(frameSink);
                return instance->ProcessInterrupt();
            }

            IRAM_ATTR bool ProcessInterrupt()
            {
                static_assert(Channels == 1);
                static_assert(BytesPerSample == 1);

                if (!_buffer)
                {
                    return false;
                }

                if (_sampleCounter >= SampleRate)
                {
                    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
                    xSemaphoreGiveFromISR(_playoutSemaphoreHandle, &pxHigherPriorityTaskWoken);
                    _buffer = nullptr;
                    return pxHigherPriorityTaskWoken == pdTRUE;
                }

                dac_output_voltage(DAC_CHANNEL_1, _buffer->at(_sampleCounter++));
                return false;
            }
        };
    }
}