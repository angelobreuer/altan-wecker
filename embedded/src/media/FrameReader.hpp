#include <memory/FileSystem.hpp>
#include <media/FrameBuffer.hpp>
#include <freertos/task.h>

namespace alarm_clock
{
    namespace media
    {
        template <size_t UnitSize>
        class FrameReader
        {
            constexpr static const char *TAG = "frame-reader";

        public:
            FrameReader(
                alarm_clock::memory::FileSystem *fileSystem,
                const char *path,
                alarm_clock::media::FrameBufferWriter<UnitSize> frameBufferWriter)
                : _abort{false}, _fileSystem{fileSystem}, _fileHandle{fileSystem->OpenFile(path)}, _frameBufferWriter{frameBufferWriter}
            {
                auto result = xTaskCreate(RunTask, "Frame Reader", 4096, this, 100, nullptr);

                if (result == pdFAIL)
                {
                    ESP_LOGE(TAG, "Failed to create frame reader task!");
                    abort();
                }
            }

            ~FrameReader()
            {
                _abort = true;
            }

        private:
            static void RunTask(void *pvParameters)
            {
                auto instance = (FrameReader<UnitSize> *)pvParameters;
                instance->Run(&instance->_abort);
            }

            void Run(bool *abort)
            {
                ESP_LOGI(TAG, "Frame reader started.");

                while (!*abort)
                {
                    auto frameBuffer = _frameBufferWriter.Rent();
                    auto bytesRead = _fileSystem->Read(_fileHandle.get(), frameBuffer->data(), frameBuffer->size());

                    if (bytesRead < frameBuffer->size())
                    {
                        _frameBufferWriter.Release(frameBuffer, false);
                        ESP_LOGI(TAG, "Frame reader finished reading.");
                        break;
                    }

                    _frameBufferWriter.Release(frameBuffer, true);
                    vPortYield();
                }

                vTaskDelete(nullptr);
            }

            bool _abort;
            alarm_clock::memory::FileSystem *_fileSystem;
            alarm_clock::memory::FileHandle _fileHandle;
            alarm_clock::media::FrameBufferWriter<UnitSize> _frameBufferWriter;
        };
    }
}