#include <freertos/task.h>
#include <media/FrameBuffer.hpp>
#include <memory/FileSystem.hpp>

namespace alarm_clock {
namespace media {
template <size_t UnitSize> class FrameReader {
    constexpr static const char *TAG = "frame-reader";

  public:
    FrameReader(
        alarm_clock::memory::FileSystem *fileSystem, const char *path,
        alarm_clock::media::FrameBufferWriter<UnitSize> frameBufferWriter)
        : _abort{false}, _finished{false}, _fileSystem{fileSystem}, _path{path},
          _frameBufferWriter{frameBufferWriter}, _abortSemaphore{
                                                     xSemaphoreCreateBinary()} {
        auto result =
            xTaskCreate(RunTask, "Frame Reader", 3072, this, 100, nullptr);

        if (result == pdFAIL) {
            ESP_LOGE(TAG, "Failed to create frame reader task!");
            abort();
        }
    }

    ~FrameReader() {
        _abort = true;
        xSemaphoreTake(_abortSemaphore, portMAX_DELAY);
        vSemaphoreDelete(_abortSemaphore);
    }

    bool IsFinished() const { return _finished; }

  private:
    static void RunTask(void *pvParameters) {
        auto instance = (FrameReader<UnitSize> *)pvParameters;
        instance->Run(&instance->_abort);
        vTaskDelete(nullptr);
    }

    void Run(bool *abort) {
        ESP_LOGI(TAG, "Frame reader started.");

        auto fileHandle = _fileSystem->OpenFile(_path);

        while (!*abort) {
            auto frameBuffer = _frameBufferWriter.Rent();

            const auto bytesRead = _fileSystem->Read(
                fileHandle.get(), frameBuffer->data(), frameBuffer->size());

            if (bytesRead < frameBuffer->size()) {
                _frameBufferWriter.Release(frameBuffer, false);
                ESP_LOGI(TAG, "Frame reader finished reading.");
                break;
            }

            _frameBufferWriter.Release(frameBuffer, true);
            vPortYield();
        }

        xSemaphoreGive(_abortSemaphore);
        _finished = true;
    }

    bool _abort;
    bool _finished;
    alarm_clock::memory::FileSystem *_fileSystem;
    const char *_path;
    alarm_clock::media::FrameBufferWriter<UnitSize> _frameBufferWriter;
    SemaphoreHandle_t _abortSemaphore;
};
} // namespace media
} // namespace alarm_clock