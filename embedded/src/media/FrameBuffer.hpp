#pragma once

#include <memory>
#include <freertos/queue.h>
#include <array>
#include <stdint.h>

namespace alarm_clock
{
    namespace media
    {
        template <size_t UnitSize>
        struct FrameBufferWriter
        {
            FrameBufferWriter(QueueHandle_t availableBufferQueue, QueueHandle_t filledBufferQueue)
                : _availableBufferQueue{availableBufferQueue}, _filledBufferQueue{filledBufferQueue}
            {
            }

            std::array<uint8_t, UnitSize> *Rent()
            {
                std::array<uint8_t, UnitSize> *buffer;

                while (xQueueReceive(_availableBufferQueue, &buffer, portMAX_DELAY) != pdPASS)
                {
                }

                return buffer;
            }

            void Release(std::array<uint8_t, UnitSize> *buffer, bool flush)
            {
                if (flush)
                {
                    xQueueSend(_filledBufferQueue, &buffer, (TickType_t)0);
                }
                else
                {
                    xQueueSend(_availableBufferQueue, &buffer, (TickType_t)0);
                }
            }

        private:
            QueueHandle_t _availableBufferQueue;
            QueueHandle_t _filledBufferQueue;
        };

        template <size_t UnitSize>
        struct FrameBufferReader
        {
            FrameBufferReader(QueueHandle_t availableBufferQueue, QueueHandle_t filledBufferQueue)
                : _availableBufferQueue{availableBufferQueue}, _filledBufferQueue{filledBufferQueue}
            {
            }

            std::array<uint8_t, UnitSize> *Wait()
            {
                std::array<uint8_t, UnitSize> *buffer;

                while (xQueueReceive(_filledBufferQueue, &buffer, 0) != pdPASS)
                {
                }

                return buffer;
            }

            void Release(std::array<uint8_t, UnitSize> *buffer)
            {
                xQueueSend(_availableBufferQueue, &buffer, portMAX_DELAY);
            }

        private:
            QueueHandle_t _availableBufferQueue;
            QueueHandle_t _filledBufferQueue;
        };

        template <size_t UnitSize, size_t Capacity>
        class FrameBuffer
        {
        public:
            FrameBuffer()
            {
                _availableBufferQueue = xQueueCreate(Capacity, sizeof(void *));
                _filledBufferQueue = xQueueCreate(Capacity, sizeof(void *));

                for (auto index = 0; index < Capacity; index++)
                {
                    auto buffer = &data.at(index);
                    xQueueSend(_availableBufferQueue, &buffer, 0);
                }
            }

            inline FrameBufferReader<UnitSize> GetReader()
            {
                return {_availableBufferQueue, _filledBufferQueue};
            }

            inline FrameBufferWriter<UnitSize> GetWriter()
            {
                return {_availableBufferQueue, _filledBufferQueue};
            }

        private:
            std::array<std::array<uint8_t, UnitSize>, Capacity> data;

            QueueHandle_t _availableBufferQueue; // TODO: dispose
            QueueHandle_t _filledBufferQueue;
        };
    }
}