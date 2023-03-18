#pragma once

#include <atomic>
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
        : _fileSystem{std::make_unique<alarm_clock::memory::FileSystem>()},
          _audioFrameSink{std::make_unique<alarm_clock::media::AudioFrameSink<
              AUDIO_SAMPLE_RATE, AUDIO_CHANNELS, AUDIO_BYTES_PER_SAMPLE>>()},
          _videoFrameSink{std::make_unique<alarm_clock::media::VideoFrameSink<
              VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_BYTES_PER_PIXEL * 8>>()},
          _mediaListener{mediaListener},
          _audioFramePlayout{std::make_unique<AudioFramePlayout>(
              audioFrameBuffer.GetReader(), _audioFrameSink.get())},
          _videoFramePlayout{std::make_unique<VideoFramePlayout>(
              videoFrameBuffer.GetReader(), _videoFrameSink.get())},
          _trackId{NO_TRACK}, _abort{false} {
        xTaskCreate(&PlayInternal, "Media Manager", 4096, this, 10, nullptr);
    }

    void Play(uint32_t trackId) { _trackId = trackId; }
    void Stop() { _trackId = NO_TRACK; }

  private:
    void PlayTrackInternal(uint16_t trackId) {
        ESP_LOGI(TAG, "Playing track: %d.", (int)trackId);

        _mediaListener->OnTrackPlayStart();

        std::array<char, 32> videoPath = {"/sdcard0/video-"};
        std::array<char, 32> audioPath = {"/sdcard0/audio-"};
        itoa((int)trackId, videoPath.data() + 15, 10);
        itoa((int)trackId, audioPath.data() + 15, 10);

        const alarm_clock::media::FrameReader<VIDEO_BYTES_PER_FRAME>
            videoFrameReader(_fileSystem.get(), videoPath.data(),
                             videoFrameBuffer.GetWriter());

        const alarm_clock::media::FrameReader<AUDIO_BYTES_PER_SECOND>
            audioFrameReader(_fileSystem.get(), audioPath.data(),
                             audioFrameBuffer.GetWriter());

        while (_trackId == trackId) {
            vTaskDelay(pdMS_TO_TICKS(100));
        }

        ESP_LOGI(TAG, "Stopped playing track: %d.", (int)trackId);

        _mediaListener->OnTrackPlayEnd();
    }

    void PlayInternal() {
        while (!_abort) {
            uint32_t trackId;
            while ((trackId = _trackId) == NO_TRACK) {
                vTaskDelay(pdMS_TO_TICKS(100));
            }

            PlayTrackInternal((uint16_t)trackId);
        }
    }

    static void PlayInternal(void *pvParameters) {
        static_cast<MediaManager *>(pvParameters)->PlayInternal();
        vTaskDelete(nullptr);
    }

    std::unique_ptr<alarm_clock::memory::FileSystem> _fileSystem;
    std::unique_ptr<alarm_clock::media::AudioFrameSink<
        AUDIO_SAMPLE_RATE, AUDIO_CHANNELS, AUDIO_BYTES_PER_SAMPLE>>
        _audioFrameSink;
    std::unique_ptr<alarm_clock::media::VideoFrameSink<
        VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_BYTES_PER_PIXEL * 8>>
        _videoFrameSink;
    alarm_clock::media::MediaListener *_mediaListener;

    constexpr const static auto AUDIO_PLAYOUT_FREQ = pdMS_TO_TICKS(1000);
    constexpr const static auto VIDEO_PLAYOUT_FREQ =
        pdMS_TO_TICKS(1000 / VIDEO_FRAMERATE);

    typedef FramePlayout<AUDIO_BYTES_PER_SECOND, AUDIO_PLAYOUT_FREQ>
        AudioFramePlayout;

    typedef FramePlayout<VIDEO_BYTES_PER_FRAME, VIDEO_PLAYOUT_FREQ>
        VideoFramePlayout;

    std::unique_ptr<AudioFramePlayout> _audioFramePlayout;
    std::unique_ptr<VideoFramePlayout> _videoFramePlayout;
    std::atomic<uint32_t> _trackId;
    bool _abort;

    constexpr const static uint32_t NO_TRACK = 0xFFFFFFFF;
};
} // namespace media
} // namespace alarm_clock