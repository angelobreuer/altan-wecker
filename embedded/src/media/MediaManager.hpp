#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <media/AudioFrameSink.hpp>
#include <media/FrameBuffer.hpp>
#include <media/FramePlayout.hpp>
#include <media/FrameReader.hpp>
#include <media/MediaListener.hpp>
#include <media/VideoFrameSink.hpp>
#include <memory/FileSystem.hpp>

namespace alarm_clock {
namespace media {
// 128x160 RGB24 8fps
constexpr const static size_t VIDEO_WIDTH = 128;
constexpr const static size_t VIDEO_HEIGHT = 160;
constexpr const static size_t VIDEO_BYTES_PER_PIXEL = 2;
constexpr const static size_t VIDEO_BYTES_PER_FRAME =
    VIDEO_BYTES_PER_PIXEL * VIDEO_HEIGHT * VIDEO_WIDTH;
constexpr const static size_t VIDEO_FRAMERATE = 8;

// PCM U8 16 kHz
constexpr const static size_t AUDIO_BYTES_PER_SAMPLE = 1;
constexpr const static size_t AUDIO_SAMPLE_RATE = 16000; // samples per second
constexpr const static size_t AUDIO_CHANNELS = 1;        // mono
constexpr const static size_t AUDIO_BYTES_PER_SECOND =
    AUDIO_SAMPLE_RATE * AUDIO_BYTES_PER_SAMPLE * AUDIO_CHANNELS;

constexpr const static gpio_num_t PIN_AUDIO_OUTPUT =
    GPIO_NUM_25; // DAC CHANNEL 1

alarm_clock::media::FrameBuffer<VIDEO_BYTES_PER_FRAME, 2> videoFrameBuffer;
alarm_clock::media::FrameBuffer<AUDIO_BYTES_PER_SECOND, 2> audioFrameBuffer;

constexpr const static char *TAG = "media";

class MediaManager {
  public:
    MediaManager(alarm_clock::media::MediaListener *mediaListener)
        : _version{0},
          _fileSystem{std::make_unique<alarm_clock::memory::FileSystem>()},
          _audioFrameSink{std::make_unique<alarm_clock::media::AudioFrameSink<
              AUDIO_SAMPLE_RATE, AUDIO_CHANNELS, AUDIO_BYTES_PER_SAMPLE>>()},
          _videoFrameSink{std::make_unique<alarm_clock::media::VideoFrameSink<
              VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_BYTES_PER_PIXEL * 8>>()},
          _mediaListener{mediaListener},
          _audioFramePlayout{std::make_unique<AudioFramePlayout>(
              audioFrameBuffer.GetReader(), _audioFrameSink.get())},
          _videoFramePlayout{std::make_unique<VideoFramePlayout>(
              videoFrameBuffer.GetReader(), _videoFrameSink.get())} {}

    void Play(uint32_t trackId) {
        _version++;
        _trackId = trackId;

        xSemaphoreGive(_playSemaphore);
        xTaskCreate(&PlayInternal, "Media Manager", 4096, this, 10, nullptr);
    }

  private:
    static void PlayInternal(void *pvParameters) {
        auto manager = static_cast<MediaManager *>(pvParameters);

        const uint16_t trackId = manager->_trackId;
        const int32_t currentVersion = ++manager->_version;

        ESP_LOGI(TAG, "Playing track: %d.", trackId);

        alarm_clock::media::AudioFrameSink<AUDIO_SAMPLE_RATE, AUDIO_CHANNELS,
                                           AUDIO_BYTES_PER_SAMPLE>
            audioFrameSink;

        std::array<char, 64> path = {"/sdcard0/video-"};
        itoa((int)trackId, path.data() + 15, 10);

        alarm_clock::media::FrameReader<VIDEO_BYTES_PER_FRAME> videoFrameReader(
            manager->_fileSystem.get(), path.data(),
            videoFrameBuffer.GetWriter());

        path = {"/sdcard0/audio-"};
        itoa((int)trackId, path.data() + 15, 10);
        manager->_mediaListener->OnTrackPlayStart();

        alarm_clock::media::FrameReader<AUDIO_BYTES_PER_SECOND>
            audioFrameReader(manager->_fileSystem.get(), path.data(),
                             audioFrameBuffer.GetWriter());

        xSemaphoreTake(manager->_playSemaphore, portMAX_DELAY);

        ESP_LOGI(TAG, "Stopped playing track: %d.", trackId);

        manager->_mediaListener->OnTrackPlayEnd();
        vTaskDelete(nullptr);
    }

    uint16_t _trackId;
    int32_t _version;

    std::unique_ptr<alarm_clock::memory::FileSystem> _fileSystem;
    std::unique_ptr<alarm_clock::media::AudioFrameSink<
        AUDIO_SAMPLE_RATE, AUDIO_CHANNELS, AUDIO_BYTES_PER_SAMPLE>>
        _audioFrameSink;
    std::unique_ptr<alarm_clock::media::VideoFrameSink<
        VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_BYTES_PER_PIXEL * 8>>
        _videoFrameSink;
    alarm_clock::media::MediaListener *_mediaListener;
    SemaphoreHandle_t _playSemaphore;

    constexpr const static auto AUDIO_PLAYOUT_FREQ = pdMS_TO_TICKS(1000);
    constexpr const static auto VIDEO_PLAYOUT_FREQ =
        pdMS_TO_TICKS(1000 / VIDEO_FRAMERATE);

    typedef FramePlayout<AUDIO_BYTES_PER_SECOND, AUDIO_PLAYOUT_FREQ>
        AudioFramePlayout;

    typedef FramePlayout<VIDEO_BYTES_PER_FRAME, VIDEO_PLAYOUT_FREQ>
        VideoFramePlayout;

    std::unique_ptr<AudioFramePlayout> _audioFramePlayout;
    std::unique_ptr<VideoFramePlayout> _videoFramePlayout;
};
} // namespace media
} // namespace alarm_clock